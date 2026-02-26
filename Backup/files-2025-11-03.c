#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "global.h"
#include "futils.h"

#define OK 0
#define TIMEOUT 1
#define CANCEL 2

#define SOH 1
#define EOT 4
#define ACK 6
#define NAK 21
#define CAN 24
#define PAD 0

#define JCH 160
#define JCM 161
#define JCL 162

#define F1 133
#define F3 134
#define F5 135
#define F7 136

#define MAXRETRIES 10

/* local functions */
void rec_file(void);
void snd_file(void);
char inbyte(char, char*);
void outbyte(char);
void enjoythesilence(void);
void pad(char*);

void main(void) {

    char id, action;
    char running = TRUE;

    LOCALMODE = PEEK(BS_LOCALMODE);
    action = PEEK(BS_ACTION);

    if (PEEK(BS_MODULE) == 4) {

        if (LOCALMODE == TRUE) {

            print("\n\nYou are local!\n");
            pause(FALSE);

        } else {

            ONLINE = carrierdetect();

            if (ONLINE == TRUE) {

                id = PEEK(BS_ID);

                if (id > 0) {
                    loaduser(id);

                    if (U.SECURITY >= 3) {

                        switch (action) {

                            case 1:
                                /* DOWNLOAD! */
                                snd_file();
                                break;
                            case 2:
                                /* UPLOAD! */
                                rec_file();
                                break;
                            default:
                                /* do nothing! */
                                print("\n\nHuh?\n\n");

                        }
                    }
                }
            }
        }

        POKE(BS_ACTION, 0);

        bootstrap("BBS");

    } else {
        print("\nLoad BBS instead.\n");
    }


}

void rec_file(void) {

    char ftype, packet[131], filename[20], ch, i, m, e, blk, chk, checksum, badbytes, t;
    char receiving = TRUE, p = 1, retries = 0, status = OK, starting = TRUE, go = FALSE;
    char scratch[20];

    print("\223\034\022XMODEM\222\005UPLOAD\n\n");

    print("Enter filename\034:\005\n>");
    input('A', 0, 16, FALSE);

    if (strlen(I) > 0) {
        go = TRUE;
    }

    if (go == TRUE) {

        strcpy(scratch, I);

        print("\nType (\034P\005/\034S\005/\034U\005)\034?\005\n>");
        ch = get_command();
        ftype = tolower(ch);

        if ((ftype != 'p') && (ftype != 's') && (ftype != 'u')) {
            go = FALSE;
        }

    }

    if (go == TRUE) {

        cbm_open(15, 8, 15, "");
        cbm_open(3, 8, 3, "1:uploads,a");

        sprintf(O, "\022%s\222 %c\n", scratch, ftype);
        cbm_write(3, O, strlen(O));

        print("\n4 Line Desc:\n");
        print("(/s to save)\n");
        clear(W);

        for (t = 0; t < 4; t++) {

            if (strlen(W) > 0) {
                strcpy(I, W);
                clear(W);
                print(I);
                input('W', 0, 21, TRUE);
            } else {
                input('W', 0, 21, FALSE);
            }

            if ((strlen(I) > 0) || (strcmp(strupper(I), "/S") == 0)) {
                strcat(I, "\n");
                cbm_write(3, I, strlen(I));
            } else {

                if (t == 0) {
                    go = FALSE;
                }
                t = 4;
                break;
            }
        }

        cbm_close(3);
        cbm_close(15);

    }

    if (go == TRUE) {

        sprintf(filename, "3:%s,%c,w", scratch, ftype);

        cbm_open(15, 8, 15, "");
        cbm_open(3, 8, 3, filename);

        e = errorcheck();

        if (e == 0) {

            print("\n\nStart your upload!");

            for (t = 0; t < MAXRETRIES; t++) {

                /* send NAK to initiate transfer */
                outbyte(NAK);

                /* get control byte */
                ch = inbyte('M', &status);

                if ((ch != 0) || (status == OK)) {
                    t = MAXRETRIES;
                    break;
                }

            }

            do {

                if (starting == TRUE) {
                    starting = FALSE;
                } else {
                    ch = inbyte('M', &status);
                }

                if ((ch == 1) && (status == OK)) {

                    /* SOH received! */

                    /* get packet! */
                    sprintf(I, "\n\n\236Block #%i:", p); lprint(I);
                    pad(packet);
                    badbytes = 0;
                    m = 0;

                    for (i = 0; i <= 130; i++) {
                        ch = inbyte('S', &status);
                        if (status == OK) {
                            packet[i] = ch;
                            if ((i >= 2) && (i <= 129)) {
                                m = m + ch;
                            }
                            lprint("\237+");
                        } else {
                            lprint("\034.");
                            badbytes++;
                        }
                    }

                    if (badbytes == 0) {

                        blk = packet[0];
                        chk = packet[1];
                        checksum = packet[130];

                        /* validate packet */
                        if ((blk == p) && (255 - chk == blk) && (checksum == m)) {

                            /* its good! save it! */
                            for (i = 2; i <= 129; ++i) {
                                cbm_k_ckout(3);
                                cbm_k_bsout(packet[i]);
                                cbm_k_clrch();
                            }
                            lprint("\036\272");
                            p++;
                            retries = 0;
                            outbyte(ACK);

                        } else {
                            badbytes++;
                        }

                    }

                    if (badbytes > 0) {

                        /* bad packet! */
                        lprint("\034X");
                        /* enjoythesilence(); */
                        outbyte(NAK);

                        retries++;

                    }


                } else if ((ch == 4) && (status == OK)) {

                    /* EOT received! */
                    lprint("\036E");
                    outbyte(ACK);
                    receiving = FALSE;

                } else if ((ch == 24) && (status == OK)) {

                    retries = MAXRETRIES;

                } else {

                    /* unexpected packet! */
                    lprint("\034?");
                    /* enjoythesilence(); */
                    outbyte(NAK);

                    retries++;

                }

                if (retries > MAXRETRIES) {
                    status = CANCEL;
                    lprint("\034C");
                    outbyte(CAN);
                    outbyte(CAN);
                    outbyte(CAN);
                    receiving = FALSE;
                    break;
                }

            } while (receiving == TRUE);

            if (status == OK) {
                print("\n\n\005SUCCCES!\n");
            } else {
                print("\n\n\005FAILURE!\n");
            }

        }

        cbm_close(3);
        if (status != OK) {
            sprintf(O, "s3:%s", scratch);
            cbm_write(15, O, strlen(O));
        }
        cbm_close(15);

    } else {

        print("\n\nABORTED!\n\n");

    }

    beep();
    putch(7);

    pause(FALSE);

}

