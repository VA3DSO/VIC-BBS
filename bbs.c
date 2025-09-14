/*****************************************************************************/
/*                                                                           */
/*                                   bbs.c                                   */
/*                                                                           */
/*                  Commodore VIC-20 Bulletin Board System                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1982-2025, Rick Towns                                                 */
/*                Sudbury, Ontario                                           */
/*                CANADA                                                     */
/* EMail:         sysop@deepskies.com                                        */
/*                                                                           */
/* Acknowledgments:                                                          */
/*   Special thanks to Francesco Sblendorio (github.com/sblendorio) for his  */
/*   excellent victerm300 which inspired the writing of this program. Some   */
/*   of the functions here are based on some functions in victerm300.        */
/*                                                                           */
/*   Specifically: print, cursor_on, cursor_off and beep                     */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cbm.h>
#include <peekpoke.h>
#include <vic20.h>
/* FOR DEBUGGING ONLY! */
#include <conio.h>

#pragma charmap(147, 147)
#pragma charmap(17, 17)

#define TRUE 1
#define FALSE 0
#define S1 0x900A           /* 36874 - oscillator 1 (low) */
#define S2 0x900B           /* 36875 - oscillator 2 (med) */
#define S3 0x900C           /* 36876 - oscillator 3 (high) */
#define SN 0x900D           /* 36877 - noise generator */
#define SV 0x900E           /* 36878 - speaker volume (1-15) */
#define SC 0x900F           /* 36879 - screen border */
#define RS 0x9110           /* 37136 - RS-232 register */
#define RT 0x9800           /* 38912 - real time clock chip */
#define QR 0xA000           /* 40960 - free upper memory */

/* program functions */
void open_comms();
char carrierdetect();
void answer_call();
void logon();
void welcome();
void main_menu();
void bulletins();
void showtime();
void chatmode();
void sysop_menu();
void hangup();
void close_comms();

/* utility functions */
void loadstats();
void savestats();
void showfile(char*);
char open_userfile();
char gotouser(char);
char loaduser(char);
char pauseorquit(char);
void settimelimit();
void newuser();
char nextid();
void edituser(char, char);
void changepassword();
void saveuser(char);
void genuserlist();
char finduser(char*);
void close_userfile();
void file_editor(int);
void file_edit(char*, char, int);
unsigned int file_load(char*, char);
void file_save(char*, char, unsigned int);
void file_viewer(unsigned int, char);
void directory(char);
void pause();
char confirm();
void askforhelp();
char get_command();

/* system functions */
void print(char*);
void lprint(char*);                   /* for printing locally only */
void putch(char);
void input(char, int, int, char);
char getch();
char errorcheck();
void showerr(char*);
void clear(char*);
void cursor_on();
void cursor_off();
void beep();
void sleep();
void trim(char*);
void gettime();
void getdate();
int RTCconvert(int);
char hardchar(char);

/* NOTE: all fields are n+1 in size to accomodate null termination */
typedef struct {
    char ID;
    char USERNAME[13];
    char PASSWORD[11];
    char REALNAME[21];
    char FROM[16];
    char SECURITY;
    int CALLS;
    int LASTREAD;
    char LASTLOGON[11];
    long TU;
}  USER;

typedef struct {
    char LASTCALLER[13];
    int NUMCALLS;
    int NUMUSERS;
    int NUMMESGS;
    int NUMBULLS;
} SYSTEM;

/* global variables */
static USER U;                         /* current logged in user */
static SYSTEM S;                       /* system stats */
static char DATE[11];                  /* date in yyyy-mm-dd format */
static long TIME;                      /* seconds since midnight */
static char AD;                        /* add day to time up calc */
static char LOCALMODE = TRUE;          /* local mode toggle */
static char CURSORSTATUS = FALSE;      /* cursor status toggle */
static char SYSOP = FALSE;             /* Sysop in/out toggle */
static char LOGGINGON = TRUE;          /* toggle for logging on - for times up calc */
static char I[40];                     /* system input variable */

