#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "sm_common.h"

void print(char *str) {

    char ch;

    while (*str) {

        ch = *str++;

        cbm_k_bsout(ch);

        if ((ONLINE == TRUE) && (LOCALMODE == FALSE)) {
            cbm_k_ckout(5);
            cbm_k_bsout(ch);
            cbm_k_clrch();
        }

        if (ch == 7) {
            beep();
        }
    }
}

void cursor_on(void) {

    if (CURSORSTATUS == OFF) {
        POKE(212, 0);
        POKE(216, 0);

        if (PEEK(204) != 0) {
            asm("ldy #$00");
            asm("sty $cc");
            CURSORSTATUS = ON;
        }

    }

}

void cursor_off(void) {
        asm("ldy $cc");
        asm("bne %g", exitloop);
        asm("ldy #$01");
        asm("sty $cd");
        loop:
        asm("ldy $cf");
        asm("bne %g", loop);
        exitloop:
        asm("ldy #$ff");
        asm("sty $cc");
        CURSORSTATUS = OFF;
}

void beep(void) {

    static unsigned long j;

    /* VIC 20 BEEP */
    POKE(SV, 15);
    POKE(SN, 0);
    POKE(S3, 230);
    for (j=0; j<1000; ++j) asm("nop");
    POKE(SV, 0);

}

void sleep() {
    static unsigned long j;
    for (j=0; j<666; ++j) __asm__ ("nop");
}

char carrierdetect() {

    char st;
    char result = FALSE;

    /*
     * NOTE: in VICE, the value is 16, on a real VIC, the value is 8
     *
     */

    if (LOCALMODE == TRUE) {

        result = TRUE;

    } else {

        st = PEEK(RS)&16;

        if (st == 16) {
            result = TRUE;
        }

    }

    return(result);

}

char getch(void) {

    char ch = 0;
    char st;

    cursor_on();

    do {

        /* XLOCAL */
        ch = cbm_k_getin();

        if(ch == 136) {                /* F7 - HANG UP */
            LOCALMODE = FALSE;
            ch = 255;
        } else if ((ch == 0) && (LOCALMODE == FALSE)) {
            /* XREMOTE */
            cbm_k_chkin(5);
            ch = cbm_k_getin();
            cbm_k_clrch();
            if ((ch >= 133) && (ch <= 140) && (U.SECURITY < 5)) {
                ch = 0;
            }
        }

        /* remove shifted-space! */
        if (ch == 160) {
            ch = 32;
        }

        if (ch == 0) {

            st = carrierdetect();

            if (st == FALSE) {
                ch = 255;
            }

        }

    } while(ch == 0);

    cursor_off();

    return(ch);

}

void putch(char ch) {

    /* print local */
    cbm_k_bsout(ch);

    /* beep if required */
    if (ch == 7) {
        beep();
    }

    /* print remote */
    if ((ONLINE == TRUE) && (LOCALMODE == FALSE)) {
        cbm_k_ckout(5);
        cbm_k_bsout(ch);
        cbm_k_clrch();
    }

}

void bootstrap(char *module) {

    char sw = 0;
    char remark[22];

    /* select module number */
    if (strcmp(module, "BBS") == 0) {
        sw = 1;
        strcpy(remark, "Hang on...");
    }

    if (strcmp(module, "GAMES") == 0) {
        sw = 2;
        strcpy(remark, "Hold your shorts...");
    }

    if (strcmp(module, "EDITOR") == 0) {
        sw = 3;
        strcpy(remark, "Loading editor...\n");
    }

    if (strcmp(module, "FILES") == 0) {
        sw = 4;
        strcpy(remark, "Just a sec...");
    }

    if (strcmp(module, "RSA") == 0) {
        sw = 5;
        strcpy(remark, "One more hop...");
    }

    if (strcmp(module, "SYSOP") == 0) {
        sw = 6;
        strcpy(remark, "Uno momento...");
    }

    if (sw > 0) {
        print("\n");
        print(remark);
        cbm_k_setlfs (1,8,3);
        cbm_k_setnam ("bootstrap");
        cbm_k_load (0, BS_ADDRESS);
        POKE(BS_MODULE, sw);
        POKE(BS_ID, U.ID);
        POKE(BS_LOCALMODE, LOCALMODE);
        asm( BS_JUMP );
    }


}

int showfile(char *filename, char pauseatend) {

    char fullfile[32], st, a, e, ch;
    char qe = FALSE, fs = FALSE;
    int i = 0, ln = 0, reply = 0, sl = 0;
    int c;

    strcpy(fullfile, filename);
    strcat(fullfile, ",s,r");

    cbm_open(15,8,15,"");
    cbm_open(2,8,2,fullfile);

    e = errorcheck();

    if (e == 0) {

        st = 0;
        c = 0;
        clear(W);

        do {

            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();
            st = cbm_k_readst();

            if (st == 0) {
                putch(a);
                c++;
                if (a == 13) {
                    ln++;
                    c = 0;
                    if (ln >= 22) {
                        ch = gpause();
                        ln = 0;
                        if (ch == 'Q') {
                            st = 64;
                            qe = TRUE;
                            break;
                        }
                    }
                }
            } else if (st == 64) {
                /* end of file */
            } else {
                e = errorcheck();
            }

        } while (st == 0);

    }

    cbm_close(2);
    cbm_close(15);

    if ((pauseatend == TRUE) && (qe == FALSE)) {
        print("\n");
        gpause();
    }

    return reply;

}