void snd_file(void) {

    char packet[128], ch, i, j, m, e, st, t;
    char filename[20], sending = TRUE, p = 1, retries = 0, status = OK, retry = FALSE;

    print("\223\034\022XMODEM\222\005DOWNLOAD\n\n");

    print("Enter filename\034:\005\n>");
    input('A', 0, 16, FALSE);

    if (strlen(I) > 0) {

        sprintf(filename, "2:%s,r", I);

        cbm_open(15, 8, 15, "");
        cbm_open(3, 8, 3, filename);

        e = errorcheck();

        if (e == 0) {

            print("\n\nStart your download!");

            for (t = 0; t < MAXRETRIES; t++) {

                /* wait for NAK from remote... */
                ch = inbyte('L', &status);

                if ((ch == NAK) || (status != OK)) {
                    break;
                }

            }

            if (ch == NAK) {

                /* lets go! */
                do {

                    /* clear buffer */
                    clearbuffer();

                    /* header */
                    outbyte(SOH);
                    outbyte(p);
                    outbyte(255 - p);

                    sprintf(I, "\n\n\236Block #%i:", p); lprint(I);

                    if (retry == FALSE) {

                        m = 0;
                        pad(packet);

                        /* get bytes from disk */
                        for (i = 0; i < 128; i++) {
                            cbm_k_chkin(3);
                            ch = cbm_k_basin();
                            cbm_k_clrch();
                            st = cbm_k_readst();

                            if (st == 0) {
                                outbyte(ch);
                                lprint("\237+");
                                packet[i] = ch;
                                m = m + ch;
                            } else if (st == 64) {
                                /* end of file */
                                for (j = i; j < 128; j++) {
                                    lprint("\237+");
                                    outbyte(PAD);
                                    m = m + PAD;
                                }
                                i = 128;
                                break;
                            } else {
                                e = errorcheck();
                                status = CANCEL;
                                lprint("\034C");
                                outbyte(CAN);
                                outbyte(CAN);
                                outbyte(CAN);
                                sending = FALSE;
                                break;
                            }
                        }

                        outbyte(m);         /* checksum! */

                    } else {

                        m = 0;

                        /* re-send the packet */
                        for (i = 0; i < 128; i++) {
                            outbyte(packet[i]);
                            m = m + packet[i];
                            lprint("\237+");
                        }

                        outbyte(m);         /* checksum! */

                        retry = FALSE;

                    }

                    if ((st == 0) || (st ==64)) {

                        ch = inbyte('S', &status);

                        if (ch == ACK) {

                            lprint("\036\272");
                            p++;

                        } else if (ch == NAK) {

                            lprint("\034X");
                            retries++;

                            enjoythesilence();

                            if (retries > MAXRETRIES) {
                                status = CANCEL;
                                lprint("\034C");
                                outbyte(CAN);
                                outbyte(CAN);
                                outbyte(CAN);
                                sending = FALSE;
                                break;
                            }

                        } else if (ch == CAN) {
                            lprint("\034C");
                            sending = FALSE;
                            break;
                        }
                    }

                    if (st == 64) {

                        /* we are done! */

                        outbyte(EOT);

                        ch = inbyte('S', &status);

                        if (ch == NAK) {
                            /* send EOT again? */
                            outbyte(EOT);
                            ch = inbyte('S', &status);
                        }

                        if (ch == ACK) {
                            lprint("\036E");
                            lprint("\n\n\005SUCCESS\n");
                            outbyte(ACK);
                        } else {
                            lprint("\034C");
                            lprint("\n\n\005FAILURE\n");
                            outbyte(CAN);
                        }

                        sending = FALSE;

                    }

                } while (sending == TRUE);

            } else {

                print("\n\nABORTED!");

            }

        }

        cbm_close(3);
        cbm_close(15);

    } else {

        print("\n\nABORTED!\n\n");

    }

    beep();
    putch(7);

    pause(FALSE);

}

