#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sm_common.h"
#include "sm_global.h"
#include "rsa.h"

/* local variables - move to rsa.h? */
char dronesDisabled = FALSE;

void main() {

    LOCALMODE = PEEK(BS_LOCALMODE);
    U.ID = PEEK(BS_ID);

    /* enable running directly from load! */
    LOCALMODE = TRUE;
    U.ID = 1;
    POKE(BS_MODULE, 5);
    POKE(SC, 8);

    ONLINE = carrierdetect();

    if ((ONLINE == TRUE) && (PEEK(BS_MODULE) == 5)) {
        play_red_sector_a();
    }

    /* re-enable for BBS connectivity! 
    bootstrap("GAMES");
    */

}

void play_red_sector_a() {

    char playing = TRUE;
    char dead = FALSE;
    char yn, st;
    char action[10];
    char subject[20];
    char paction[10];
    char psubject[20];
    signed char lastSpace = -1;
    char currentRoom = 0;
    char inventoryCount = 0;
    char mirrorUses = 0;
    char flashlightON = FALSE;
    char i, j, f, k, len, dir;
    char tries = 0;
    char *lowramfile = "@1:rsa low";
    char *highramfile = "@1:rsa high";

    /* UNCOMMNET FOR PRODUCTION
    showfile("rsa intro", FALSE);

    print("\n\nInstructions (Y/N)?");
    yn = get_command();

    if (yn == 'Y') {
        showfile("rsa help", TRUE);
    }
    */

    /* LOAD BINARY DATA FILES */
    print("\223\005Loading data...");

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(lowramfile);
    cbm_k_load(0, 1);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(highramfile);
    cbm_k_load(0, 1);

    /* START OF GAME */
    showstr(strd[STRD_RSA], D, 2);                      // >>> RED SECTOR A <<<

    showRoom(currentRoom, flashlightON);                // start in Cryo Chamber

    /* COMMAND PARSER */
    do {

        showstr(stra[STRA_GAME_PROMPT], A, 0);

        input('A', 0, 32);
        trim(I);

        /*
         * COMMANDS:
         *         - go/run (north, south, east, west)
         *         - take/drop/use {item}
         *         - look
         *         - inventory
         *         - help
         */

        /* backup action and subject for unique situations */
        if (strlen(action) > 0) {
            strcpy(paction, action);
        }
        if (strlen(subject) > 0) {
            strcpy(psubject, subject);
        }

        /* parse command into action and subject */
        parse_input(I, action, subject);
        trim(strlower(action));
        trim(strlower(subject));

        if (strcmp(action, "look") == 0) {

            /* LOOK */

            if ((strcmp(subject, "") == 0) || (strcmp(subject, "around") == 0)) {

                showRoom(currentRoom, flashlightON);

            } else if ((currentRoom == BARRACKS) && (flashlightON == FALSE)) {

                showstr(strc[STRC_NOTHING], C, 0);

            } else {

                dir = dirLookup(subject);

                if (dir == NORTH || dir == SOUTH || dir == EAST || dir == WEST) {
                    showstr(strb[STRB_YOU_SEE], B, 1);
                    /* EDGE CASES */
                    f = FALSE;
                    if (currentRoom == CRYOCHAMBER) {

                        /* West wall: poster */
                        if (dir == WEST && items[POSTER].room != CRYOCHAMBER) {
                            showstr(strd[STRD_CRYO_POSTER_GONE], D, 1);
                            f = TRUE;
                        }

                        /* East wall: mirror */
                        if (dir == EAST && items[MIRROR].room != CRYOCHAMBER) {
                            showstr(strd[STRD_CRYO_MIRROR_GONE], D, 1);
                            f = TRUE;
                        }
                    }

                    /* Default: normal directional description */
                    if (f == FALSE) {
                        showstr_by_type(rooms[currentRoom].dirdesc[dir][0], rooms[currentRoom].dirdesc[dir][1], 1);
                    }

                } else {
                    showstr(stra[STRA_GAME_HUH], A, 2);         // huh?
                }
            }
        } else if (strcmp(action, "go") == 0) {
            
            /* GO */

            dir = dirLookup(subject);

            if (dir == NORTH || dir == SOUTH || dir == EAST || dir == WEST) {
                if (rooms[currentRoom].doors[dir] != NO_EXIT) {
                    if (rooms[rooms[currentRoom].doors[dir]].locked == FALSE) {
                        currentRoom = rooms[currentRoom].doors[dir];
                        showRoom(currentRoom, flashlightON);
                    } else {
                        showstr(strc[STRC_DOOR_LOCKED], C, 0);
                    }
                } else {
                    showstr(strc[STRC_CANT_GO_THAT_WAY], C, 0);
                }
            } else {
                showstr(strc[STRC_CANT_GO_THAT_WAY], C, 0);
            }
        } else if (strcmp(action, "take") == 0) {

            /* TAKE */

            if (inventoryCount < 5) {
                f = FALSE;
                /* set subject to only first four letters */
                for (i = 4; i < strlen(subject); i++) {
                    subject[i] = 0;
                }
                for (i = 0; i < MAXITEMS; i++) {
                    if (strcmp(stra[items[i].abbrev], subject) == 0) {
                        if ((items[i].room != TAKEN) && (items[i].type == MOVABLE)) {
                            items[i].room = TAKEN;
                            inventoryCount++;
                            showstr(strb[STRB_TAKEN], B, 0);
                            f = TRUE;
                            break;
                        } else {
                            showstr(strc[STRC_CANT_TAKE_THAT], C, 0);
                        }
                    }
                }
                if (f == FALSE) {
                    showstr(strb[STRB_NOTHING_TO_TAKE], B, 1);
                }
            } else {
                showstr(strc[STRC_CANT_CARRY_ANYMORE], C, 0);
            }
        } else if (strcmp(action, "drop") == 0) {

            /* DROP */

            if (inventoryCount > 0) {
                f = FALSE;
                /* set subject to only first four letters */
                for (i = 4; i < strlen(subject); i++) {
                    subject[i] = 0;
                }
                for (i = 0; i < MAXITEMS; i++) {
                    if (strcmp(stra[items[i].abbrev], subject) == 0) {
                        if (items[i].room == TAKEN) {
                            items[i].room = currentRoom;
                            inventoryCount--;
                            showstr(strb[STRB_DROPPED], B, 0);
                            f = TRUE;
                            break;
                        } else {
                            showstr(strc[STRB_NOTHING_TO_DROP], C, 0);
                        }
                    }
                }
                if (f == FALSE) {
                    showstr(strb[STRB_NOTHING_TO_DROP], B, 1);
                }
            } else {
                showstr(strc[STRB_NOTHING_TO_DROP], C, 0);
            }

        } else if (strcmp(action, "use") == 0) {

            /* USE */

            f = FALSE;

            if (((strncmp(subject, "term", 4) == 0) || (strncmp(subject, "comp", 4) == 0)) && (currentRoom == SERVERROOM)) {
                
                if (tries < 3) {

                    showstr(strc[STRC_ENTER_PASSWORD], C, 0);

                    input('P', 0, 12);
                    trim(I);

                    if (check(I) == TRUE) {

                        /* SUCCESS! */
                        showstr(strd[STRD_DRONES_DISABLED], D, 0);
                        dronesDisabled = TRUE;
                        f = TRUE;

                    } else {

                        /* INCORRECT */
                        showstr(strb[STRB_INCORRECT], B, 0);
                        tries++;
                        f = TRUE;

                    }

                } else {

                    showstr(strb[STRB_TERMINAL_LOCKED], B, 1);
                    f = TRUE;

                }

            }


            if (strcmp(subject, "keys") == 0 && items[KEYS].room == TAKEN && strlen(paction) > 0 && strlen(psubject) > 0) {

                if (strcmp(paction, "go") == 0) {

                    dir = dirLookup(psubject);

                    if (dir == NORTH || dir == SOUTH || dir == EAST || dir == WEST) {

                        if (rooms[rooms[currentRoom].doors[dir]].locked == TRUE) {

                            rooms[rooms[currentRoom].doors[dir]].locked = FALSE;
                            showstr(strc[STRC_YOU_UNLOCKED_DOOR], C, 0);
                            f = TRUE;

                        } 
                    }
                }
            }

            if (f == FALSE) {
                showstr(strc[STRC_THAT_DIDNT_WORK], C, 0);
            }

        } else if (strncmp(action, "inventory", 3) == 0){

            /* INVENTORY */

            showstr(strb[STRB_YOUVE_GOT], B, 1);
            if (inventoryCount > 0) {
                for (i = 0; i < MAXITEMS; i++) {
                    if (items[i].room == TAKEN) {
                        putch(DASH);
                        showstr_by_type(items[i].name[STR_T], items[i].name[STR_N], 1);
                    }
                }
            } else {
                putch(DASH);
                showstr(strb[STRB_NOTHING], B, 1);
            }
        } else if (strcmp(action, "help") == 0) {

            /* HELP */

            showfile("rsa help", TRUE);

        } else if (strcmp(action, "quit") == 0) {

            /* QUIT */

            print("\n\nAre you sure? (Y/N)?");
            yn = get_command();

            if (yn == 'Y') {
                    playing = FALSE;
            }

        } else {
            showstr(stra[STRA_GAME_HUH], A, 2);         // huh?
        }

    } while (playing == TRUE);

    /*
    if (dead == TRUE) {
        print(str[34]); print(str[35]);  // Ah, you gave it your best shot, right?
    } else {
        print(str[36]); print(str[37]);  // CONGRATS! You made it!
    }
    */

    gpause();
    print("\n");

}

