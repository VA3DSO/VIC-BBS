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
#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"
#include "bbs.h"
#include "uutils.h"
#include "futils.h"

static char RELOAD = FALSE;

void main(void) {

    char running = TRUE, id = 0;
    char waiting, ch;

    if ((PEEK(BS_MODULE) == 0) || (PEEK(BS_MODULE) == 255)) {

        /* firing up... */
        POKE(SV, 0);                       /* turn off speaker volume */
        POKE(SC, 8);                       /* black on black screen */

        print("\223\005\016\010firing up...");

        if (open_userfile() == 62) {
            print("\n\nUSER file missing!\n\nCreate from SYSOP\nmenu!\n\n");
            pause(FALSE);
        } else {
            close_userfile();
        }

        open_comms();

        waiting = TRUE;
        ONLINE = FALSE;
        LOCALMODE = FALSE;
        LOGGINGON = TRUE;

        id = 0;
        POKE(BS_LOCALMODE,0);
        POKE(BS_ID,0);
        POKE(BS_MODULE,0);

    } else {

        id = PEEK(BS_ID);
        LOCALMODE = PEEK(BS_LOCALMODE);

        POKE(BS_LOCALMODE,0);
        POKE(BS_ID,0);
        POKE(BS_MODULE,0);

        waiting = FALSE;
        ONLINE = TRUE;
        /* LOCALMODE loaded above */
        LOGGINGON = FALSE;
        RELOAD = TRUE;                 /* to skip main menu help on returning */

    }

    loadstats();

    do {

        if (ONLINE == FALSE) {

            print("\223\237\016\022VIC\222\005BBS           \237V1.04\005\n");
            print("Last : ");
            print(S.LASTCALLER);
            print("\n");

            print("Calls: ");
            itoa(S.NUMCALLS, O, 10);
            print(O);
            print("\n");

            print("Users: ");
            itoa(S.NUMUSERS, O, 10);
            print(O);
            print("\n");

            print("Mesgs: ");
            itoa(S.NUMMESGS, O, 10);
            print(O);
            print("\n");

            print("Bulls: ");
            itoa(S.NUMBULLS, O, 10);
            print(O);
            print("\n");

            if (SYSOP == TRUE) {
                print("Sysop: IN  \022 F3 \222\n\n");
            } else {
                print("Sysop: OUT \022 F3 \222\n\n");
            }

            print("Waiting...\n");
            print("\021\021\021\021\021\021\021\021\021\021\021\021 \022 F1 \222=local \022 F7 \222=exit\221\221\221\221\221\221\221\221\221\221\221\221\221\235\235\235\235\235\235\235\235\235\235\235");

            do {

                ch = cbm_k_getin();

                if (ch == 133) {           /* F1 - LOCAL */
                    waiting = FALSE;
                    ONLINE = TRUE;
                    LOCALMODE = TRUE;
                    break;
                } else if (ch == 134) {    /* F3 - SYSOP STATUS TOGGLE */
                    if (SYSOP == FALSE) {
                        SYSOP = TRUE;
                        print("\221\221\235\235\235IN \021\021");
                    } else {
                        SYSOP = FALSE;
                        print("\221\221\235\235\235OUT\021\021");
                    }
                } else if (ch == 136) {      /* F7 - EXIT */
                    waiting = FALSE;
                    ONLINE = FALSE;
                    running = FALSE;
                    hangup();
                    break;
                }

                ONLINE = carrierdetect();

                if (ONLINE == TRUE) {
                    waiting = FALSE;
                }

            } while (waiting == TRUE);
        }

        if (ONLINE == TRUE) {

            /* comment the following 'if' statement out if you'd like local mode to
             * have to go through login process instead of jumping to Command */

            if (LOCALMODE == TRUE) {
                id = 1;
            }

            if (id == 0) {

                answer_call();

                showfile("banner", 8, FALSE, 0);

                logon();

                if (U.ID != 0) {

                    LOGGINGON = FALSE;

                    welcome();

                    showfile("motd", 8, TRUE, 0);

                    print("\n\005View The Wall (Y/N)?");
                    ch = get_command();

                    if (ch == 'Y') {
                        the_wall();
                    }

                    main_menu();

                } else {

                    waiting = TRUE;
                    hangup();

                }

            } else {

                loaduser(id);
                main_menu();

            }

        }

        U.CALLS++;
        saveuser(U.ID);
        S.NUMCALLS++;
        savestats();

        if (LOCALMODE == TRUE) {
            /* extra hangup() */
            hangup();
        }

        id = 0;
        waiting = TRUE;
        hangup();


    } while (running == TRUE);

    /* shutting down... */
    print("\223\005\016\010shutting down...");

    hangup();
    close_comms();

    print("\n\n");

}

