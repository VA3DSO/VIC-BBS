/*
 * WHERE I LEFT OFF:
 *
 * Okay, let's get rid of the "Items" list when showing a room. Instead, lets
 * tie the items to the NSEW descriptions. We'll have 1 item per direction.
 * So we'll need two descriptions for each direction - one with the item
 * present, and one with it taken. If we randomly drop an item in a room,
 * perhaps it can be near the doorway direction. Will have to figure out how
 * to handle dropping items in the hallway or elevator.
 *
 * Next, let's shorten the commands to just the first char: g for go, u for
 * use, t for take, etc. Then the first four chars of the last word for
 * the object (assuming they are all unique in that way). That will save
 * bytes in the program, and let "pros" navigate through the game faster.
 *
 * Need a mirror counter and what to do when it disintegrates. The rest of the
 * game seems pretty straight forward. Double check that you have all the
 * finishing scenarios figured out.
 *
 *
 */

#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sm_common.h"
#include "sm_global.h"

/* RED SECTOR A */
#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3

#define IMMOVABLE 0
#define MOVABLE   1
#define DONTSHOW  2

#define MAXROOMS 17
#define MAXITEMS 30

#define KEY         0
#define ACCESSCARD  2
#define MIRROR      4
#define FLASHLIGHT  5

#define CRYOCHAMBER 0
#define SERVERROOM  2
#define BARRACKS    9
#define HALLWAY1    1
#define HALLWAY2    4
#define HALLWAY3    7
#define HALLWAY4    10
#define ELEVATOR    12

typedef struct {
    char name[32];
    char abbrev[16];
    int taken;
    int type;
} Item;

typedef struct {
    char name[32];
    int locked;
    char description[64];
    int doors[4];                 // room doors [NSEW] (-1 if no exit, or # of room it leads to)
    char dirdesc[4][64];          // description in each dir [NSEW]
    int itemIndex[4];             // index of item in room (-1 if none, otherwise item number)
} Room;

void play_red_sector_a();
void showRoom(int, char);
int dirLookup(char*);
char inHallway(int);

#pragma bss-name ("BDATA")
Room rooms[MAXROOMS];
Item items[MAXITEMS];
char out[64];
#pragma bss-name ("LDATA")
char str[95][32];
#pragma bss-name ("BSS")   // switch back to default
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

    // -- bootstrap("GAMES");

}

