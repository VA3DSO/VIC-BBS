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

/* local functions */
void sysop_menu();
void fix_stats();
void backup();
void restore();
void create(char);

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
                    POKE(BS_MSG_HI, 0);
                    POKE(BS_MSG_LO, 0);
                    bootstrap("EDITOR");
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
            pause();
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
                pause();
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
            pause();
        }
    }

}