void open_comms() {

    char ch;
    char *params = "x";
    params[0] = 8;

    cbm_close(5);

    cbm_open(5,2,3,params);
    POKE(169,192);

    sleep();

    /* toss a null */
    cbm_k_chkin(5);
    ch = cbm_k_getin();
    cbm_k_clrch();

}

void answer_call() {

    char ch = 0, i, j;

    print("\223\005\n\nPress RETURN:");

    do {
        ch = getch();
        if (ch == 255) {
            /* carrier drop - fall through */
            ch = 13;
        }
    } while (ch != 13);

    print("\n");
    for (i = 0; i < 2; i++) {
        print(".");
        putch(7);
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
                    newuser();
                    if (U.ID != 0) {
                        done = TRUE;
                        break;
                    }

                } else if (strcmp(I, "?") == 0) {

                    print("\223\237\022USER\222\005LOG\n\n");
                    showfile("userlog", 8, FALSE, 0);
                    print("\n\n");
                    pause(FALSE);
                    tries--;

                } else {

                    v = atoi(I);          /* try to convert to int */

                    if ((v > 0) && (v < 255)) {

                        U.ID = (char)v;

                        print("\005\nEnter Your Password:\n");
                        print(">");
                        input('P', 3, 10, FALSE);

                        print("\005\nChecking...");

                        if (loaduser(U.ID) == 0) {

                            if (U.USERNAME[0] != 255) {

                                if (strcmp(strupper(U.PASSWORD), strupper(I)) == 0) {

                                    /* SUCCESS! */
                                    done = TRUE;
                                    break;

                                } else {

                                    U.ID = 0;
                                    print("\nINVALID PASSWORD!\n");

                                }

                            } else {

                                U.ID = 0;
                                print("\nINVALID ID!\n");

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

    int newmsgs = 0;

    sprintf(O, "\223\005Hi \237%s\005!\n\n", U.REALNAME); print(O);
    sprintf(O, "USERNAME:%s\n", U.USERNAME); print(O);
    sprintf(O, "LST USER:%s\n", S.LASTCALLER); print(O);
    sprintf(O, "LST CALL:%s\n", U.LASTLOGON); print(O);
    sprintf(O, " # CALLS:%i\n", U.CALLS); print(O);
    sprintf(O, " USER ID:%i\n", U.ID); print(O);
    sprintf(O, "SECURITY:%i\n", U.SECURITY); print(O);

    newmsgs = S.NUMMESGS - U.LASTREAD;

    sprintf(O, "NEW MSGS:%i\n", newmsgs); print(O);

    print("SYSOP IS:");
    if (SYSOP == TRUE) {
        print("\022IN!\222");
    } else {
        print("OUT");
    }
    print("\n\n");

    /* update last caller */
    strcpy(S.LASTCALLER, U.USERNAME);
    savestats();

    pause(FALSE);

}

void main_menu() {

    char ch, visiting = TRUE, yn, i, menu[6] = "menuX";

    menu[4] = U.SECURITY + 48;

    if((LOCALMODE == FALSE) && (RELOAD == FALSE)) {
        showfile(menu, 8, FALSE, 0);
    } else if (LOCALMODE == TRUE) {
        print("\223");
    }

    if (PEEK(BS_REPLY) == 1) {
        POKE(BS_REPLY, 0);
        read_messages();
    }

    do {

        print("\005\n\nCOMMAND\237:\005");

        ch = get_command();

        switch(ch) {
            case 'B':
                bulletins();
                break;
            case 'P':
                strcpy(W, "");
                post_message(0);
                break;
            case 'R':
                read_messages();
                break;
            case 'W':
                the_wall();
                break;
            case '$':
                /* directory of downloads: partition 2 */
                directory(8, '2');
                break;
            case 'F':
                showfile("filelist", 8, FALSE, 0);
                break;
            case 'D':
                if (U.SECURITY >= 3) {
                    POKE(BS_ACTION, 1);
                    bootstrap("FILES");
                } else {
                    askforhelp();
                }
                break;
            case 'U':
                if (U.SECURITY >= 3) {
                    showfile("upnote", 8, TRUE, 0);
                    POKE(BS_ACTION, 2);
                    bootstrap("FILES");
                } else {
                    askforhelp();
                }
                break;
            case 'V':
                print("\223\237\022USER\222\005LOG\n\n");
                showfile("userlog", 8, FALSE, 0);
                print("\n\n");
                pause(FALSE);
                break;
            case 'I':
                showfile("info", 8, TRUE, 0);
                break;
            case 'T':
                showtime();
                break;
            case 'C':
                chatmode(TRUE);
                break;
            case 'S':
                changepassword();
                break;
            case 'G':
                /* GAMES! */
                if (U.SECURITY >= 3) {
                    bootstrap("GAMES");
                } else {
                    askforhelp();
                }
                break;
            case 'Z':
                if (U.SECURITY >= 5) {
                    savestats();
                    bootstrap("SYSOP");
                } else {
                    askforhelp();
                }
                break;
            case 'O':
                yn = confirm();
                if (yn == TRUE) {
                    print("\237\n\nThanks for calling!\n");
                    print("\005Call back soon...\n");
                    /* pause to let modem buffer clear */
                    for (i=0; i<10; i++) {
                        sleep();
                    }
                    visiting = FALSE;
                }
                break;
            case 'H':
            case '?':
                showfile(menu, 8, FALSE, 0);
                break;
            case 255:
                print("\005\n\nCarrier Dropped!\n");
                visiting = FALSE;
                break;
            default:
                askforhelp();
        }

    } while (visiting == TRUE);

}

void showtime() {

    long x;

    gettime();

    if (AD == 1) {
        x = U.TU - (TIME - 86400);
    } else if (AD == 0) {
        x = U.TU - TIME;
    }

    x = x / 60 + 1;

    ltoa(x, O, 10);

    print("\237\n\n");
    print(O);
    print("\005 mins left\n");

}

void bulletins() {

    char reading = TRUE;
    char online;
    int b;

    if (U.SECURITY >= 1) {

        do {

            showfile("bulletins", 8, FALSE, 0);

            print("\005\n\nBULLETIN\036:\005");

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
                print("\223\005");
                clear(O);
                strcpy(O, "b ");
                strcat(O, I);
                showfile(O, 8, FALSE, 0);
                print("\n-- \022END\222 --\n");
                pause(FALSE);
            } else if ((strcmp(I, "?") == 0) || (strcmp(I, "H") == 0)) {
                /* do nothing... it's gonna show the listing again... */
            } else {
                print("\nPress ? for List!");
                pause(FALSE);
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

void read_messages() {

    int lm = S.NUMMESGS - 255, lr = U.LASTREAD;
    char reading = TRUE;
    char online, ch;
    int reply, dm;

    if (U.SECURITY >= 2) {

        if (lm < 1) {
            lm = 1;
        }

        print("\223\236\022READ\222\005MESSAGES\n\n");
        print("\022RETURN\222 - Next Msg\n");
        print("\022P\222 - Previous Msg\n");
        print("\022#\222 - Enter Msg #\n");
        print("\022A\222 - Read Again\n");
        print("\022R\222 - Reply\n");
        print("\022G\222 - Glance Up\n\n");
        print("\022X\222 - Exit Messages\n\n");

        sprintf(O, "Low Msg: %i\nHigh Msg: %i\nLast Read: %i\n", lm, S.NUMMESGS, lr);
        print(O);

        clear(W);

        do {

            sprintf(O, "\005\n\nMESSAGES\236(\005%d\236):\005", lr); print(O);
            ch = get_command();

            online = carrierdetect();

            if (online == FALSE) {
                reading = FALSE;
            }

            if (ch == 13) {
                ch = 'N';
            }

            switch (ch) {
                case 'N':
                    lr++;
                    if ((lr >= lm) && (lr <= S.NUMMESGS)) {
                        sprintf(O, "4:m %i", lr);
                        print("\223");
                        reply = showfile(O, 8, FALSE, lr);
                    } else {
                        lr--;
                        reading = FALSE;
                        print("\005\n\n - \236\022END OF MESSAGES\222\005 -\n\n");
                    }
                    break;
                case 'P':
                    lr--;
                    if ((lr >= lm) && (lr <= S.NUMMESGS)) {
                        sprintf(O, "4:m %i", lr);
                        print("\223");
                        reply = showfile(O, 8, FALSE, lr);
                    } else {
                        lr++;
                    }
                    break;
                case 'A':
                    if ((lr >= lm) && (lr <= S.NUMMESGS)) {
                        sprintf(O, "4:m %i", lr);
                        print("\223");
                        reply = showfile(O, 8, FALSE, lr);
                    }
                    break;
                case 'G':
                    if ((reply >= lm) && (reply <= S.NUMMESGS)) {
                        sprintf(O, "4:m %i", reply);
                        print("\223");
                        showfile(O, 8, FALSE, lr);
                    }
                    break;
                case 'R':
                    if (U.SECURITY >= 3) {
                        U.LASTREAD = lr;
                        saveuser(U.ID);
                        post_message(lr);
                    } else {
                        print("\nNot enough access!\n");
                    }
                    break;
                case '#':
                    sprintf(O, "\n\nLo:%i Hi:%i Lst:%i\n", lm, S.NUMMESGS, lr); print(O);
                    print("\nMsg # to read?\n>");
                    input('A', 0, 4, FALSE);
                    dm = atoi(I);
                    if ((dm >= lm) && (dm <= S.NUMMESGS)) {
                        lr = dm;
                        sprintf(O, "4:m %i", lr);
                        print("\223");
                        reply = showfile(O, 8, FALSE, lr);
                    }
                    break;
                case 'X':
                    reading = FALSE;
                    break;
                default:
                    print("\005\022P\222rev \022#\222-Msg# \022A\222gain\n\022R\222eply \022G\222lance-Up e\022X\222it\n\n");
                    sprintf(O, "Lo:%i Hi:%i Lst:%i\n", lm, S.NUMMESGS, lr);
                    print(O);
            }

        } while (reading == TRUE);

        U.LASTREAD = lr;
        saveuser(U.ID);

    } else {
        askforhelp();
    }

}

void post_message(int replynum) {

    int i;
    char e, hi, lo;
    char online;

    if (U.SECURITY >= 3) {

        print("\223\236\022NEW\222\005MESSAGE\n\n");
        print("Subject?\n>");
        strcpy(I, W);
        print(I);
        input('A', 0, 16, TRUE);

        online = carrierdetect();

        if((strlen(I) > 0) && (online == TRUE)) {

            print("\n\005\222Getting started...\n");

            S.NUMMESGS++;
            savestats();

            cbm_open(15, 8, 15, "");
            cbm_open(2, 8, 2, "@4:temp,s,w");

            e = errorcheck();

            if (e == 0) {

                sprintf(O, "%5i\n", replynum);
                cbm_write(2, O, strlen(O));

                sprintf(O, "\223Msg #%u\n\n", S.NUMMESGS);
                cbm_write(2, O, strlen(O));

                sprintf(O, "From:%s\n", U.USERNAME);
                cbm_write(2, O, strlen(O));

                getdate();

                sprintf(O, "Date:%s\n", DATE);
                cbm_write(2, O, strlen(O));

                sprintf(O, "Subj:%s\n", I);
                cbm_write(2, O, strlen(O));

                clear(O);
                O[0] = 158;
                for (i=1; i<22; i++) {
                    O[i] = 163;
                }
                O[i] = '\0';
                strcat(O, "\005\n");
                cbm_write(2, O, strlen(O));

            }

            e = errorcheck();

            cbm_close(2);
            cbm_close(15);

            if (e == 0) {
                hi = S.NUMMESGS / 256;
                lo = S.NUMMESGS - (hi * 256);
                POKE(BS_MSG_HI, hi);
                POKE(BS_MSG_LO, lo);
                if (replynum > 0) {
                    POKE(BS_REPLY, 1);
                } else {
                    POKE(BS_REPLY, 0);
                }

                bootstrap("EDITOR");
            } else {
                pause(FALSE);
            }
        }

    } else {
        askforhelp();
    }

}

void the_wall() {

    char ch, e;
    int i;
    char tag[21], buf[32];

    print("\223");
    showfile("the wall h", 8, FALSE, 0);
    showfile("the wall", 8, FALSE, 0);

    if (U.SECURITY >= 2) {

        print("\nSign The Wall (Y/N)?");
        ch = get_command();

        if (ch == 'Y') {

            print("\n\nYou have 2 lines. GO!\n\n");

            input('A', 0, 21, FALSE);

            if (strlen(I) > 0) {

                strcpy(O, I);
                strcat(O, "\n");
                input('A', 0, 21, FALSE);
                strcat(I, "\n");

                ch = confirm();

                if (ch == TRUE) {

                    print("\nSaving...");

                    cbm_open(15, 8, 15, "");
                    cbm_open(2, 8, 2, "@0:wtemp,s,w");
                    cbm_open(3, 8, 3, "the wall,s,r");

                    e = errorcheck();

                    if (e == 0) {
                        strcpy(tag, "From:\022");
                        strcat(tag, U.USERNAME);
                        strcat(tag, "\n");
                        cbm_write(2, tag, strlen(tag));
                        cbm_write(2, O, strlen(O));
                        if (strlen(I) > 0) {
                            cbm_write(2, I, strlen(I));
                        }
                        clear(I);
                        strcpy(I, "\005\222");
                        for (i = 0; i < 21; i++) {
                            strcat(I, "\300");
                        }
                        strcat(I, "\n");
                        cbm_write(2, I, strlen(I));
                        clear(I);
                        clear(O);

                        clear(buf);
                        while (cbm_read(3, buf, 32) > 0) {
                            cbm_write(2, buf, strlen(buf));
                            clear(buf);
                        }

                    }

                    cbm_close(3);
                    cbm_close(2);
                    cbm_write(15, "s0:the wall", 11);
                    cbm_write(15, "r0:the wall=wtemp", 17);
                    cbm_close(15);

                    print("DONE!\n\n");

                }

            }

        }
    }
}

void hangup() {

    clearbuffer();

    sleep();
    POKE(RS,151);
    sleep();
    POKE(RS,PEEK(RS)&32);
    sleep();

    memset(&U, 0, sizeof(U));
    LOCALMODE = FALSE;

    close_comms();
    open_comms();

    /* reset global variables */
    LOCALMODE = FALSE;
    LOGGINGON = TRUE;
    ONLINE = FALSE;
    RELOAD = FALSE;

    POKE(BS_LOCALMODE,0);
    POKE(BS_ID,0);
    POKE(BS_MODULE,0);

}

void close_comms() {

    cbm_close(5);

}
