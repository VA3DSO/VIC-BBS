#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"
#include "uutils.h"

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

            pause(FALSE);

            showfile("newuser2", TRUE, 0);
        }

    } else {
        print("\n\nUSER LOG FULL!\n");
    }


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

void genuserlist() {

    int users = 0;
    char id, un[13];

    open_userfile();
    cbm_open(3, 8, 3, "@0:userlog,s,w");

    for(id=1; id<255; id++) {
        gotouser(id);
        clear(un);
        cbm_read(1, un, 12);
        if (un[0] != 255) {
            users++;
            trim(un);
            clear(O);
            sprintf(O, "%-3i %s\n", id, un);
            cbm_write(3, O, strlen(O));
        }
    }

    cbm_close(3);
    close_userfile();
    S.NUMUSERS = users;
    savestats();

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
            pause(FALSE);
        }
    } else if (strlen(I) > 0) {
        print("\n\n\022MIN 3 CHAR REQ!\n");
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

    char sid, yn = FALSE;

    print("\223\005\022EDIT\222USER\n\n");

    if (sysop == TRUE) {
        sid = U.ID;
        sprintf(O, "Sysop ID: %i\n", sid); print(O);
        U.ID = id;
        sprintf(O, "User ID: %i\n", id); print(O);
        loaduser(U.ID);
        sprintf(O, "Loading ID: %i\n\n", U.ID); print(O);
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

        if (sysop == TRUE) {
            sprintf(O, "Saving ID: %i\n", U.ID); print(O);
        }
        saveuser(U.ID);

        if (sysop == TRUE) {
            sprintf(O, "Updating UL...\n"); print(O);
        }
        genuserlist();

    }

    if (sysop == TRUE) {
        sprintf(O, "Reloading ID: %i\n\n", sid); print(O);
        loaduser(sid);
    }

    pause(FALSE);

}
