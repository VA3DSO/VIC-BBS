#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"
#include "global.h"
#include "uutils.h"
#include "futils.h"

/* local functions */
void sysop_menu();
void fix_stats();
void backup();
void restore();
void create(char);
void deleteuser(char);
void validateusers(void);
void dos_commands(void);

void main(void) {

    char id;


    if (PEEK(BS_MODULE) == 6) {

        LOCALMODE = PEEK(BS_LOCALMODE);

        ONLINE = carrierdetect();

        if (ONLINE == TRUE) {

            id = PEEK(BS_ID);

            if (id > 0) {
                loaduser(id);
            }

            loadstats();

            sysop_menu();

        }

        bootstrap("BBS");

    } else {
        print("\nLoad BBS instead.\n");
    }

}

void sysop_menu() {

    char ch, sysoping = TRUE;
    int i;

    if (U.SECURITY >= 5) {

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
                    if (i > 0) {
                        edituser((char)i, TRUE);
                    }
                    break;
                case '!':
                    validateusers();
                    break;
                case 'D':
                    /* delete user */
                    print("\n\nUser ID:");
                    input('A', 0, 3, FALSE);
                    i = atoi(I);
                    if ((i > 0) && (i < 255)) {
                        deleteuser((char)i);
                    }
                    break;
                case 'G':
                    print("\n\nGenerating user log...");
                    genuserlist();
                    print("\nDONE!\n");
                    break;
                case 'V':
                    print("\223\237\022USER\222\005LOG\n\n");
                    showfile("userlog", FALSE, 0);
                    print("\n\n");
                    pause(FALSE);
                    break;
                case 'F':
                    /* file editor */
                    POKE(BS_MSG_HI, 0);
                    POKE(BS_MSG_LO, 0);
                    bootstrap("EDITOR");
                    break;
                case '>':
                    /* DOS commands! */
                    dos_commands();
                    break;
                case 'B':
                    /* backup user and file logs */
                    backup();
                    break;
                case 'R':
                    /* restore user or file log */
                    restore();
                    break;
                case 'S':
                    /* show/fix stats file */
                    fix_stats();
                    break;
                case 'C':
                    /* create user log DANGER */
                    create(FALSE);
                    break;
                case '@':
                    /* DEBUG! */
                    debug();
                    break;
                case 'X':
                    sysoping = FALSE;
                    break;
                case 'H':
                case '?':
                    showfile("sysopmenu", FALSE, 0);
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

void fix_stats() {

    SYSTEM TEMP;
    char yn;

    print("\223\005\022FIX\222STATS\n\n");

    sprintf(O, "Last Caller\n>%s", S.LASTCALLER); print(O);
    strcpy(I, S.LASTCALLER);
    strcpy(TEMP.LASTCALLER, S.LASTCALLER);
    input('A', 0, 12, TRUE);
    if (strlen(I) > 0) {
        strcpy(TEMP.LASTCALLER, I);
    }

    sprintf(O, "# of Calls: %i", S.NUMCALLS); print(O);
    sprintf(I,"%i", S.NUMCALLS);
    TEMP.NUMCALLS = S.NUMCALLS;
    input('A', 0, 12, TRUE);
    if (strlen(I) > 0) {
        TEMP.NUMCALLS = atoi(I);
    }

    sprintf(O, "# of Users: %i", S.NUMUSERS); print(O);
    sprintf(I,"%i", S.NUMUSERS);
    TEMP.NUMUSERS = S.NUMUSERS;
    input('A', 0, 12, TRUE);
    if (strlen(I) > 0) {
        TEMP.NUMUSERS = atoi(I);
    }

    sprintf(O, "# of Mesgs: %i", S.NUMMESGS); print(O);
    sprintf(I,"%i", S.NUMMESGS);
    TEMP.NUMMESGS = S.NUMMESGS;
    input('A', 0, 12, TRUE);
    if (strlen(I) > 0) {
        TEMP.NUMMESGS = atoi(I);
    }

    sprintf(O, "# of Bulls: %i", S.NUMBULLS); print(O);
    sprintf(I,"%i", S.NUMBULLS);
    TEMP.NUMBULLS = S.NUMBULLS;
    input('A', 0, 12, TRUE);
    if (strlen(I) > 0) {
        TEMP.NUMBULLS = atoi(I);
    }

    sprintf(O, "\nLast Caller\n>%s\n", TEMP.LASTCALLER); print(O);
    sprintf(O, "# of Calls: %i\n", TEMP.NUMCALLS); print(O);
    sprintf(O, "# of Users: %i\n", TEMP.NUMUSERS); print(O);
    sprintf(O, "# of Mesgs: %i\n", TEMP.NUMMESGS); print(O);
    sprintf(O, "# of Bulls: %i\n\n", TEMP.NUMBULLS); print(O);

    yn = confirm();

    if (yn == TRUE) {
        print("\nSaving... ");
        strcpy(S.LASTCALLER, TEMP.LASTCALLER);
        S.NUMCALLS = TEMP.NUMCALLS;
        S.NUMUSERS = TEMP.NUMUSERS;
        S.NUMMESGS = TEMP.NUMMESGS;
        S.NUMBULLS = TEMP.NUMBULLS;
        savestats();
        print("DONE!\n");
    } else {
        print("\nABORTED\n");
    }

}

void backup() {

    char drive, rec[76], id, e;

    print("\223\005\022USER\222BACKUP\n\n");
    print("Drive to use (8-10)?\n>");
    input('A', 0, 2, FALSE);

    if (strlen(I) > 0) {

        drive = atoi(I);

        if ((drive >= 8) && (drive <= 10)) {

            print("\nBacking up...\n");

            cbm_open(15, 8, 15, "");
            cbm_open(3, drive, 3, "@0:users-bak,s,w");

            if (open_userfile() == 0) {

                for (id = 1; id < 255; id++) {

                    sprintf(O, "%-3i", id); print(O);

                    if (gotouser(id) == 0) {

                        clear(rec);
                        cbm_read(1, rec, 76);

                        if (errorcheck() == 0) {
                            cbm_write(3, rec, strlen(rec));
                            e = errorcheck();
                        }
                    }

                    putch(20);
                    putch(20);
                    putch(20);

                }
            }

            close_userfile();
            cbm_close(3);
            cbm_close(15);

            print("\nDone!\n\n");
            pause(FALSE);
        }
    }

}

void restore() {

    char drive, rec[76], id, yn, uc, e;

    print("\223\005\022USER\222RESTORE\n\n");
    print("Drive to use (8-10)?\n>");
    input('A', 0, 2, FALSE);

    if (strlen(I) > 0) {

        drive = atoi(I);

        if ((drive >= 8) && (drive <= 10)) {

            print("\n\nTHIS IS DESTRUCTIVE!\n");
            yn = confirm();

            if (yn == TRUE) {

                print("\nRestoring...\n");

                uc = 0;

                cbm_open(15, 8, 15, "");
                cbm_open(3, drive, 3, "users-bak,s,r");

                if (open_userfile() == 0) {

                    for (id = 1; id < 255; id++) {

                        sprintf(O, "%-3i", id); print(O);

                        cbm_read(3, rec, 75);

                        if (gotouser(id) == 0) {

                            cbm_write(1, rec, 76);

                            e = errorcheck();

                        }

                        if (rec[0] != 255) {
                            uc++;
                        }

                        putch(20);
                        putch(20);
                        putch(20);

                    }
                } else {
                    print("\n\nIssue with USER\nfile? Run Create.\n\n");
                }

                close_userfile();
                cbm_close(3);
                cbm_close(15);

                sprintf(O, "\n\nRestored %i users.\n", uc); print(O);

                S.NUMUSERS = uc;
                savestats();
                print("\nMaking User List...\n");
                genuserlist();

                print("\nDone!\n\n");
                pause(FALSE);
            }
        }
    }
}

void create(char silent) {

    char yn, p[5], e, id;
    char filename[9] = "users,l,x";
    char rec[76] = "\377ULL        NULL      NULL USER           NULLWHERE      0----------------\n";
    filename[8] = 76;

    p[0] = 'p';
    p[1] = 98;
    p[3] = 0;
    p[4] = 1;

    if (silent == FALSE) {

        print("\223WARNING!!!\n\n");
        print("This will DESTROY the\nexisting user log!\n");
        yn = confirm();

    } else {

        yn = TRUE;

    }

    if (yn == TRUE) {

        if (silent == FALSE) {
            print("\nCreating User Log...\n");
        }

        cbm_open(15, 8, 15, "s0:users");
        cbm_close(15);

        cbm_open(15, 8, 15, "");
        cbm_open(1, 8, 2, filename);

        e = errorcheck();

        for (id = 1; id < 255; id++) {

            sprintf(O, "%-3i", id); print(O);

            p[2] = id;
            cbm_write(15, p, 5);
            cbm_write(1, rec, strlen(rec));

            putch(20);
            putch(20);
            putch(20);

        }

        cbm_close(1);

        if (silent == FALSE) {
            print("\nAdding SYSOP...\n");
        }

        strcpy(rec, "SYSOP       SYSOP     SYSOP               NOWHERE        6    0    0800101\n");
        cbm_open(1, 8, 2, filename);

        e = errorcheck();

        p[2] = 1;
        cbm_write(15, p, 5);
        cbm_write(1, rec, strlen(rec));

        e = errorcheck();

        cbm_close(1);
        cbm_close(15);

        S.NUMUSERS = 1;
        savestats();

        print("\nMaking User List...\n");

        genuserlist();

        if (silent == FALSE) {
            print("\nDONE!\n\n");
            pause(FALSE);
        }
    }

}

void deleteuser(char id) {

    char sid, yn = FALSE, p[5], e;
    char filename[9] = "users,l,x";
    char rec[76];
    filename[8] = 76;

    p[0] = 'p';
    p[1] = 98;
    p[2] = id;
    p[3] = 0;
    p[4] = 1;

    if (U.SECURITY == 6) {

        print("\223\005\022DELETE\222USER\n\n");
        sid = U.ID;
        U.ID = id;
        loaduser(U.ID);

        sprintf(O, "USERNAME:%s\n", U.USERNAME); print(O);
        sprintf(O, "PASSWORD:%s\n", U.PASSWORD); print(O);
        sprintf(O, "REALNAME:%s\n", U.REALNAME); print(O);
        sprintf(O, "    FROM:%s\n", U.FROM); print(O);
        sprintf(O, "SECURITY:%i\n", U.SECURITY); print(O);
        sprintf(O, "LST CALL:%s\n\n", U.LASTLOGON); print(O);

        yn = confirm();

        if (yn == TRUE) {

            print("Working...\n");

            cbm_open(15, 8, 15, "");
            cbm_open(1, 8, 2, filename);

            e = errorcheck();

            cbm_write(15, p, 5);
            cbm_read(1, rec, 76);

            e = errorcheck();

            rec[0] = 255;

            cbm_write(15, p, 5);
            cbm_write(1, rec, strlen(rec));
            cbm_write(15, p, 5);

            e = errorcheck();

            cbm_close(1);

            e = errorcheck();

            cbm_close(1);
            cbm_close(15);

            genuserlist();

            loaduser(sid);

            print("\nDELETED.\n");

        }


    } else {
        print("\n\nYOU CAN'T DO THAT!\n\n");
    }

}

void validateusers(void) {

    char id, rec[76], user[13], numcalls[6], lastcall[5], ch = 0;
    int c = 0;

    print("\n\nUser Validation:\n\n");
    print("\022USERNAME    \222 \022CLS\222 \022LAST\222\n");

    if (open_userfile() == 0) {

        for (id = 1; id < 255; id++) {

            if (gotouser(id) == 0) {

                clear(rec);
                cbm_read(1, rec, 76);

                if (rec[0] != 255) {

                    strncpy(user, rec, 12);
                    user[12] = '\0';

                    strncpy(numcalls, rec + 58, 5);
                    numcalls[5] = '\0';
                    trim(numcalls);

                    lastcall[0] = rec[68];
                    lastcall[1] = rec[69];
                    lastcall[2] = rec[72];
                    lastcall[3] = rec[73];
                    lastcall[4] = '\0';

                    clear(O);
                    sprintf(O, "%12s %-3s %s\n", user, numcalls, lastcall); print(O);

                    c++;
                    if (c >= 21) {
                        ch = pause(TRUE);
                        if (ch == 'Q') {
                            id = 255;
                            break;
                        }
                        c = 0;
                    }

                }
            }
        }

        pause(FALSE);
    }

    close_userfile();


}

void dos_commands(void) {

    char e, drive = 8, temp[3], partition;
    int i;

    char dossing = TRUE;

    print("\223\005\022DOS\222SHELL\n\n");

    print("Press ? for help!");

    do {

        e = 0;

        print("\n\n>");
        input('A', 0, 32, FALSE);

        if (strlen(I) == 0) {
            dossing = FALSE;
        } else if (I[0] == '$') {
            print("\nPart? (0-9) >");
            partition = getch(); putch(partition);
            directory(drive, partition);
        } else if (I[0] == '@') {
            e = errorcheck();
            if (e == 0) {
                print("\n\02200 OK\222\n");
            }
        } else if (I[0] == '#') {
            clear(temp);
            temp[0] = I[1];
            if ((I[2] == '0') || (I[2] == '1' )) {
                temp[1] = I[2];
            } else {
                temp[1] = '\0';
            }
            temp[2] = '\0';
            i = atoi(temp);
            if ((i >= 8) && (i <= 11)) {
                drive = (char)i;
                sprintf(O, "\nDrive = %i\n", drive); print(O);
            }
        } else if (I[0] == '?') {
            showfile("dosshell", FALSE, 0);
        } else {
            cbm_open(15, drive, 15, I);
            e = errorcheck();
            if (e == 0) {
                print("\n\02200 OK\222\n");
            }
            cbm_close(15);
        }

    } while (dossing == TRUE);
}
