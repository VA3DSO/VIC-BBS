#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"

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

void lprint(char *str) {
    while (*str) {
        cbm_k_bsout(*str++);
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

    if (LOCALMODE == TRUE) {

        result = TRUE;

    } else {

        st = PEEK(RS)&16;

        if (st == 16) {
            result = TRUE;
        }


        if (LOGGINGON != TRUE) {

            /* check if time is up... */

            gettime();

            if ((TIME > U.TU) && (AD == 0)) {
                print("\n\nTIMES UP!\n");
                result = FALSE;
            }
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

        } else if (ch == 134) {        /* F3 - CHAT MODE */
            chatmode(FALSE);

        } else if ((ch == 0) && (LOCALMODE == FALSE)) {

            /* XREMOTE */
            cbm_k_chkin(5);
            ch = cbm_k_getin();
            cbm_k_clrch();

            /* FILTER OUT FUNCTION KEYS FROM REMOTE UNLESS SYSOP */
            if ((ch >= 133) && (ch <= 140) && (U.SECURITY < 5)) {
                ch = 0;
            }

            /* FIX UPPER CASE CHARS IF REMOTE IS ASCII TERMINAL */
            if ((ch >= 97) && (ch <= 122)) {
                ch = ch + 96;
            }

        }

        /* REMOVE SHIFTED-SPACE */
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

void loadstats() {

    int i, j, c;
    char e, ch, buffer[40], t[15];
    char nostats = FALSE;

    cbm_open(15, 8, 15, "");
    cbm_open(3, 8, 3, "stats,s,r");

    e = errorcheck();

    if (e == 0) {

        cbm_read(3, buffer, 40);

        e = errorcheck();

        if (e == 0) {

            clear(t);
            c = 0;
            j = 0;

            for (i=0; i<strlen(buffer); i++) {

                ch = buffer[i];

                if (ch == 13) {

                    t[j] = '\0';
                    trim(t);

                    switch(c) {
                        case 0:
                            strcpy(S.LASTCALLER, t);
                            break;
                        case 1:
                            S.NUMCALLS = atoi(t);
                            break;
                        case 2:
                            S.NUMUSERS = atoi(t);
                            break;
                        case 3:
                            S.NUMMESGS = atoi(t);
                            break;
                        case 4:
                            S.NUMBULLS = atoi(t);
                            break;
                        default:
                            print("\022ERROR! Unexpected field!\n");
                    }

                    clear(t);
                    c++;
                    j = 0;

                } else {

                    t[j] = ch;
                    j++;

                }
            }
        }
    } else if (e == 62) {
        nostats = TRUE;
        cbm_write(15, "I0:", 3);
    }

    cbm_close(3);
    cbm_close(15);

    if (nostats == TRUE) {
        strcpy(S.LASTCALLER, "SYSOP");
        S.NUMCALLS = 1;
        S.NUMUSERS = 1;
        S.NUMMESGS = 0;
        S.NUMBULLS = 0;
        savestats();
    }
}

void savestats() {

    char e, buffer[40], t[15];

    clear(buffer);
    clear(t);

    if (strlen(S.LASTCALLER) < 1) {
        strcpy(S.LASTCALLER, "NOBODY");
    }
    strcpy(buffer, S.LASTCALLER);
    strcat(buffer, "\n");

    itoa(S.NUMCALLS, t, 10);
    strcat(buffer, t);
    strcat(buffer, "\n");

    itoa(S.NUMUSERS, t, 10);
    strcat(buffer, t);
    strcat(buffer, "\n");

    itoa(S.NUMMESGS, t, 10);
    strcat(buffer, t);
    strcat(buffer, "\n");

    itoa(S.NUMBULLS, t, 10);
    strcat(buffer, t);
    strcat(buffer, "\n");

    cbm_open(15, 8, 15, "");
    cbm_open(3, 8, 3, "@0:stats,s,w");

    e = errorcheck();

    if (e == 0) {

        cbm_write(3, buffer, 40);
        e = errorcheck();

    }

    cbm_close(3);
    cbm_close(15);

}

void saveuser(char id) {

    char rec[76], e;
    int i;

    for (i=0; i<76; i++) {
        rec[i] = 32;
    }

    if (open_userfile() == 0) {
        if (gotouser(id) == 0) {
            if (strlen(U.USERNAME) > 0) {

                /* update user record */

                for (i=0; i<strlen(U.USERNAME); i++) {
                    rec[i] = U.USERNAME[i];
                }

                for (i=0; i<strlen(U.PASSWORD); i++) {
                    rec[i+12] = U.PASSWORD[i];
                }

                for (i=0; i<strlen(U.REALNAME); i++) {
                    rec[i+22] = U.REALNAME[i];
                }

                for (i=0; i<strlen(U.FROM); i++) {
                    rec[i+42] = U.FROM[i];
                }

                itoa((int)U.SECURITY, O, 10);
                rec[57] = O[0];

                itoa((int)U.CALLS, O, 10);
                for (i=0; i<strlen(O); i++) {
                    rec[i+58] = O[i];
                }

                itoa((int)U.LASTREAD, O, 10);
                for (i=0; i<strlen(O); i++) {
                    rec[i+63] = O[i];
                }

                getdate();

                rec[68] = DATE[0];
                rec[69] = DATE[1];
                rec[70] = DATE[3];
                rec[71] = DATE[4];
                rec[72] = DATE[8];
                rec[73] = DATE[9];
                rec[74] = 13;
                rec[75] = 0;

                cbm_write(1, rec, 76);
                e = errorcheck();
                e = gotouser(id);

            }

        }

    }

    close_userfile();


}

int showfile(char *filename, char drive, char pauseatend, int msgnum) {

    char fullfile[32], st, a, e, ch;
    char qe = FALSE, fs = FALSE;
    int i = 0, ln = 0, reply = 0, sl = 0;
    int c;

    strcpy(fullfile, filename);
    strcat(fullfile, ",s,r");

    cbm_open(15, drive, 15, "");
    cbm_open(2, drive, 2, fullfile);

    e = errorcheck();

    if (e == 0) {

        st = 0;
        c = 0;
        clear(W);

        if (msgnum > 0) {

            /* read first 5 bytes looking for reply email number */
            cbm_read(2, I, 5);
            reply = atoi(I);

            /* read one more char - line break after reply number */
            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();

        }

        /* process rest of file / message */
        do {

            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();
            st = cbm_k_readst();

            if (st == 0) {

                putch(a);
                c++;

                /* save subject in W (word wrap) variable */
                if ((ln == 4) && (a != 13) && (c > 5) && (fs == FALSE)) {
                    W[sl] = a;
                    sl++;                   /* subject length */
                }

                if (ln == 5) {
                    fs = TRUE;
                }

                /* check for a space from the remote side to end viewing early */
                cbm_k_chkin(5);
                ch = cbm_k_getin();
                cbm_k_clrch();
                if (ch == ' ') {
                    /* client requested early quit */
                    st = 64;
                    qe = TRUE;
                    break;
                }

                if (a == 13) {
                    ln++;
                    c = 0;
                    if (ln >= 22) {
                        ch = pause(TRUE);
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
                sprintf(O, "\n\n\022ERR:%d\222\n\n", st); print(O);
                e = errorcheck();
            }

        } while (st == 0);

        W[sl] = '\0';                       /* zero terminate subject line */
    }

    cbm_close(2);
    cbm_close(15);

    if ((pauseatend == TRUE) && (qe == FALSE)) {
        print("\n");
        pause(FALSE);
    }

    return reply;

}

char open_userfile() {

    /* this function will return 62 if the user file does not exist */

    char e, err[3];
    char filename[9] = "users,l,x";
    filename[8] = 76;

    cbm_open(15,8,15,"");

    /* check if user file exists */
    strcpy(O, "r0:users=users");
    cbm_write(15, O, strlen(O));

    clear(err);
    cbm_read(15, err, 2);
    e = (char)atoi(err);

    if ((e == 0) || (e == 63)) {
        cbm_open(1,8,2,filename);
        e = errorcheck();
    }

    return e;

}

char gotouser(char id) {

    char p[5], e;

    /* set REL pointer */
    p[0] = 'p';
    p[1] = 98;
    p[2] = id;
    p[3] = 0;
    p[4] = 1;

    cbm_write(15, p, 5);

    e = errorcheck();

    return e;

}

char loaduser(char id) {

    char rec[76], e = 0;

    clear(rec);
    memset(&U, 0, sizeof(U));

    if (open_userfile() == 0) {
        if (gotouser(id) == 0) {
            cbm_read(1, rec, 76);
            e = errorcheck();
        }

    }

    close_userfile();

    if (strlen(rec) == 75) {

        U.ID = id;

        strncpy(U.USERNAME, rec, 12);
        trim(U.USERNAME);

        strncpy(U.PASSWORD, rec + 12, 10);
        trim(U.PASSWORD);

        strncpy(U.REALNAME, rec + 22, 20);
        trim(U.REALNAME);

        strncpy(U.FROM, rec + 42, 15);
        trim(U.FROM);

        strncpy(O, rec + 57, 1);
        O[1] = 0;
        U.SECURITY = (char)atoi(O);

        strncpy(O, rec + 58, 5);
        trim(O);
        U.CALLS = atoi(O);

        strncpy(O, rec + 63, 5);
        trim(O);
        U.LASTREAD = atoi(O);

        if (U.LASTREAD < S.NUMMESGS - 255) {
            U.LASTREAD = S.NUMMESGS;
        }

        if (U.LASTREAD > S.NUMMESGS) {
            U.LASTREAD = S.NUMMESGS;
        }

        clear(O);

        /* month */
        strncpy(O, rec + 68, 2);
        O[2] = '\0';
        strcpy(U.LASTLOGON, O);
        strcat(U.LASTLOGON, "/");

        /* day */
        strncpy(O, rec + 70, 2);
        O[2] = '\0';
        strcat(U.LASTLOGON, O);
        strcat(U.LASTLOGON, "/");

        /* year */
        strncpy(O, rec + 72, 2);
        O[2] = '\0';
        strcat(U.LASTLOGON, "20");
        strcat(U.LASTLOGON, O);

        settimelimit();

    } else if (LOCALMODE == TRUE) {

        U.ID = 1;

        strcpy(U.USERNAME, "SYSOP");
        strcpy(U.PASSWORD, "SYSOP");
        strcpy(U.REALNAME, "SYSOP");
        strcpy(U.FROM, "SYSOP");
        U.SECURITY = 6;
        U.CALLS = 0;
        U.LASTREAD = 0;
        strcpy(U.LASTLOGON, "1980/01/01");

    } else {
        e = 1;
    }

    return e;

}

void close_userfile() {

    cbm_close(1);
    cbm_close(15);

}

void settimelimit() {

    long tl;

    AD = 0;
    gettime();

    switch (U.SECURITY) {
        case 6:             /* SYSOP */
            tl = 120;
            break;
        case 5:             /* ASST SYSOP */
            tl = 90;
            break;
        case 4:             /* POWER USER */
        case 3:             /* REGULAR USER */
            tl = 60;
            break;
        case 2:             /* MODERATED USER */
            tl = 30;
            break;
        case 1:             /* NEW USER */
            tl = 20;
            break;
        default:
            tl = 10;
    }

    U.TU = TIME + tl * 60;
    if (U.TU > 86400) {
        U.TU = U.TU - 86400;
        AD = 1;
    }

}

char pause(char quitearly) {

    int i;

    char ch;
    if (quitearly == TRUE) {
        print("hit any key (q=quit)");
    } else {
        print(" - press any key -  ");
    }
    ch = getch();
    for (i = 0; i < 20; i++) {
        putch(20);
    }

    ch = toupper(ch);

    return ch;

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

char hardchar(char ch) {

    /* examines supplied char ch
     * returns 1 if char is a printing char (hard char)
     * returns 0 if char is a non-printing char (color, etc)
     */

    char hc = 0;

    if (((ch >= 32) && (ch <= 127)) || ((ch >= 161) && (ch <= 254)))  {
        hc = 1;
    }

    /* special case for editing user records */
    if (ch == 255) {
        hc = 1;
    }

    return hc;

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

    char ch, done = FALSE, answer = FALSE, st;

    print("\n\nAre you sure? ");

    do {
        ch = getch();

        if ((ch == 'Y') || (ch == 'y')) {
            print("Y\n");
            answer = TRUE;
            done = TRUE;
        } else if ((ch == 'N') || (ch == 'n')) {
            print("N\n");
            done = TRUE;
        }

        st = carrierdetect();

        if (st == FALSE) {
            ch = 'Y';
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

void chatmode(char page) {

    int t,k;
    char ch, online;
    char chat = FALSE;
    char sb = 10;
    char track[3] ="XX";

    if (page == TRUE) {

        print("\005\nPaging the Sysop:\n");
        sprintf(O, "\022%s\222\n", U.USERNAME); lprint(O);
        lprint("\n\nPress \022 F3 \222 to chat!\n\n");

        for (t=0; t<20; t++) {
            beep();
            POKE(SC,sb);
            if (sb == 8) {
                sb = 10;
            } else {
                sb = 8;
            }
            print(".");
            for (k=0; k<10; k++) {
                sleep();
                ch = cbm_k_getin();
                if (ch == 134) {                /* F3 */
                    chat = TRUE;
                    t = 30;
                } else if (ch != 0) {
                    chat = FALSE;
                    t = 30;
                }
            }
        }

        POKE(SC,8);

    } else {
        chat = TRUE;
    }

    if (chat == TRUE) {
        print("\223\005CHAT MODE!\n\n");
        print("\007");
        print("type /x to exit...\n\n");
        lprint("\nUSER: \022");
        lprint(U.USERNAME);
        lprint("\n\n");
        lprint("\022 F3 \222 to exit...\n\n");

        do {

            cursor_on();

            do {

                /* XLOCAL */
                ch = cbm_k_getin();

                if (ch == 134) {

                    chat = FALSE;

                } else if((ch == 0) && (LOCALMODE == FALSE)) {

                    /* XREMOTE */

                    cbm_k_chkin(5);
                    ch = cbm_k_getin();
                    cbm_k_clrch();

                    if ((ch >= 133 && ch <= 140)) {
                        ch = 0;
                    }

                    online = carrierdetect();

                    if (online == FALSE) {
                        chat = FALSE;
                    }
                }

            } while(ch == 0);

            cursor_off();

            putch(ch);

            if (ch == 13) {
                if (strcmp(track, "/X") == 0)  {
                    chat = FALSE;
                }
            } else {
                ch = toupper(ch);
                track[0] = track[1];
                track[1] = ch;
                track[2] = 0;
            }

        } while ((chat == TRUE) && (ONLINE == TRUE));

        print("\nExiting chat mode...\n");


    } else {

        print("\n\nSorry the Sysop is\nnot here.\n");

    }

}

void input(char fmt, int min, int max, char editmode) {

    /* uses global I[] array to return result */

    /*
     * fmt =
     *   'A' alphanumeric,
     *   'W' word wrap alphanumeric,
     *   'P' password'
     *   'G' game mode - not char[13] at end of input
     *
     * min = 0 to 40
     *
     * max = 0 to 40
     *
     * editmode = TRUE or FALSE
     *   TRUE = editing existing data in I[]
     *   FALSE = starting from nothing
     *
     * hc = hard chars - ie: printing chars
     *
     */

    int i, j, k, c;
    char done = FALSE, ch, hc = 0;
    char tries = 0;

    /* reduce max by 1 since we count starting at zero instead of one */
    max--;
    if (max < 0) {
        max = 0;
    }

    if (editmode == FALSE) {
        clear(I);
        i = 0;
    } else {
        for (i = 0; i < strlen(I); i++) {
            ch = I[i];
            hc = hc + hardchar(ch);
        }
        i = strlen(I);
    }

    if (fmt == 'W') {
        clear(W);
    }

    do {

        ch = getch();

        /* end line early if word wrap is on */
        if ((ch == 32) && (fmt == 'W') && (i > 19)) {
            ch = 13;
        }

        if (ch == 255) {
            done = TRUE;
            break;
        } else if (ch == 13) {
            if (hc >= min) {
                if (fmt != 'G') {
                    putch(ch);
                }
                done = TRUE;
                break;
            } else {
                if (fmt != 'G') {
                    sprintf(O, "\n\n\022MIN %i CHARS REQ\nTry again...\n>", min); print(O);
                    clear(I);
                    i = 0;
                }
            }
        } else if (ch == 20) {
            if (i > 0) {
                ch = I[i-1];
                I[i] = '\0';
                i--;
                hc = hc - hardchar(ch);
                if (hardchar(ch) == 1) {
                    putch(20);
                }
            }
        } else if ((hc <= max) && (i < 40)) {

            /* filter char here based on 'fmt' */

            if ((ch != 17) && (ch != 19) && (ch != 29) && (ch != 145) && (ch != 147) && (ch != 148) && (ch != 157)) {
                if (fmt == 'P') {
                    putch('*');
                } else {
                    /* check for F8 - if so, swap with CLR HOME to allow clear screen code in files */
                    if (ch == 140) {
                        ch = 147;
                    }
                    putch(ch);
                }
                I[i] = ch;
                i++;
                hc = hc + hardchar(ch);
                if ((hc == max + 1) && (fmt == 'W')) {
                    /* wrap this line! */
                    for (j = i; j > 0; j--) {
                        if (I[j] == 32) {
                            /* found last space! */
                            c = 0;
                            clear(W);
                            for (k = j + 1; k < i; k++) {
                                W[c] = I[k];
                                c++;
                                putch(20);
                                I[k] = '\0';
                            }
                            W[c] = '\0';
                            putch(13);
                            j = 0;
                            done = TRUE;
                            break;
                        }
                    }
                    if (done == FALSE) {
                        /* we didn't find a space to wrap with! */
                        done = TRUE;
                        putch(13);
                        break;
                    }
                }

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

    char ch;

    print("   - hit any key - ");

    ch = getch();

    if (ch == 255) {
        ch = 'q';
    }

    return ch;

}