char errorcheck() {

    /* assumes channel 15 is already open */

    char data[32];
    char err[3];
    int e = 0;

    cbm_read(15, data, 32);

    if ((data[0] != '0') || (data[1] != '0')) {
        showerr(data);
        err[0] = data[0];
        err[1] = data[1];
        err[2] = '\0';
        e = (char)atoi(err);
    }

    return e;

}

void showerr(char *err) {

    int i, x;

    x = strlen(err);

    print("\n\n\022");
    print("ERROR: ");
    putch(err[0]);
    putch(err[1]);
    print("\n\022");

    i = 3;

    do {
        putch(toupper(err[i]));
        i++;
    } while ((err[i] != ',') && (i < x));

    print("\222\n\n");

}

void clear(char *str) {
    while (*str) {
        *str++ = '\0';
    }
}

void trim(char *str) {
    int start = 0, end = strlen(str) - 1;

    // Remove leading whitespace
    while (isspace(str[start])) {
        start++;
    }

    // Remove trailing whitespace
    while (end > start && isspace(str[end])) {
        end--;
    }

    // If the string was trimmed, adjust the null terminator
    if (start > 0 || end < (strlen(str) - 1)) {
        memmove(str, str + start, end - start + 1);
        str[end - start + 1] = '\0';
    }
}

void getdate() {

    /* populates global variable DATE with the date in format yyyy-mm-dd */

    int v;

    /* MONTH */
    POKE(RT,8);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, O, 10);
    if (v < 10) {
        strcpy(DATE, "0");
        strcat(DATE, O);
    } else {
        strcpy(DATE, O);
    }
    strcat(DATE,"/");

    /* DAY */
    POKE(RT,7);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, O, 10);
    if (v < 10) {
        strcat(DATE, "0");
    }
    strcat(DATE,O);

    /* YEAR */
    POKE(RT,9);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, O, 10);
    strcat(DATE, "/20");
    strcat(DATE,O);

}

void gettime() {

    /* populates global variable TIME with seconds since midnight */

    int v;
    long tm;

    POKE(RT,4);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    tm = (long)v * 3600;

    POKE(RT,2);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    tm = tm + (long)v * 60;

    POKE(RT,0);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    tm = tm + (long)v;

    if (U.TU > tm) {
        AD = 0;
    }

    TIME = tm;

}

int RTCconvert(int i) {

    long z;
    int vd;
    int ret;

    /* int(v/16)*10+(v-16*int(v/16)) */

    z = (long)i;
    z = z * 10000;
    z = z / 16;
    z = z / 10000;
    vd = (int)z;

    ret = vd * 10 + (i - 16 * vd);

    return ret;

}

char confirm() {

    char ch, done = FALSE, answer = FALSE;

    print("\n\nAre you sure? ");

    do {
        ch = getch();

        if ((ch == 'Y') || (ch == 'y') || (ch = 255)) {
            print("Y\n");
            answer = TRUE;
            done = TRUE;
        } else if ((ch == 'N') || (ch == 'n')) {
            print("N\n");
            done = TRUE;
        }

    } while (done == FALSE);

    return answer;

}

void askforhelp() {
    print("\nPress ? for Help!");
}

char get_command() {

    char ch;

    ch = getch();
    ch = toupper(ch);
    putch(ch);
    print("\n");

    return ch;

}

void input(int min, int max) {

    int i;
    char done = FALSE, ch;

    /* reduce max by 1 since we count starting at zero instead of one */
    max--;
    if (max < 0) {
        max = 0;
    }

    clear(I);
    i = 0;

    do {

        ch = getch();

        if (ch == 255) {
            done = TRUE;
            break;
        } else if (ch == 13) {
            if (i >= min) {
                putch(ch);
                done = TRUE;
                break;
            }
        } else if (ch == 20) {
            if (i > 0) {
                ch = I[i-1];
                I[i] = '\0';
                i--;
                putch(20);
            }
        } else if ((i <= max) && (i < 40)) {

            if ((ch != 17) && (ch != 19) && (ch != 29) && (ch != 145) && (ch != 147) && (ch != 148) && (ch != 157)) {
                I[i] = ch;
                i++;
                putch(ch);
            }

        }

    } while (done == FALSE);

}

void clearbuffer(void) {

    char ch;

    do {
        cbm_k_chkin(5);
        ch = cbm_k_getin();
        cbm_k_clrch();
    } while (PEEK(667) != PEEK(668));


}

char gpause() {

    char ch, i;

    print("   - hit any key - ");

    ch = getch();

    if (ch == 255) {
        ch = 'q';
    }

    for (i = 0; i < 19; i++) {
        putch(20);
    }

    return ch;

}