void showRoom(char roomNum, char lightMode) {

    char i;
    char f = FALSE;

    // show room name and description
    showstr_by_type(rooms[roomNum].name[STR_T], rooms[roomNum].name[STR_N], 2);
    showstr_by_type(rooms[roomNum].description[STR_T], rooms[roomNum].description[STR_N], 2);

    if ((inHallway(roomNum) == TRUE) && (dronesDisabled == FALSE)) {
        showstr(strc[STRC_BUZZING], C, 1);
    }

    showstr(strb[STRB_ITEMS_HERE], B, 0);

    if ((roomNum == BARRACKS) && (lightMode == FALSE)) {
        // do nothing!
    } else {
        // show room items
        for (i = 0; i < MAXITEMS; i++) {
            if (items[i].room == roomNum) {
                // show this item!
                putch(DASH);
                showstr_by_type(items[i].name[0], items[i].name[1], 1);
                f = TRUE;
            }
        }
    }

    if (f == FALSE) {
        print("-nothing\n");
    }
}

void showstr_by_type(char type, char index, char numCR) {

    const char *src;
    char len;
    char i;

    switch (type) {
        case A:
            src = stra[index];
            break;

        case B:
            src = strb[index];
            break;

        case C:
            src = strc[index];
            break;

        case D:
            src = strd[index];
            break;

        default:
            return;   // invalid type, do nothing
    }

    showstr(src, type, numCR);

}