int main() {

    char running = TRUE, waiting, online, out[13], ch;

    /* firing up... */
    POKE(SV, 0);                       /* turn off speaker volume */
    POKE(SC, 8);                       /* black on black screen */

    lprint("\223\005\016\010\n\n\n\n\n\n\n\n\n\nfiring up...");
    cursor_on();

    loadstats();

    do {

        waiting = TRUE;
        online = FALSE;
        LOGGINGON = TRUE;

        open_comms();

        lprint("\223\005\016\010\022VIC\222BBS           V1.01\n");
        lprint("Last : ");
        lprint(S.LASTCALLER);
        lprint("\n");

        lprint("Calls: ");
        itoa(S.NUMCALLS, out, 10);
        lprint(out);
        lprint("\n");

        lprint("Users: ");
        itoa(S.NUMUSERS, out, 10);
        lprint(out);
        lprint("\n");

        lprint("Mesgs: ");
        itoa(S.NUMMESGS, out, 10);
        lprint(out);
        lprint("\n");

        lprint("Bulls: ");
        itoa(S.NUMBULLS, out, 10);
        lprint(out);
        lprint("\n");

        if (SYSOP == TRUE) {
            lprint("Sysop: IN  \022 F3 \222\n\n");
        } else {
            lprint("Sysop: OUT \022 F3 \222\n\n");
        }

        lprint("Waiting...\n");
        lprint("\021\021\021\021\021\021\021\021\021\021\021\021 \022 F1 \222=local \022 F7 \222=exit\221\221\221\221\221\221\221\221\221\221\221\221\221\235\235\235\235\235\235\235\235\235\235\235");

        /* wait for call */
        cursor_on();

        do {

            online = carrierdetect();

            if (online == TRUE) {
                waiting = FALSE;
            }

            ch = cbm_k_getin();

            if (ch == 133) {           /* F1 - LOCAL */
                waiting = FALSE;
                online = TRUE;
                LOCALMODE = TRUE;
            } else if (ch == 134) {    /* F3 - SYSOP STATUS TOGGLE */
                if (SYSOP == FALSE) {
                    SYSOP = TRUE;
                    lprint("\221\221\235\235\235IN \021\021");
                    cursor_on();
                } else {
                    SYSOP = FALSE;
                    lprint("\221\221\235\235\235OUT\021\021");
                    cursor_on();
                }
            } else if (ch == 136) {    /* F7 - EXIT */
                waiting = FALSE;
                online = FALSE;
                running = FALSE;
            }

        } while (waiting == TRUE);

        if (online == TRUE) {

            if (LOCALMODE == FALSE) {

                answer_call();

                showfile("banner");

                logon();

                if (U.ID != 0) {

                    LOGGINGON = FALSE;
                    S.NUMCALLS++;

                    welcome();

                    showfile("motd");
                    pause();
                    main_menu();
                }

            } else {

                loaduser(1);
                main_menu();

            }

        }

        if (LOCALMODE == TRUE) {
            hangup();
        }

        hangup();
        close_comms();

    } while (running == TRUE);

    /* shut down... */
    lprint("\223\005\016\010\n\n\n\n\n\n\n\n\n\nshutting down...");
    cursor_on();

    lprint("\n\n");

    return 0;

}

