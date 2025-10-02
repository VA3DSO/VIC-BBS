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

    if (strcmp(module, "FUTURE") == 0) {
        sw = 5;
        strcpy(remark, "Somthing witty...");
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

int showfile(char *filename, char pauseatend, int msgnum) {

    char fullfile[32], st, a, e, ch, t[2];
    char qe = FALSE;
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
        t[0] = '\0';
        t[1] = '\0';

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
                if ((ln == 4) && (a != 13) && (c > 5)) {
                    t[0] = a;
                    strcat(W, t);
                    sl++;                   /* subject length */
                }
                if (a == 13) {
                    ln++;
                    c = 0;
                    if (ln >= 22) {
                        ch = pause();
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

        W[sl] = '\0';                       /* zero terminate subject line */
    }

    cbm_close(2);
    cbm_close(15);

    if ((pauseatend == TRUE) && (qe == FALSE)) {
        print("\n");
        pause();
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

char pause() {

    int i;

    char ch;
    print("  - press any key -");
    ch = getch();
    for (i = 0; i < 19; i++) {
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

char nextid() {

    char nid = 0, data[2], e;
    int i;

    if (open_userfile() == 0) {
        for (i=2; i<255; i++) {
            if(gotouser((char)i) == 0) {
                cbm_read(1, data, 1);
                e = errorcheck();
                if (e == 0) {
                    if (data[0] == 255) {
                        /* we found the next ID! */
                        nid = (char)i;
                        i = 255;
                        break;
                    }
                }
            }
        }
    }

    close_userfile();

    return nid;

}

void edituser(char id, char sysop) {

    char sid, yn = FALSE;

    print("\223\005\022EDIT\222USER\n\n");

    if (sysop == TRUE) {
        sid = U.ID;
        U.ID = id;
        loaduser(U.ID);
    }

    print("\nHandle?\n(min 3,max 12)\n>");
    strcpy(I, U.USERNAME);
    print(I);
    input('A', 3, 12, TRUE);
    strcpy(U.USERNAME, I);
    trim(U.USERNAME);

    print("\nPassword?\n(min 3, max 10)\n>");
    strcpy(I, U.PASSWORD);
    print(I);
    input('A', 3, 10, TRUE);
    strcpy(U.PASSWORD, I);
    trim(U.PASSWORD);

    print("\nReal name?\n(min 0, max 20)\n>");
    strcpy(I, U.REALNAME);
    print(I);
    input('A', 0, 20, TRUE);
    strcpy(U.REALNAME, I);
    trim(U.REALNAME);

    print("\nFrom?\n(min 0, max 15)\n>");
    strcpy(I, U.FROM);
    print(I);
    input('A', 0, 15, TRUE);
    strcpy(U.FROM, I);
    trim(U.FROM);

    if (sysop == TRUE) {

        print("\nSecurity?\n(min 1, max 6)\n>");
        itoa(U.SECURITY, O, 10);
        strcpy(I, O);
        print(O);
        input('A', 1, 1, TRUE);
        U.SECURITY = atoi(I);

        print("\n# of Calls?\n(min 0, max 9999)\n>");
        itoa(U.CALLS, O, 10);
        strcpy(I, O);
        print(O);
        input('A', 1, 4, TRUE);
        U.CALLS = atoi(I);

        print("\nLast Read?\n(min 0, max 9999)\n>");
        itoa(U.LASTREAD, O, 10);
        strcpy(I, O);
        print(O);
        input('A', 1, 4, TRUE);
        U.LASTREAD = atoi(I);

    }

    yn = confirm();

    if (yn == TRUE) {

        print("\nSaving... ");

        saveuser(U.ID);
        genuserlist();

        if (sysop == TRUE) {
            loaduser(sid);
        }

    }

}

void changepassword() {

    char yn;

    print("\223\022CHANGE\222PASSWORD\n\n");
    print("New Password?\n(min 3, max 10)\n>");
    input('A', 0, 10, FALSE);

    if (strlen(I) >= 3) {
        yn = confirm();

        if (yn == TRUE) {
            print("\nSaving... ");
            strcpy(U.PASSWORD, I);
            trim(U.PASSWORD);
            saveuser(U.ID);
            print("OK!\n\n");
            print("Write it down!\n");
            pause();
        }
    } else if (strlen(I) > 0) {
        print("\n\n\022MIN 3 CHAR REQ!\n");
    }




}

void genuserlist() {

    int i, users = 0;

    open_userfile();
    cbm_open(3, 8, 3, "@0:userlog,s,w");

    for(i=1; i<255; i++) {
        gotouser((char)i);
        cbm_read(1, I, 12);
        if (I[0] != 255) {
            users++;
            clear(O);
            itoa(i, O, 10);
            strcat(O, " ");
            strcat(O, I);
            strcat(O, "\n");
            cbm_write(3, O, strlen(O));
        }
    }

    cbm_close(3);
    close_userfile();
    S.NUMUSERS = users;
    savestats();

}

char finduser(char *username) {

    char id = 0, temp[13];
    int i;

    trim(username);

    open_userfile();

    for (i=1; i<255; i++) {
        gotouser((char)i);
        cbm_read(1, temp, 12);
        trim(temp);
        if (strcmp(strupper(username), strupper(temp)) == 0) {
            id = (char)i;
            i = 255;
            break;
        }
    }

    close_userfile();

    return id;
}

char confirm() {

    char ch, done = FALSE, answer = FALSE;

    print("\n\nAre you sure? ");

    do {
        ch = getch();

        if ((ch == 'Y') || (ch == 'y')) {
            print("Y\n");
            answer = TRUE;
            done = TRUE;
        } else if ((ch == 'N' || ch == 'n')) {
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

void chatmode(char page) {

    int t,k;
    char ch, online;
    char chat = FALSE;
    char sb = 10;
    char track[3] ="XX";

    if (page == TRUE) {

        print("\005\nPaging the Sysop:\n");
        print("\n\nPress \022 F3 \222 to chat!\n\n");

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
        print("\nUSER:");
        print(U.USERNAME);
        print("\n\n");
        print("\022 F3 \222 to exit...\n\n");

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
     *   'P' password
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
        if ((ch == 32) && (fmt == 'W') && (i > 18)) {
            ch = 13;
        }

        if (ch == 255) {
            done = TRUE;
            break;
        } else if (ch == 13) {
            if (hc >= min) {
                putch(ch);
                done = TRUE;
                break;
            } else {
                sprintf(O, "\n\n\022MIN %i CHARS REQ\nTry again...\n>", min); print(O);
                clear(I);
                i = 0;
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
                            strcpy(W, "");
                            for (k = j + 1; k < i; k++) {
                                W[c] = I[k];
                                c++;
                                putch(20);
                                I[k] = 0;
                            }
                            putch(13);
                            W[c] = 0;
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

void newuser() {

    char yn, check, id, sid=0;

    if (U.SECURITY >= 5) {
        sid = U.ID;
    }

    id = nextid();

    if (id != 0) {

        if (sid != 0) {
            print("\223\005\022ADD\222USER\n\n");
        } else {
            showfile("newuser1", FALSE, 0);
        }

        yn = FALSE;

        do {

            memset(&U, 0, sizeof(U));

            print("\nHandle?\n(min 3,max 12)\n>");
            input('A', 3, 12, FALSE);
            strcpy(U.USERNAME, I);
            trim(U.USERNAME);

            print("\nChecking... ");
            check = finduser(U.USERNAME);

            if (check == 0) {

                print("\nPassword?\n(min 3, max 10)\n>");
                input('A', 3, 10, FALSE);
                strcpy(U.PASSWORD, I);
                trim(U.PASSWORD);

                print("\nReal name?\n(min 0, max 20)\n>");
                input('A', 0, 20, FALSE);
                strcpy(U.REALNAME, I);
                trim(U.REALNAME);

                print("\nFrom?\n(min 0, max 15)\n>");
                input('A', 0, 15, FALSE);
                strcpy(U.FROM, I);
                trim(U.FROM);

                U.ID = id;
                U.SECURITY = 3;
                U.CALLS = 1;
                U.LASTREAD = 0;

                getdate();
                strcpy(U.LASTLOGON, DATE);

                settimelimit();

                print("\223You entered...\n\n");

                print("Handle:\n");
                print(U.USERNAME);
                print("\n\n");

                print("Password:\n");
                print(U.PASSWORD);
                print("\n\n");

                print("Real name:\n");
                print(U.REALNAME);
                print("\n\n");

                print("From:\n");
                print(U.FROM);
                print("\n\n");

                yn = confirm();

            } else {

                print("\n\n\022Handle already taken!\n");
            }


        } while (yn == FALSE);

        print("\nSaving... ");

        saveuser(U.ID);
        S.NUMUSERS++;
        savestats();
        genuserlist();

        print("OK!\n");

        if (sid != 0) {
            loaduser(sid);
        } else {
            print("\nIMPORTANT!\n\nWrite down your User\n");
            print("ID and Password:\n\nUser ID = ");

            itoa(U.ID, O, 10);
            print(O);
            print("\nPassword= ");
            print(U.PASSWORD);
            print("\n");

            pause();

            showfile("newuser2", TRUE, 0);
        }

    } else {
        print("\n\nUSER LOG FULL!\n");
    }


}

void debug() {

    print("\223\022DEBUG\222INFO\n");

    sprintf(O, " USER ID:%i\n", U.ID); print(O);
    sprintf(O, "USERNAME:%s\n", U.USERNAME); print(O);
    sprintf(O, "PASSWORD:%s\n", U.PASSWORD); print(O);
    sprintf(O, "REALNAME:%s\n", U.REALNAME); print(O);
    sprintf(O, "    FROM:%s\n", U.FROM); print(O);
    sprintf(O, "SECURITY:%i\n", U.SECURITY); print(O);
    sprintf(O, " # CALLS:%i\n", U.CALLS); print(O);
    sprintf(O, "LST READ:%i\n", U.LASTREAD); print(O);
    sprintf(O, "LST CALL:%s\n", U.LASTLOGON); print(O);
    getdate();
    sprintf(O, "Date: %s\n", DATE); print(O);
    sprintf(O, "LST USER:%s\n", S.LASTCALLER); print(O);
    sprintf(O, " # CALLS:%i\n", S.NUMCALLS); print(O);
    sprintf(O, " # USERS:%i\n", S.NUMUSERS); print(O);
    sprintf(O, " # MESGS:%i\n", S.NUMMESGS); print(O);
    sprintf(O, " # BULLS:%i\n", S.NUMBULLS); print(O);
    sprintf(O, "AD: %i\n", AD); print(O);
    sprintf(O, "LOCALMODE: %i\n", LOCALMODE); print(O);
    sprintf(O, "CURSORSTATUS: %i\n", CURSORSTATUS); print(O);
    sprintf(O, "LOGGINGON: %i\n", LOGGINGON); print(O);
    sprintf(O, "ONLINE: %i\n", ONLINE); print(O);

    pause();

}