void play_red_sector_a() {

    char playing = TRUE;
    char dead = FALSE;
    char yn, st;
    char action[10];
    char subject[20];
    char paction[10];
    char psubject[20];
    int currentRoom = 0;
    int inventoryCount = 0;
    int mirrorUses = 0;
    int droneAware = 0;
    char flashlightON = FALSE;
    int i, j, f, len, dir;
    int tries = 0;
    char *datafile = "1:rsa data";
    char *strfile = "1:rsa strings";

    /* showfile("rsa intro", FALSE);

    print("\n\nInstructions (Y/N)?");
    yn = get_command();

    if (yn == 'Y') {
        showfile("rsa inst", TRUE);
    }

    */

    print("\223\005Loading data...");

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(datafile);
    cbm_k_load(0, 1);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(strfile);
    cbm_k_load(0, 1);

    print(str[0]); print(str[1]);  // >>> RED SECTOR A <<<

    showRoom(currentRoom, flashlightON);

    do {

        print(str[2]); // >
        input(0, 32);
        trim(I);

        /* process commands here */
        /*
         * COMMANDS:
         *         - go/run (north, south, east, west)
         *         - take/drop/use {item}
         *         - look
         *         - inventory
         *         - help
         */

        /* parse command into action and subject */
        if (strlen(action) > 0) {
            strcpy(paction, action);
        }
        clear(action);
        i = 0;
        while((I[i] != 13) && (I[i] != 0) && (I[i] != 32)) {
            i++;
        }
        strncpy(action, &I[0], i);
        trim(strlower(action));

        len = strlen(I);
        while ((len >= 0) && I[len] != 32) {
            len--;
        }

        if (strlen(subject) > 0) {
            strcpy(psubject, subject);
        }
        clear(subject);
        j = strlen(I) - len;
        strncpy(subject, &I[len], j);
        trim(strlower(subject));

        if (strcmp(action, "look") == 0) {
            if ((strcmp(subject, "") == 0) || (strcmp(subject, "around") == 0))  {
                showRoom(currentRoom, flashlightON);
            } else if ((currentRoom == BARRACKS) && (flashlightON == FALSE)) {
                print("\nYou see:\nnothing!\n");
            } else {
                dir = dirLookup(subject);
                if (dir > -1) {
                    print(str[3]);  // You see:
                    print(rooms[currentRoom].dirdesc[dir]);
                    print(str[4]);  // \n\n
                } else {
                    print(str[5]);  // huh?
                }
            }
        } else if (strcmp(action, "go") == 0) {
            dir = dirLookup(subject);
            if (dir > -1) {
                if (rooms[currentRoom].doors[dir] > -1) {
                    if (rooms[rooms[currentRoom].doors[dir]].locked == FALSE) {
                        currentRoom = rooms[currentRoom].doors[dir];
                        showRoom(currentRoom, flashlightON);
                    } else {
                        if (items[KEY].taken == TRUE) {
                            rooms[rooms[currentRoom].doors[dir]].locked = FALSE;
                            print(str[6]);  // You've unlocked the door!
                            currentRoom = rooms[currentRoom].doors[dir];
                            showRoom(currentRoom, flashlightON);
                        } else {
                            print(str[7]);  // The door is locked.
                        }
                    }
                } else {
                    print(str[8]);  // You can't go that way.
                }
            } else {
                print(str[9]);  // You can't go that way!
            }
        } else if (strcmp(action, "take") == 0) {
            if (inventoryCount < 5) {
                f = FALSE;
                for (i = 0; i < 4; i++) {
                    if (strcmp(items[rooms[currentRoom].itemIndex[i]].abbrev, subject) == 0) {
                        if ((items[rooms[currentRoom].itemIndex[i]].taken == FALSE) && (items[rooms[currentRoom].itemIndex[i]].type == MOVABLE)) {
                            items[rooms[currentRoom].itemIndex[i]].taken = TRUE;
                            rooms[currentRoom].itemIndex[i] = -1;
                            inventoryCount++;
                            print(str[10]); // Taken!
                            f = TRUE;
                            break;
                        } else {
                            print(str[11]); // You can't take that.
                            f = TRUE;
                        }
                    }
                }
                if (f == FALSE) {
                    print(str[12]); // Nothing to take!
                }
            } else {
                print(str[13]);  // You can't carry any more!
            }
        } else if (strcmp(action, "drop") == 0) {
            f = FALSE;
            for (i = 0; i < MAXITEMS; i++) {
                if (strcmp(items[i].abbrev, subject) == 0) {
                    for (j = 0; j < 4; j++) {
                        if (rooms[currentRoom].itemIndex[j] == -1) {
                            rooms[currentRoom].itemIndex[j] = i;
                            items[i].taken = FALSE;
                            print(str[14]);  // Dropped.
                            inventoryCount--;
                            f = TRUE;
                            break;
                        }
                    }
                    if (j == 4) {
                        print(str[15]);  // You can't drop that here.
                    }
                    if (f == TRUE) {
                        break;
                    }
                }
            }
            if (f == FALSE) {
                print(str[16]);  // You can't drop that.
            }
        } else if (strcmp(action, "use") == 0) {
            if ((strcmp(subject, "terminal") == 0) && (currentRoom == SERVERROOM)) {
                if (tries < 3) {
                    print(str[17]);  // ENTER PASSWORD:
                    input(0, 12);
                    trim(I);
                    if (strcmp(I, "4fter1mage") == 0) {
                        print(str[18]); print(str[19]);  // SUCCESS! Drones have been DISABLED!
                        dronesDisabled = TRUE;
                    } else {
                        print(str[20]);  // INCORRECT.
                        tries++;
                    }
                } else {
                    print(str[21]);  // TERMINAL LOCKED.
                }
            } else if ((strcmp(subject, "flashlight") == 0) && (items[FLASHLIGHT].taken == TRUE)) {
                if (flashlightON == FALSE) {
                    flashlightON = TRUE;
                    print(str[38]);  // Flashlight ON!
                } else {
                    flashlightON = FALSE;
                    print(str[39]);  // Flashlight OFF!
                }
            } else if ((strcmp(subject, "card") == 0) && (items[ACCESSCARD].taken == TRUE)) {
                if (currentRoom == ELEVATOR){
                    if ((strcmp(paction, "run") == 0) || (strcmp(paction, "go") == 0)) {
                        if (strcmp(psubject, "east") == 0) {
                            /* we came from hallway 10 -> take us to yard 13 */
                            currentRoom = 13;
                        } else {
                            currentRoom = 10;
                        }
                        print(str[41]); print(str[42]); print(str[43]); print(str[44]); // Elevator ride!
                        showRoom(currentRoom, flashlightON);
                    } else {
                        print(str[40]);  // That did nothing.
                    }
                } else {
                    print(str[40]);  // That did nothing.
                }
            } else if ((strcmp(subject, "mirror") == 0) && (items[MIRROR].taken == TRUE)) {
                if  ((inHallway(currentRoom) == TRUE) && (droneAware == 3)) {
                    print("Deflected!\n");
                    droneAware = 1;
                } else {
                    print("Don't you look pretty!\n");
                }
            } else {
                print(str[22]);  // You can't use that.
            }
        } else if (strcmp(action, "inventory") == 0) {
            if (inventoryCount > 0) {
                for (i = 0; i < MAXITEMS; i++) {
                    if (items[i].taken == TRUE) {
                        sprintf(out, "-%s\n", items[i].name); print(out);
                    }
                }
            } else {
                print(str[23]);  // You don't have anything.
            }
        } else if (strcmp(action, "quit") == 0) {
            print(str[24]);  // Leaving!
            playing = FALSE;
            dead = TRUE;
        } else if (strcmp(action, "help") == 0) {
            showfile("rsa help", FALSE);
        } else {
            print(str[25]);  // Huh?
        }

        st = carrierdetect();

        if (st == FALSE) {
            playing = FALSE;
            dead = TRUE;
        }

        if ((playing == TRUE) && (dronesDisabled == FALSE) && (inHallway(currentRoom) == TRUE)) {
            droneAware++;
            switch (droneAware) {
                case 1:
                    print(str[26]); print(str[27]);  // The attack drones have spotted you!
                    break;
                case 2:
                    print(str[28]); print(str[29]);  // The attack drones are taking aim!
                    break;
                case 3:
                    print(str[30]); print(str[31]);  // The attack drones are firing!
                    break;
                case 4:
                    print(str[32]); print(str[33]);  // The drones find their mark! You are DEAD.
                    dead = TRUE;
                    playing = FALSE;
                    break;
            }
        } else {
            droneAware = 0;
        }

    } while (playing == TRUE);

    if (dead == TRUE) {
        print(str[34]); print(str[35]);  // Ah, you gave it your best shot, right?
    } else {
        print(str[36]); print(str[37]);  // CONGRATS! You made it!
    }

    gpause();
    print("\n");
}

void showRoom(int roomNum, char lightMode) {

    int i;
    int f = FALSE;

    sprintf(out, "\n\022%s\222\n\n%s", rooms[roomNum].name, rooms[roomNum].description); print(out);
    if ((roomNum == 1) && (dronesDisabled == FALSE)) {
        print(str[45]);
    } else {
        print("\n");
    }
    print("\n\nItems here:\n");
    if ((roomNum == BARRACKS) && (lightMode == FALSE)) {
        // do nothing!
    } else {
        for (i = 0; i < 4; i++) {
            if ((rooms[roomNum].itemIndex[i] != -1 && items[rooms[roomNum].itemIndex[i]].taken == FALSE)) {
                sprintf(out, "-%s\n", items[rooms[roomNum].itemIndex[i]].name); print(out);
                f = TRUE;
            }
        }
    }

    if (f == FALSE) {
        print("-nothing\n\n");
    } else {
        print("\n");
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

char inHallway(int roomNum) {

    char Result = FALSE;

    if ((roomNum == HALLWAY1) || (roomNum == HALLWAY2) || (roomNum == HALLWAY3) || (roomNum == HALLWAY4)) {
        Result = TRUE;
    }

    return Result;

}