void showstr(const char *src, char len, char numCR) {

    char i;

    // print designated string - end early if zero terminated
    for (i = 0; i < len; i++) {
        if (src[i] == 0) break;
        putch(src[i]);
    }

    // print the number of carriage returns in numCR
    if (numCR > 0) {
        for (i = 0; i < numCR; i++) {
            putch(RETURN);
        }
    }

}

int dirLookup(char* direction) {

    int retDir = -1;

    if (strcmp(direction, "north") == 0) {
        retDir = NORTH;
    } else if (strcmp(direction, "south") == 0) {
        retDir = SOUTH;
    } else if (strcmp(direction, "east") == 0) {
        retDir = EAST;
    } else if (strcmp(direction, "west") == 0) {
        retDir = WEST;
    }

    return retDir;

}

char inHallway(char roomNum) {

    char Result = FALSE;

    if ((roomNum == HALLWAY1) || (roomNum == HALLWAY2) || (roomNum == HALLWAY3) || (roomNum == HALLWAY4)) {
        Result = TRUE;
    }

    return Result;

}

void parse_input(const char *input, char *action, char *subject) {

    /* NOTE: This function written by Microsoft Copilot */

    int a, i, s;
    int len = strlen(input);

    // Clear outputs
    action[0] = 0;
    subject[0] = 0;
    i = 0;

    // Skip leading spaces
    while (i < len && input[i] == ' ')
        i++;

    // If empty input, done
    if (i >= len)
        return;

    // Copy action until space or end
    a = 0;
    while (i < len && input[i] != ' ' && a < 39) {
        action[a++] = tolower(input[i++]);
    }
    action[a] = 0;

    // Skip spaces between action and subject
    while (i < len && input[i] == ' ')
        i++;

    // If no subject, done
    if (i >= len)
        return;

    // Copy subject until next space or end
    s = 0;
    while (i < len && input[i] != ' ' && s < 39) {
        subject[s++] = tolower(input[i++]);
    }
    subject[s] = 0;
}

char check(const char *input) {

    char i, x;
    char result = FALSE;

    static const unsigned char S[11] = {
        117, 35, 21, 38, 15, 114, 30, 34, 36, 38, 0
    };

    char buf[11];

    for (i = 0; i < 10; i++) {
        x = S[i];
        x -= 7;
        x ^= 0x5A;
        buf[i] = (char)x;
    }
    buf[10] = '\0';

    if (strcmp(buf, input) == 0) {
        result = TRUE;
    }

    return(result);

}