char inbyte(char delay, char *status) {

    char ch = 0, s, lch, st;
    char listening = TRUE;

    *status = OK;

    if (delay == 'S') {
        /* short delay ~ 10 secs */
        delay = 3;
    } else if (delay == 'L') {
        /* long delay ~ 100 secs */
        delay = 24;
    } else if (delay == 'M') {
        /* micro delay < 3 secs */
        delay = 1;
    } else {
        delay = 3;
    }

    /* set jiffy clock low + med bytes to zero */
    POKE(JCM, 0);
    POKE(JCL, 0);

    do {

        s = PEEK(668);

        /* XLOCAL */
        lch = cbm_k_getin();
        if (lch == F7) {
            ch = 255;
            *status = CANCEL;
            listening = FALSE;
            break;
        }

        /* XREMOTE */
        cbm_k_chkin(5);
        ch = cbm_k_getin();
        cbm_k_clrch();

        if (PEEK(668) != s) {
            listening = FALSE;
        }

        if (PEEK(JCM) > delay) {
            *status = TIMEOUT;
            print("\034?");
            listening = FALSE;
        }

        st = carrierdetect();

        if (st == FALSE) {
            ch = 255;
            *status = CANCEL;
            listening = FALSE;
            break;
        }

    } while (listening == TRUE);

    return ch;

}

void outbyte(char ch) {
    cbm_k_ckout(5);
    cbm_k_bsout(ch);
    cbm_k_clrch();
}

void enjoythesilence(void) {

    char ch, r;
    char outer = TRUE, inner = TRUE;

    POKE(JCL, 0);

    do {

        do {

            r = PEEK(668);

            cbm_k_chkin(5);
            ch = cbm_k_getin();
            cbm_k_clrch();

            if (PEEK(668) == r) {
                inner = FALSE;
            } else {
                POKE(JCL, 0);
            }

        } while (inner == TRUE);

    } while (PEEK(JCL) < 60);

}

void pad(char *str) {
    while(*str) {
        *str++ = PAD;
    }
}