/* program functions */
void open_comms() {

    char ch;
    char *params = "x";
    params[0] = 8;

    close_comms();

    cbm_open(5,2,3,params);
    POKE(169,192);

    sleep();

    /* toss a null */
    cbm_k_chkin(5);
    ch = cbm_k_getin();
    cbm_k_clrch();

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

void answer_call() {

    char ch = 0, out[3], i, j;

    print("\223\n\nPress RETURN:");

    do {
        ch = getch();
        if (ch == 255) {
            /* carrier drop - fall through */
            ch = 13;
        }
    } while (ch != 13);

    print("\n");
    out[0] = '.';
    out[1] = 7;
    out[2] = 0;
    for (i = 0; i < 2; i++) {
        print(out);
        beep();
        for (j = 0; j < 10; j++) {
            sleep();
        }
    }
    print("\n");

}

void logon() {


    char done = FALSE, online;
    int tries = 0, v;

    memset(&U, 0, sizeof(U));

    do {

        tries++;

        if (tries > 3) {

            print("\n\nToo many tries!\n");
            done = TRUE;

        } else {

            print("\005\n\nEnter User ID or NEW:\n");
            print("(? to list all IDs)\n");
            print(">");
            input('A', 0, 3, FALSE);

            online = carrierdetect();

            if (online == FALSE) {

                U.ID = 0;
                done = TRUE;
                break;

            } else {

                trim(I);

                if (strcmp(strupper(I), "NEW") == 0) {

                    /* new user application */
                    print("\nNew User Application...\n\n");
                    newuser();
                    if (U.ID != 0) {
                        done = TRUE;
                        break;
                    }

                } else if (strcmp(I, "?") == 0) {

                    print("\223\022USER\222LOG\n\n");
                    showfile("userlog");
                    print("\n");
                    pause();
                    tries--;

                } else {

                    v = atoi(I);          /* try to convert to int */

                    if ((v > 0) && (v < 255)) {

                        U.ID = (char)v;

                        print("\nEnter Your Password:\n");
                        print(">");
                        input('P', 3, 10, FALSE);

                        print("\nChecking...");

                        if (loaduser(U.ID) == 0) {

                            if (strcmp(U.PASSWORD, I) == 0) {

                                /* SUCCESS! */
                                done = TRUE;
                                break;

                            } else {

                                U.ID = 0;
                                print("\nINVALID PASSWORD!\n");

                            }

                        } else {

                            U.ID = 0;
                            print("\nINVALID RECORD!\n");

                        }

                    } else {

                        U.ID = 0;
                        print("\nINVALID ID!\n");

                    }

                }

            }

        }

    } while (done == FALSE);

}

void welcome() {

    char out[5];

    print("\223Hi ");
    print(U.REALNAME);
    print("!\n\n");

    print("USERNAME:");
    print(U.USERNAME);
    print("\n");

    print("LST USER:");
    print(S.LASTCALLER);
    print("\n");

    print("LST CALL:");
    print(U.LASTLOGON);
    print("\n");

    print(" # CALLS:");
    itoa(U.CALLS, out, 10);
    print(out);
    print("\n");

    print(" USER ID:");
    itoa((int)U.ID, out, 10);
    print(out);;
    print("\n");

    print("SECURITY:");
    clear(out);
    itoa(U.SECURITY, out, 10);
    print(out);
    print("\n");

    print("NEW MSGS:");
    print("0");
    print("\n");

    print("SYSOP IS:");
    if (SYSOP == TRUE) {
        print("\022IN!\222");
    } else {
        print("OUT");
    }
    print("\n");

    /* update last caller */
    strcpy(S.LASTCALLER, U.USERNAME);
    savestats();

    pause();

}

void main_menu() {

    char ch, visiting = TRUE, yn, i, menu[6] = "menuX";

    menu[4] = U.SECURITY + 48;

    if(LOCALMODE == FALSE) {
        showfile(menu);
    } else {
        print("\223");
    }

    do {

        print("\005\n\nCOMMAND:");

        ch = get_command();

        switch(ch) {
            case 'B':
                bulletins();
                break;
            case 'V':
                print("\223\005\022USER\222LOG\n\n");
                showfile("userlog");
                break;
            case 'I':
                showfile("info");
                break;
            case 'T':
                showtime();
                break;
            case 'C':
                chatmode();
                break;
            case 'S':
                changepassword();
                break;
            case 'Z':
                sysop_menu();
                break;
            case 'O':
                yn = confirm();
                if (yn == TRUE) {
                    print("\n\nThanks for calling!\n");
                    print("Call back soon...\n");
                    /* pause to let modem buffer clear */
                    for (i=0; i<10; i++) {
                        sleep();
                    }
                    hangup();
                    visiting = FALSE;
                }
                break;
            case 'H':
            case '?':
                showfile(menu);
                break;
            case 255:
                print("\n\nCarrier Dropped!\n");
                visiting = FALSE;
                break;
            default:
                askforhelp();
        }

    } while (visiting == TRUE);

}

void bulletins() {

    char reading = TRUE;
    char online, out[7];
    int b;

    if (U.SECURITY > 0) {

        do {

            showfile("bulletins");

            print("\005\n\nBULLETIN:");

            input('U', 1, 3, FALSE);
            print("\n");

            strupr(I);
            trim(I);

            b = atoi(I);

            if (strcmp(I, "X") == 0) {
                reading = FALSE;
                break;
            } else if ((b >=1) && (b <=255)) {
                /* show bulletin */
                print("\223");
                clear(out);
                strcpy(out, "b ");
                strcat(out, I);
                showfile(out);
                pause();
            } else if ((strcmp(I, "?") == 0) || (strcmp(I, "H") == 0)) {
                /* do nothing... it's gonna show the listing again... */
            } else {
                print("\nPress ? for List!");
                pause();
            }

            online = carrierdetect();

            if (online == FALSE) {
                reading = FALSE;
            }

        } while (reading == TRUE);
    } else {
        askforhelp();
    }


}

void showtime() {

    long x;
    char out[5];

    gettime();

    if (AD == 1) {
        x = U.TU - (TIME - 86400);
    } else if (AD == 0) {
        x = U.TU - TIME;
    }

    x = x / 60 + 1;

    ltoa(x, out, 10);

    print("\n\n");
    print(out);
    print(" mins left\n");

}

void chatmode() {

    int t,k;
    char ch;
    char chat;
    char sb = 10;

    print("\nPaging the Sysop:\n");
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

    if (chat == TRUE) {
        print("\223CHAT MODE!\n\n");
        print("\007");
        lprint("\nUSER:");
        lprint(U.USERNAME);
        lprint("\n\n");
        lprint("\022 F3 \222 to exit...\n\n");
        chat = TRUE;
        cursor_on();

        do {
            ch = getch();
            if ((ch == 134) || (ch == 255)) {
                chat = FALSE;
            } else {
                putch(ch);
            }

        } while (chat == TRUE);

        print("\nExiting chat mode...\n");
        cursor_off();
    } else {
        print("\n\nSorry the Sysop is\nnot here.\n");
    }

}

void sysop_menu() {

    char ch, sysoping = TRUE;
    int i;

    if (U.SECURITY > 4) {

        do {

            print("\005\n\nSYSOP:");

            ch = get_command();

            switch(ch) {
                case 'A':
                    /* add a user - recycle New User function */
                    newuser();
                    break;
                case 'E':
                    /* edit user */
                    print("\n\nUser ID:");
                    input('A', 0, 3, FALSE);
                    i = atoi(I);
                    if ((i > 0) && (i < 255)) {
                        edituser((char)i, TRUE);
                    }
                    break;
                case 'G':
                    print("\n\nGenerating user log...");
                    genuserlist();
                    print("\nDONE!\n");
                    break;
                case 'F':
                    /* file editor */
                    file_editor(0);
                    break;
                case 'B':
                    /* backup user and file logs */
                    break;
                case 'R':
                    /* restore user or file log */
                    break;
                case 'S':
                    /* show/fix stats file */
                    break;
                case 'C':
                    /* create user log DANGER */
                    break;
                case 'X':
                    sysoping = FALSE;
                    break;
                case 'H':
                case '?':
                    showfile("sysop");
                    break;
                case 255:
                    print("\n\nCarrier Dropped!\n");
                    sysoping = FALSE;
                    break;
                default:
                    askforhelp();
            }

        } while (sysoping == TRUE);

    } else {
        askforhelp();
    }

}

void hangup() {

    sleep();
    POKE(RS,151);
    sleep();
    POKE(RS,PEEK(RS)&32);
    sleep();

    if (U.ID != 0) {
        U.CALLS++;
        saveuser(U.ID);
    }

    savestats();

    memset(&U, 0, sizeof(U));
    LOCALMODE = FALSE;

}

void close_comms() {

    hangup();
    cbm_close(5);

}

/* utility functions */
void loadstats() {

    int i, j, c;
    char e, ch, buffer[40], t[15];

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
    }

    cbm_close(3);
    cbm_close(15);

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

void showfile(char *filename) {

    char fullfile[32];
    char st, a, e;
    int i = 0;

    strcpy(fullfile, filename);
    strcat(fullfile, ",s,r");

    cursor_off();

    cbm_open(15,8,15,"");
    cbm_open(2,8,2,fullfile);

    e = errorcheck();

    if (e == 0) {

        st = 0;

        do {

            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();
            st = cbm_k_readst();

            if (st == 0) {
                putch(a);
            } else if (st == 64) {
                /* end of file */
            } else {
                e = errorcheck();
            }

            /* check for pause or quit */
            st = pauseorquit(st);

        } while (st == 0);
    }

    cbm_close(2);
    cbm_close(15);

    cursor_on();

}

char open_userfile() {

    char e;
    char filename[9] = "users,l,x";
    filename[8] = 76;

    cbm_open(15,8,15,"");
    cbm_open(1,8,2,filename);

    e = errorcheck();

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

    char rec[76], out[5], e;

    clear(rec);
    memset(&U, 0, sizeof(U));

    if (open_userfile() == 0) {
        if (gotouser(id) == 0) {
            cbm_read(1, rec, 76);
            e = errorcheck();
        }

    }

    close_userfile();

    if ((strlen(rec) == 75) && (rec[0] != 255)) {

        U.ID = id;

        strncpy(U.USERNAME, rec, 12);
        trim(U.USERNAME);

        strncpy(U.PASSWORD, rec + 12, 10);
        trim(U.PASSWORD);

        strncpy(U.REALNAME, rec + 22, 20);
        trim(U.REALNAME);

        strncpy(U.FROM, rec + 42, 15);
        trim(U.FROM);

        strncpy(out, rec + 57, 1);
        out[1] = 0;
        U.SECURITY = (char)atoi(out);

        strncpy(out, rec + 58, 5);
        trim(out);
        U.CALLS = atoi(out);

        strncpy(out, rec + 63, 5);
        trim(out);
        U.LASTREAD = atoi(out);

        clear(out);

        /* month */
        strncpy(out, rec + 68, 2);
        out[2] = '\0';
        strcpy(U.LASTLOGON, out);
        strcat(U.LASTLOGON, "/");

        /* day */
        strncpy(out, rec + 70, 2);
        out[2] = '\0';
        strcat(U.LASTLOGON, out);
        strcat(U.LASTLOGON, "/");

        /* year */
        strncpy(out, rec + 72, 2);
        out[2] = '\0';
        strcat(U.LASTLOGON, "20");
        strcat(U.LASTLOGON, out);

        settimelimit();

    } else {
        e = 1;
    }

    return e;

}

char pauseorquit(char ost) {

    char pc, online, st = ost;

    /* check for pause or quit */
    pc = cbm_k_getin();
    if ((pc != 32) && (pc != 'q') && (pc != 'Q')) {
        cbm_k_chkin(5);
        pc = cbm_k_getin();
        cbm_k_clrch();
    }
    if (pc == 32) {
        cursor_on();
        do {
            pc = cbm_k_getin();
            if ((pc != 32) && (pc != 'q') && (pc != 'Q')) {
                cbm_k_chkin(5);
                pc = cbm_k_getin();
                cbm_k_clrch();
            }
            online = carrierdetect();
            if (online == FALSE) {
                pc = 32;
            } else if ((pc == 'q') || (pc == 'Q')) {
                pc = 32;
                st = 64;
            }
        } while(pc != 32);
        cursor_off();
    } else if ((pc == 'q') || (pc == 'Q')) {
        st = 64;
    }

    return st;

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

void newuser() {

    char yn, check, id, out[5], sid=0;

    if (U.SECURITY > 4) {
        sid = U.ID;
    }

    id = nextid();

    if (id != 0) {

        if (sid != 0) {
            print("\223\005\022ADD\222USER\n\n");
        } else {
            showfile("newuser1");
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

            itoa(U.ID, out, 10);
            print(out);
            print("\nPassword= ");
            print(U.PASSWORD);
            print("\n");

            pause();

            showfile("newuser2");
        }

        pause();

    } else {
        print("\n\nUSER LOG FULL!\n");
    }


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

    char sid, out[5], yn = FALSE;

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
        itoa(U.SECURITY, out, 10);
        strcpy(I, out);
        print(out);
        input('A', 1, 1, TRUE);
        strcpy(U.FROM, I);
        trim(U.FROM);

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
    input('A', 3, 10, FALSE);

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



}

void saveuser(char id) {

    char rec[76], out[10], e;
    int i;

    for (i=0; i<76; i++) {
        rec[i] = 32;
    }

    if (open_userfile() == 0) {
        if (gotouser(id) == 0) {

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

            itoa((int)U.SECURITY, out, 10);
            rec[57] = out[0];

            itoa((int)U.CALLS, out, 10);
            for (i=0; i<strlen(out); i++) {
                rec[i+58] = out[i];
            }

            itoa((int)U.LASTREAD, out, 10);
            for (i=0; i<strlen(out); i++) {
                rec[i+63] = out[i];
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

    close_userfile();


}

void genuserlist() {

    int i, users = 0;
    char temp[13], out[20];

    open_userfile();
    cbm_open(3, 8, 3, "@0:userlog,s,w");

    for(i=1; i<255; i++) {
        gotouser((char)i);
        cbm_read(1, temp, 12);
        if (temp[0] != 255) {
            users++;
            clear(out);
            itoa(i, out, 10);
            strcat(out, " ");
            strcat(out, temp);
            strcat(out, "\n");
            cbm_write(3, out, strlen(out));
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

void close_userfile() {

    cbm_close(1);
    cbm_close(15);

}

void file_editor(int msgnum) {

    char filename[32], fullname[32], drive, err[32], yn;

    if (msgnum == 0) {
        print("\223\005\022FILE\222EDITOR\n\n");
        print("Filename ($ = dir)?\n>");
        input('A', 0, 32, FALSE);
        if (strlen(I) > 0) {
            strcpy(filename, I);
            trim(filename);
            print("\nDrive? (8-10)\n>");
            input('A', 0, 2, FALSE);
            if (strlen(I) > 0) {
                drive = (char)atoi(I);
                if (strcmp(filename,"$") == 0) {
                    directory(drive);
                } else {
                    strcpy(fullname, filename);
                    strcat(fullname, ",s,r");

                    cbm_open(15,drive,15,"");
                    cbm_open(2,drive,2,fullname);

                    cbm_read(15, err, 2);
                    err[2] = 0;

                    cbm_write(15, "i0", 2);

                    cbm_close(2);
                    cbm_close(15);

                    if (strcmp(err, "62") == 0) {
                        print("New file... create?\n");
                        yn = confirm();
                        if (yn == TRUE) {
                            strcpy(fullname, filename);
                            strcat(fullname, ",s,w");
                            cbm_open(2,drive,2,fullname);
                            cbm_close(2);
                            file_edit(filename, drive, 0);
                        }
                    } else {
                        file_edit(filename, drive, 0);
                    }

                }

            }
        }
    }

}

void file_edit(char *filename, char drive, int msgnum) {

    int i, j, line, ln, df;
    unsigned int QX, Q1, Q2;
    char editing = TRUE, out[10], a, ORIG[40], yn;

    QX = file_load(filename, drive);

    if (QX > 0) {

        file_viewer(QX, FALSE);

        do {

            input('A', 0, 21, FALSE);

            if ((strlen(I) == 2) && (I[0] == 47)) {

                /* Slash Commands */

                strupper(I);

                switch(I[1]) {
                    case 'A':
                        /* abort */
                        editing = FALSE;
                        break;
                    case 'S':
                        /* save */
                        file_save(filename, drive, QX);
                        editing = FALSE;
                        break;
                    case 'L':
                        /* list */
                        file_viewer(QX, TRUE);
                        break;
                    case 'P':
                        /* preview */
                        file_viewer(QX, FALSE);
                        break;
                    case 'E':
                        /* edit line */
                        print("\nLine # to edit? ");
                        input('A', 0, 3, FALSE);
                        line = atoi(I);
                        if ((line > 0) && (line < 1000)) {
                            print("Searching...");
                            Q1 = QR;
                            Q2 = QR;
                            ln = 0;
                            for (i = QR; i <= QR + QX; i++) {
                                a = PEEK(i);
                                if (a == 13) {
                                    ln++;
                                    if (ln == line) {
                                        Q2 = i - 1;
                                        i = QR + QX;
                                        break;
                                    } else {
                                        Q1 = i;
                                    }
                                }
                            }
                            if ((Q1 >= QR) && (Q1 <= QR + QX) && (Q2 >= QR) && (Q2 <= QR + QX)) {

                                /* DEBUG DANGER AREA START
                                cprintf("\r\nQ1:%u\r\n", Q1);
                                cprintf("Q2:%u\r\n", Q2);
                                 DEBUG DANGER AREA END */

                                /* we have our line! */
                                clear(ORIG);
                                j = 0;
                                for (i = Q1; i <= Q2; i++) {
                                    ORIG[j] = PEEK(i);
                                    j++;
                                }
                                print("\nOriginal Line:\n>");
                                print(ORIG);
                                print("\n\nEnter New Line:\n>");
                                input('A', 0, 21, FALSE);
                                if (strlen(I) > 1) {
                                    print("\nWorking...");
                                    df = strlen(I) - strlen(ORIG);
                                    if (df < 0) {
                                        for (i=Q2; i<=QR+QX; i++) {
                                            POKE(i+df, PEEK(i));
                                        }
                                    } else if (df > 0) {
                                        for (i=QR+QX; i>=Q2; i--) {
                                            POKE(i+df, PEEK(i));
                                        }
                                    }
                                    QX = QX + df;
                                    for (i=0; i<strlen(I); i++) {
                                        POKE(i+Q1,I[i]);
                                    }
                                    print(" DONE!\n");
                                } else {
                                    print("\nNO CHANGES MADE.\n");
                                }
                            } else {
                                print("\022NOT FOUND\n");
                            }
                        }
                        pause();
                        file_viewer(QX, FALSE);
                        break;
                    case 'D':
                        /* delete line */
                        print("\nLine # to delete? ");
                        input('A', 0, 3, FALSE);
                        line = atoi(I);
                        if ((line > 0) && (line < 1000)) {
                            print("Searching...");
                            Q1 = QR;
                            Q2 = QR;
                            ln = 0;
                            for (i = QR; i <= QR + QX; i++) {
                                a = PEEK(i);
                                if (a == 13) {
                                    ln++;
                                    if (ln == line) {
                                        Q2 = i;
                                        i = QR + QX;
                                        break;
                                    } else {
                                        Q1 = i+1;
                                    }
                                }
                            }
                            if ((Q1 >= QR) && (Q1 <= QR + QX) && (Q2 >= QR) && (Q2 <= QR + QX)) {
                                /* we have our line! */
                                clear(ORIG);
                                j = 0;
                                for (i = Q1; i < Q2; i++) {
                                    ORIG[j] = PEEK(i);
                                    j++;
                                }
                                ORIG[j] = 0;
                                print("\nDelete Line:\n>");
                                print(ORIG);
                                yn = confirm();
                                if (yn == TRUE) {
                                    print("\nWorking...");
                                    df = strlen(ORIG) + 1;
                                    for (i=Q2; i<QR+QX; i++) {
                                        POKE(i-df, PEEK(i));
                                    }
                                    QX = QX - df;
                                    print(" DONE!\n");
                                }
                            } else {
                                print("\022NOT FOUND\n");
                            }

                        }
                        break;
                    case 'H':
                    case '?':
                        print("\nSlash Commands:\n");
                        print("/a:Abort /s:Save \n/l:List /p:Preview \n/e:Edit /d:Delete\n");
                        break;
                    default:
                        print("? unknown ?\nEnter /? for help!\n");
                }

            } else {
                /* update RAM */
                strcat(I, "\n");
                for (i=0; i<=strlen(I); i++) {
                    QX++;
                    POKE(QR + QX,I[i]);
                }
            }

        } while (editing == TRUE);

    }

    /* STEP 3 - UPDATE FILE ON DISK FROM MEMORY */

}

unsigned int file_load(char *filename, char drive) {

    unsigned int QX = 0;
    char fullname[32], st, e, a;

    /* LOAD FILE INTO MEMORY */
    strcpy(fullname, filename);
    strcat(fullname, ",s,r");

    cbm_open(15,drive,15,"");
    cbm_open(2,drive,2,fullname);

    e = errorcheck();

    if (e == 0) {

        st = 0;
        print("\223\005Loading...");

        do {

            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();
            st = cbm_k_readst();

            if ((st != 0) && (st != 64)) {
                e = errorcheck();
                QX = 0;
                break;
            } else if (a != 0) {
                POKE((QR + QX), a);
                QX++;
            }

        } while (st == 0);

    } else {
        QX = 0;
    }

    cbm_close(2);
    cbm_close(15);

    return QX;

}

void file_save(char *filename, char drive, unsigned int QX) {

    int i;
    char fullname[32], e;

    /* SAVE FILE FROM MEMORY */
    strcpy(fullname, "@0:");
    strcat(fullname, filename);
    strcat(fullname, ",s,w");

    cbm_open(15,drive,15,"");
    cbm_open(2,drive,2,fullname);

    e = errorcheck();

    if (e == 0) {

        print("\223\005Saving...");

        for (i = QR; i <= QR + QX; i++) {
            cbm_k_ckout(2);
            cbm_k_bsout(PEEK(i));
            cbm_k_clrch();

            e = errorcheck();

            if (e != 0) {
                break;
            }
        }

        print("DONE!\n");

    }

    cbm_close(2);
    cbm_close(15);

}

void file_viewer(unsigned int QX, char listing) {

    int i, line = 2;
    char out[5], ch, st;

    QX = QX + 40960UL;

    print("\223\005");

    if (listing == TRUE) {
        print("Line 1:\n");
    }

    for (i = 40960UL; i < QX; i++) {

        ch = PEEK(i);

        if ((ch == 13) && (listing == TRUE)) {
            print("\n\n");
            itoa(line, out, 10);
            print("Line ");
            print(out);
            print(":\n");
            line++;
            st = pauseorquit(0);
        } else {
            putch(ch);
            st = pauseorquit(0);
        }

        if (st == 64) {
            i = QX;
            break;
        }

    }


}

void directory(char drive) {

    char bam[142];
    int x;

    /* stub to read directory */
    cbm_open(2,8,2,"$");
    cbm_read(2, bam, 141);


}

void pause() {

    char ch;
    print("\n\nPress any key...");
    ch = getch();
    print("\n");

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

/* system functions */
void print(char *str) {

    char ch;

    cursor_off();

    while (*str) {

        ch = *str++;
        cbm_k_bsout(ch);

        if (LOCALMODE != TRUE) {
            cbm_k_ckout(5);
            cbm_k_bsout(ch);
            cbm_k_clrch();
        }

    }

}

void lprint(char *str) {

    char ch;

    cursor_off();

    while (*str) {

        ch = *str++;
        cbm_k_bsout(ch);

    }

}

void putch(char ch) {

    cursor_off();

    /* print local */
    cbm_k_bsout(ch);

    /* beep if required */
    if (ch == 7) {
        beep();
    }

    /* print remote */
    if (LOCALMODE == FALSE) {
        cbm_k_ckout(5);
        cbm_k_bsout(ch);
        cbm_k_clrch();
    }

}

void input(char fmt, int min, int max, char editmode) {

    /* uses global I[] array to return result */

    /*
     * fmt =
     *   'A' alphanumeric,
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

    int i;
    char done = FALSE, ch, hc = 0, np;

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

    do {

        ch = getch();

        if (ch == 255) {
            done = TRUE;
            break;
        } else {
            if (ch == 13) {
                if (hc >= min) {
                    putch(ch);
                    done = TRUE;
                    break;
                } else {
                    clear(I);
                    print("\n\n\022MIN ");
                    itoa(min, I, 10);
                    print(I);
                    print(" CHARS REQ!\nTry again...\n>");
                    clear(I);
                    i = 0;
                }
            } else if (ch == 20) {
                if (i > 0) {
                    ch = I[i-1];
                    I[i] = '\0';
                    i--;
                    hc = hc - hardchar(ch);
                    np = hardchar(ch);
                    if (np == 1) {
                        putch(20);
                    }
                }
            } else if ((hc < max) && (i < 40)) {

                /* filter char here based on 'fmt' */

                if ((ch != 17) && (ch != 19) && (ch != 29) && (ch != 145) && (ch != 147) && (ch != 148) && (ch != 157)) {
                    if (fmt == 'P') {
                        putch('*');
                    } else {
                        putch(ch);
                    }
                    I[i] = ch;
                    i++;
                    hc = hc + hardchar(ch);
                }
            }
        }

    } while (done == FALSE);

}

char getch(void) {

    char ch = 0;
    char online;

    cursor_on();

    do {

        /* XLOCAL */
        ch = cbm_k_getin();

        if((ch == 0) && (LOCALMODE == FALSE)) {
            /* XREMOTE */
            cbm_k_chkin(5);
            ch = cbm_k_getin();
            cbm_k_clrch();
        }

        online = carrierdetect();

        if (online == FALSE) {
            ch = 255;
        }

    } while(ch == 0);

    return(ch);

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

    print("\222");

}

void clear(char *str) {
    while (*str) {
        *str++ = '\0';
    }
}

void cursor_on() {

    if (CURSORSTATUS == FALSE) {
        POKE(212, 0);
        POKE(216, 0);

        if (PEEK(204) != 0) {
            asm("ldy #$00");
            asm("sty $cc");
            CURSORSTATUS = TRUE;
        }

    }

}

void cursor_off() {
    if (CURSORSTATUS == TRUE) {
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
        CURSORSTATUS = FALSE;
    }
}

void beep() {

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
    char out[3];

    /* MONTH */
    POKE(RT,8);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, out, 10);
    if (v < 10) {
        strcpy(DATE, "0");
        strcat(DATE, out);
    } else {
        strcpy(DATE, out);
    }
    strcat(DATE,"/");

    /* DAY */
    POKE(RT,7);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, out, 10);
    if (v < 10) {
        strcat(DATE, "0");
    }
    strcat(DATE,out);

    /* YEAR */
    POKE(RT,9);
    v = PEEK(RT+1);
    v = RTCconvert(v);
    itoa(v, out, 10);
    strcat(DATE, "/20");
    strcat(DATE,out);

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

char hardchar(char ch) {

    /* examines supplied char ch
     * returns 1 if char is a printing char (hard char)
     * returns 0 if char is a non-printing char (color, etc)
     */

    char hc = 0;

    if (((ch >= 32) && (ch <= 127)) || ((ch >= 161) && (ch <= 254)))  {
        hc = 1;
    }

    return hc;

}
