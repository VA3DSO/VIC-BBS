#pragma charmap(147, 147)
#pragma charmap(17, 17)
#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define SUCCESS 0
#define ERROR 1

#define S1 0x900A           /* 36874 - oscillator 1 (low) */
#define S2 0x900B           /* 36875 - oscillator 2 (med) */
#define S3 0x900C           /* 36876 - oscillator 3 (high) */
#define SN 0x900D           /* 36877 - noise generator */
#define SV 0x900E           /* 36878 - speaker volume (1-15) */
#define SC 0x900F           /* 36879 - screen border */
#define RS 0x9110           /* 37136 - RS-232 register */
#define RT 0x9800           /* 38912 - real time clock chip */
#define QR 0xA000           /* 40960 - start of free upper memory */
#define QZ 0xBFFF           /* 49151 - limit of free upper memory */

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

#define IMMOVABLE 0
#define MOVABLE   1
#define DONTSHOW  2

#define MAXROOMS 17
#define MAXITEMS 21

#define KEY 0

typedef struct {
    char name[20];
    char abbrev[5];
    int taken;
    int type;
} Item;

typedef struct {
    char name[32];
    int locked;
    char description[64];
    int doors[4];                 // room doors [NSEW] (-1 if no exit, or # of room it leads to)
    char dirdesc[4][64];              // description in each dir [NSEW]
    int itemIndex[4];             // index of item in room (-1 if none, otherwise item number)
} Room;

/* local functions */
void print(char*);

#pragma bss-name ("BDATA")
Room rooms[MAXROOMS]; //-- 567 bytes
Item items[MAXITEMS];
#pragma bss-name ("LDATA")
char str[95][32];
#pragma bss-name ("BSS")   // switch back to default

char O[40];

void main(void) {

    char *datafile = "@1:rsa data";
    char *strfile = "@1:rsa strings";

    POKE(36879UL, 8);
    print("\223\034>>> \022RED\222 \005\022SECTOR\222 \034\022A\222 <<<\005\n\n");
    print("Building worlds...\n");

    // --- Initialize Items ---

    strcpy(items[0].name, "master keys");
    strcpy(items[0].abbrev, "keys");
    items[0].taken = FALSE;
    items[0].type = MOVABLE;

    strcpy(items[1].name, "computer terminal");
    strcpy(items[1].abbrev, "terminal");
    items[1].taken = FALSE;
    items[1].type = IMMOVABLE;

    strcpy(items[2].name, "access card");
    strcpy(items[2].abbrev, "card");
    items[2].taken = FALSE;
    items[2].type = MOVABLE;

    strcpy(items[3].name, "medkit");
    strcpy(items[3].abbrev, "medkit");
    items[3].taken = FALSE;
    items[3].type = MOVABLE;

    strcpy(items[4].name, "mirror");
    strcpy(items[4].abbrev, "mirror");
    items[4].taken = FALSE;
    items[4].type = MOVABLE;

    strcpy(items[5].name, "flashlight");
    strcpy(items[5].abbrev, "flashlight");
    items[5].taken = FALSE;
    items[5].type = MOVABLE;

    strcpy(items[6].name, "rusty sink");
    strcpy(items[6].abbrev, "sink");
    items[6].taken = FALSE;
    items[6].type = IMMOVABLE;

    strcpy(items[7].name, "kevlar vest");
    strcpy(items[7].abbrev, "vest");
    items[7].taken = FALSE;
    items[7].type = MOVABLE;

    strcpy(items[8].name, "wire cutters");
    strcpy(items[8].abbrev, "cutters");
    items[8].taken = FALSE;
    items[8].type = MOVABLE;

    strcpy(items[9].name, "bucket");
    strcpy(items[9].abbrev, "bucket");
    items[9].taken = FALSE;
    items[9].type = MOVABLE;

    strcpy(items[10].name, "cookies");
    strcpy(items[10].abbrev, "cookies");
    items[10].taken = FALSE;
    items[10].type = MOVABLE;

    strcpy(items[11].name, "a small turtle");
    strcpy(items[11].abbrev, "turtle");
    items[11].taken = FALSE;
    items[11].type = MOVABLE;

    strcpy(items[12].name, "canybar");
    strcpy(items[12].abbrev, "candybar");
    items[12].taken = FALSE;
    items[12].type = MOVABLE;

    strcpy(items[13].name, "food tray");
    strcpy(items[13].abbrev, "tray");
    items[13].taken = FALSE;
    items[13].type = MOVABLE;

    strcpy(items[14].name, "coffee cup");
    strcpy(items[14].abbrev, "cup");
    items[14].taken = FALSE;
    items[14].type = MOVABLE;

    strcpy(items[15].name, "scalpel");
    strcpy(items[15].abbrev, "scalpel");
    items[15].taken = FALSE;
    items[15].type = MOVABLE;

    strcpy(items[16].name, "crutches");
    strcpy(items[16].abbrev, "crutches");
    items[16].taken = FALSE;
    items[16].type = MOVABLE;

    strcpy(items[17].name, "microscope slides");
    strcpy(items[17].abbrev, "slides");
    items[17].taken = FALSE;
    items[17].type = MOVABLE;

    strcpy(items[18].name, "beaker");
    strcpy(items[18].abbrev, "beaker");
    items[18].taken = FALSE;
    items[18].type = MOVABLE;

    strcpy(items[19].name, "network cable");
    strcpy(items[19].abbrev, "cable");
    items[19].taken = FALSE;
    items[19].type = MOVABLE;

    strcpy(items[20].name, "red stapler");
    strcpy(items[20].abbrev, "stapler");
    items[20].taken = FALSE;
    items[20].type = MOVABLE;

    strcpy(items[21].name, "dead cell phone");
    strcpy(items[21].abbrev, "phone");
    items[21].taken = FALSE;
    items[21].type = MOVABLE;

    strcpy(items[22].name, "boots");
    strcpy(items[22].abbrev, "boots");
    items[22].taken = FALSE;
    items[22].type = MOVABLE;

    strcpy(items[23].name, "golf club");
    strcpy(items[23].abbrev, "club");
    items[23].taken = FALSE;
    items[23].type = MOVABLE;

    strcpy(items[24].name, "helmet");
    strcpy(items[24].abbrev, "helmet");
    items[24].taken = FALSE;
    items[24].type = MOVABLE;

    strcpy(items[25].name, "unloaded gun");
    strcpy(items[25].abbrev, "gun");
    items[25].taken = FALSE;
    items[25].type = MOVABLE;

    strcpy(items[26].name, "pinup poster");
    strcpy(items[26].abbrev, "poster");
    items[26].taken = FALSE;
    items[26].type = MOVABLE;

    strcpy(items[27].name, "leather belt");
    strcpy(items[27].abbrev, "belt");
    items[27].taken = FALSE;
    items[27].type = MOVABLE;

    strcpy(items[28].name, "clipboard");
    strcpy(items[28].abbrev, "clipboard");
    items[28].taken = FALSE;
    items[28].type = MOVABLE;

    strcpy(items[29].name, "sharpie");
    strcpy(items[29].abbrev, "sharpie");
    items[29].taken = FALSE;
    items[29].type = MOVABLE;

    // --- Initialize Rooms ---

    strcpy(rooms[0].name, "Cryo Chamber");
    rooms[0].locked = FALSE;
    strcpy(rooms[0].description, "Dim light flickers\nacross ruined cryo\npods and stale air.");
    rooms[0].doors[NORTH] = -1; rooms[0].doors[SOUTH] = 1; rooms[0].doors[EAST] = -1; rooms[0].doors[WEST] = -1;
    strcpy(rooms[0].dirdesc[NORTH], "A row of broken cryo\nbeds.");
    strcpy(rooms[0].dirdesc[SOUTH], "A door leading to a\nhallway.");
    strcpy(rooms[0].dirdesc[EAST], "A dusty mirror hangs\non the wall.");
    strcpy(rooms[0].dirdesc[WEST], "A torn poster of\nFarrah Fawcett in a\nred swimsuit!");
    rooms[0].itemIndex[0] = 4;  // mirror
    rooms[0].itemIndex[1] = 26; // poster
    rooms[0].itemIndex[2] = -1; // no item
    rooms[0].itemIndex[3] = -1; // no item

    strcpy(rooms[1].name, "Hallway");
    rooms[1].locked = FALSE;
    strcpy(rooms[1].description, "There are rows of\nclosed doors. ");
    rooms[1].doors[NORTH] = 0; rooms[1].doors[SOUTH] = 2; rooms[1].doors[EAST] = 4; rooms[1].doors[WEST] = -1;
    strcpy(rooms[1].dirdesc[NORTH], "A door leading to\nthe cryo chamber.");
    strcpy(rooms[1].dirdesc[SOUTH], "A door leading to a\nserver room.");
    strcpy(rooms[1].dirdesc[EAST], "More hallway.");
    strcpy(rooms[1].dirdesc[WEST], "A window through\nwhich you can see a\ndystopian wasteland.");
    rooms[1].itemIndex[0] = -1; // no item
    rooms[1].itemIndex[1] = -1; // no item
    rooms[1].itemIndex[2] = -1; // no item
    rooms[1].itemIndex[3] = -1; // no item

    strcpy(rooms[2].name, "Server Room");
    rooms[2].locked = TRUE;
    strcpy(rooms[2].description, "You're in a room that\nreeks of ozone.");
    rooms[2].doors[NORTH] = 1; rooms[2].doors[SOUTH] = -1; rooms[2].doors[EAST] = -1; rooms[2].doors[WEST] = -1;
    strcpy(rooms[2].dirdesc[NORTH], "A door leading to a\nhallway.");
    strcpy(rooms[2].dirdesc[SOUTH], "Rows and rows of\ndishevelled server\nracks.");
    strcpy(rooms[2].dirdesc[EAST], "A whiteboard with\nsmeared network\ndiagrams on it.");
    strcpy(rooms[2].dirdesc[WEST], "The west wall is\ncracked, wires hang\nloose and sparking.");
    rooms[2].itemIndex[0] = 1;  // computer terminal
    rooms[2].itemIndex[1] = 19; // network cable
    rooms[2].itemIndex[2] = 20; // stapler
    rooms[2].itemIndex[3] = 21; // cell phone

    strcpy(rooms[3].name, "Laboratory");
    rooms[3].locked = TRUE;
    strcpy(rooms[3].description, "You enter a ruined\nlab, shattered glass\ncrunches underfoot.");
    rooms[3].doors[NORTH] = -1; rooms[3].doors[SOUTH] = 4; rooms[3].doors[EAST] = -1; rooms[3].doors[WEST] = -1;
    strcpy(rooms[3].dirdesc[NORTH], "Cracked specimen\ntanks ooze foul\nresidue into the\nfloor.");
    strcpy(rooms[3].dirdesc[SOUTH], "A door leading to a\nhallway.");
    strcpy(rooms[3].dirdesc[EAST], "A broken microscope\nsits on a dusty\ntable.");
    strcpy(rooms[3].dirdesc[WEST], "Broken shelves with\nglowing goo on the\nfloor.");
    rooms[3].itemIndex[0] = 17; // microscope slides
    rooms[3].itemIndex[1] = 2;  // access card
    rooms[3].itemIndex[2] = 18; // beaker
    rooms[3].itemIndex[3] = -1; // no item

    strcpy(rooms[4].name, "Hallway");
    rooms[4].locked = FALSE;
    strcpy(rooms[4].description, "There are rows of\nclosed doors. The\nfloor is covered in\nstains.");
    rooms[4].doors[NORTH] = 3; rooms[4].doors[SOUTH] = 5; rooms[4].doors[EAST] = 7; rooms[4].doors[WEST] = 1;
    strcpy(rooms[4].dirdesc[NORTH], "A door leading to\nthe lab.");
    strcpy(rooms[4].dirdesc[SOUTH], "A door leading to\nthe medical bay.");
    strcpy(rooms[4].dirdesc[EAST], "More hallway.");
    strcpy(rooms[4].dirdesc[WEST], "More hallway.");
    rooms[4].itemIndex[0] = 11; // turtle
    rooms[4].itemIndex[1] = -1; // no item
    rooms[4].itemIndex[2] = -1; // no item
    rooms[4].itemIndex[3] = -1; // no item

    strcpy(rooms[5].name, "Medical Bay");
    rooms[5].locked = TRUE;
    strcpy(rooms[5].description, "The room reeks of\nantiseptic, gear is\nscattered in\ndisarray.");
    rooms[5].doors[NORTH] = 4; rooms[5].doors[SOUTH] = -1; rooms[5].doors[EAST] = -1; rooms[5].doors[WEST] = -1;
    strcpy(rooms[5].dirdesc[NORTH], "A door leading to a\nhallway.");
    strcpy(rooms[5].dirdesc[SOUTH], "A shattered sink\ndrips steadily, rust\nstains streak the\ntiles.");
    strcpy(rooms[5].dirdesc[EAST], "Broken cabinets with\nthe contents spilt\nonto the counters.");
    strcpy(rooms[5].dirdesc[WEST], "An overturned gurney\nblocks the wall,\nwheels bent and\ntwisted.");
    rooms[5].itemIndex[0] = 3;  // medkit
    rooms[5].itemIndex[1] = 15; // scalpel
    rooms[5].itemIndex[2] = 5;  // flashlight
    rooms[5].itemIndex[3] = 16; // crutches

    strcpy(rooms[6].name, "Mess Hall");
    rooms[6].locked = FALSE;
    strcpy(rooms[6].description, "This room reeks of\nstale food and you\ncan hear dripping\nwater.");
    rooms[6].doors[NORTH] = -1; rooms[6].doors[SOUTH] = 7; rooms[6].doors[EAST] = -1; rooms[6].doors[WEST] = -1;
    strcpy(rooms[6].dirdesc[NORTH], "A counter lined with\ncookies, bananas\nand candybars.");
    strcpy(rooms[6].dirdesc[SOUTH], "A door leading to a\nhallway.");
    strcpy(rooms[6].dirdesc[EAST], "A rusty sink drips\nsteadily into a\nfilthy basin.");
    strcpy(rooms[6].dirdesc[WEST], "Tables littered with\ntrays, crums and\nempty cups.");
    rooms[6].itemIndex[0] = 10; // cookies
    rooms[6].itemIndex[1] = 12; // candybar
    rooms[6].itemIndex[2] = 6;  // rusty sink
    rooms[6].itemIndex[3] = 13; // food tray

    strcpy(rooms[7].name, "Hallway");
    rooms[7].locked = FALSE;
    strcpy(rooms[7].description, "There are rows of\nclosed doors. Blood\nsmeared on the walls.");
    rooms[7].doors[NORTH] = 6; rooms[7].doors[SOUTH] = 8; rooms[7].doors[EAST] = 10; rooms[7].doors[WEST] = 4;
    strcpy(rooms[7].dirdesc[NORTH], "A door leading to\nthe mess hall.");
    strcpy(rooms[7].dirdesc[SOUTH], "A door leading to\nthe storage closet.");
    strcpy(rooms[7].dirdesc[EAST], "More hallway.");
    strcpy(rooms[7].dirdesc[WEST], "More hallway.");
    rooms[7].itemIndex[0] = -1; // no item
    rooms[7].itemIndex[1] = -1; // no item
    rooms[7].itemIndex[2] = -1; // no item
    rooms[7].itemIndex[3] = -1; // no item

    strcpy(rooms[8].name, "Storage Closet");
    rooms[8].locked = FALSE;
    strcpy(rooms[8].description, "It's dusty in here\nand crammed with\nshelves and\nscattered junk.");
    rooms[8].doors[NORTH] = 7; rooms[8].doors[SOUTH] = 7; rooms[8].doors[EAST] = 11; rooms[8].doors[WEST] = -1;
    strcpy(rooms[8].dirdesc[NORTH], "A door leading to a\nhallway.");
    strcpy(rooms[8].dirdesc[SOUTH], "Stacked boxes sag,\nlabels faded and\npeeling away.");
    strcpy(rooms[8].dirdesc[EAST], "Hooks on wall, a\nring of master keys\ngleams faintly.");
    strcpy(rooms[8].dirdesc[WEST], "Broken mop and bucket\nrest in shadowed\ncorner");
    rooms[8].itemIndex[0] = 8;  // wire cutters
    rooms[8].itemIndex[1] = 9;  // bucket
    rooms[8].itemIndex[2] = 0;  // keys
    rooms[8].itemIndex[3] = 14; // coffee cup

    strcpy(rooms[9].name, "Barracks");
    rooms[9].locked = FALSE;
    strcpy(rooms[9].description, "The door closes\nbehind you. It is\ninky black in here!");
    rooms[9].doors[NORTH] = -1; rooms[9].doors[SOUTH] = 10; rooms[9].doors[EAST] = -1; rooms[9].doors[WEST] = -1;
    strcpy(rooms[9].dirdesc[NORTH], "Hastily scrawled on\nthe wall in red\nmarker: 4fter1mage.");
    strcpy(rooms[9].dirdesc[SOUTH], "A door leading to a\nhallway.");
    strcpy(rooms[9].dirdesc[EAST], "Rows of bunks, sheets\ntorn, with a pair of\nboots on the floor.");
    strcpy(rooms[9].dirdesc[WEST], "Lockers stand ajar\nrevealing uniforms\nand other gear.");
    rooms[9].itemIndex[0] = 22; // boots
    rooms[9].itemIndex[1] = 23; // golf club
    rooms[9].itemIndex[2] = 24; // helmet
    rooms[9].itemIndex[3] = 24; // unloaded gun

    strcpy(rooms[10].name, "Hallway");
    rooms[10].locked = FALSE;
    strcpy(rooms[10].description, "There are rows of\nclosed doors. It\nsmells like fear.");
    rooms[10].doors[NORTH] = 9; rooms[10].doors[SOUTH] = 11; rooms[10].doors[EAST] = 12; rooms[10].doors[WEST] = 7;
    strcpy(rooms[10].dirdesc[NORTH], "A door leading to\nthe barracks.");
    strcpy(rooms[10].dirdesc[SOUTH], "A door leading to\nthe quartermaster.");
    strcpy(rooms[10].dirdesc[EAST], "An elevator!");
    strcpy(rooms[10].dirdesc[WEST], "More hallway.");
    rooms[10].itemIndex[0] = -1; // no item
    rooms[10].itemIndex[1] = -1; // no item
    rooms[10].itemIndex[2] = -1; // no item
    rooms[10].itemIndex[3] = -1; // no item

    strcpy(rooms[11].name, "Quartermaster");
    rooms[11].locked = FALSE;
    strcpy(rooms[11].description, "The office is stacked\nwith gear and\nsupplies");
    rooms[11].doors[NORTH] = 10; rooms[11].doors[SOUTH] = -1; rooms[11].doors[EAST] = -1; rooms[11].doors[WEST] = 8;
    strcpy(rooms[11].dirdesc[NORTH], "A door leading to a\nhallway.");
    strcpy(rooms[11].dirdesc[SOUTH], "An open cabinet with\na kevlar vest that\nhangs within reach.");
    strcpy(rooms[11].dirdesc[EAST], "Shelves lined with\nhelmets, belts, and\nworn boots");
    strcpy(rooms[11].dirdesc[WEST], "Crates marked with\nfaded stencils,\ncontents unknown");
    rooms[11].itemIndex[0] = 27; // leather belt
    rooms[11].itemIndex[1] = 28; // clipboard
    rooms[11].itemIndex[2] = 7;  // kevlar vest
    rooms[11].itemIndex[3] = 29; // helmet

    strcpy(rooms[12].name, "Elevator");
    rooms[12].locked = FALSE;
    strcpy(rooms[12].description, "The access panel\nblinks expectantly.");
    rooms[12].doors[NORTH] = -1; rooms[12].doors[SOUTH] = -1; rooms[12].doors[EAST] = 13; rooms[12].doors[WEST] = 10;
    strcpy(rooms[12].dirdesc[NORTH], "A torn poster for\nRush's album 'Grace\nUnder Pressure'.");
    strcpy(rooms[12].dirdesc[SOUTH], "A blank wall.");
    strcpy(rooms[12].dirdesc[EAST], "An elevator door.");
    strcpy(rooms[12].dirdesc[WEST], "An elevator door.");
    rooms[12].itemIndex[0] = -1; // no item
    rooms[12].itemIndex[1] = -1; // no item
    rooms[12].itemIndex[2] = -1; // no item
    rooms[12].itemIndex[3] = -1; // no item

    strcpy(rooms[13].name, "Fenced Yard");
    rooms[13].locked = FALSE;
    strcpy(rooms[13].description, "You're in a grass\nyard with barbwired\nfences all around.");
    rooms[13].doors[NORTH] = 14; rooms[13].doors[SOUTH] = 16; rooms[13].doors[EAST] = 15; rooms[13].doors[WEST] = 12;
    strcpy(rooms[13].dirdesc[NORTH], "You see barbwired\nfence and freedom\nbeyond...");
    strcpy(rooms[13].dirdesc[SOUTH], "You see barbwired\nfence and freedom\nbeyond...");
    strcpy(rooms[13].dirdesc[EAST], "You see barbwired\nfence and freedom\nbeyond...");
    strcpy(rooms[13].dirdesc[WEST], "An elevator.");
    rooms[13].itemIndex[0] = -1; // no item
    rooms[13].itemIndex[1] = -1; // no item
    rooms[13].itemIndex[2] = -1; // no item
    rooms[13].itemIndex[3] = -1; // no item

    strcpy(rooms[14].name, "Barbwired Fence");
    rooms[14].locked = FALSE;
    strcpy(rooms[14].description, "You crouch down along\nthe fence just out of\nthe guard's view.");
    rooms[14].doors[NORTH] = -1; rooms[14].doors[SOUTH] = 13; rooms[14].doors[EAST] = -1; rooms[14].doors[WEST] = -1;
    strcpy(rooms[14].dirdesc[NORTH], "Fence.");
    strcpy(rooms[14].dirdesc[SOUTH], "A fenced yard.");
    strcpy(rooms[14].dirdesc[EAST], "Fence.");
    strcpy(rooms[14].dirdesc[WEST], "Fence.");
    rooms[14].itemIndex[0] = -1; // no item
    rooms[14].itemIndex[1] = -1; // no item
    rooms[14].itemIndex[2] = -1; // no item
    rooms[14].itemIndex[3] = -1; // no item

    strcpy(rooms[15].name, "Barbwired Fence");
    rooms[15].locked = FALSE;
    strcpy(rooms[15].description, "You crouch down along\nthe fence just out of\nthe guard's view.");
    rooms[15].doors[NORTH] = -1; rooms[15].doors[SOUTH] = -1; rooms[15].doors[EAST] = -1; rooms[15].doors[WEST] = 13;
    strcpy(rooms[15].dirdesc[NORTH], "Fence.");
    strcpy(rooms[15].dirdesc[SOUTH], "Fence.");
    strcpy(rooms[15].dirdesc[EAST], "Fence.");
    strcpy(rooms[15].dirdesc[WEST], "A fenced yard.");
    rooms[15].itemIndex[0] = -1; // no item
    rooms[15].itemIndex[1] = -1; // no item
    rooms[15].itemIndex[2] = -1; // no item
    rooms[15].itemIndex[3] = -1; // no item

    strcpy(rooms[16].name, "Barbwired Fence");
    rooms[16].locked = FALSE;
    strcpy(rooms[16].description, "You crouch down along\nthe fence just out of\nthe guard's view.");
    rooms[16].doors[NORTH] = 13; rooms[16].doors[SOUTH] = -1; rooms[16].doors[EAST] = -1; rooms[16].doors[WEST] = -1;
    strcpy(rooms[16].dirdesc[NORTH], "A fenced yard.");
    strcpy(rooms[16].dirdesc[SOUTH], "Fence.");
    strcpy(rooms[16].dirdesc[EAST], "Fence.");
    strcpy(rooms[16].dirdesc[WEST], "Fence.");
    rooms[16].itemIndex[0] = -1; // no item
    rooms[16].itemIndex[1] = -1; // no item
    rooms[16].itemIndex[2] = -1; // no item
    rooms[16].itemIndex[3] = -1; // no item

    print("Saving to disk...\n");

    cbm_open(15,8,15,"s1:rsa data");
    cbm_close(15);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(datafile);
    cbm_k_save(0xA000, 0xC000);

    print("Done!\n\n");

    print("Building strings...\n");

    strcpy(str[0], "\223\034>>> \022RED\222 \022\005SECTOR");
    strcpy(str[1], "\222\034 \022A\222 <<<\005\n\n\0");
    strcpy(str[2], "\005\n>");
    strcpy(str[3], "\nYou see:\n");
    strcpy(str[4], "\n\n");
    strcpy(str[5], "huh?\n");
    strcpy(str[6], "You've unlocked the\ndoor!\n");

    strcpy(str[7], "The door is locked.\n");
    strcpy(str[8], "You can't go that\nway.\n");
    strcpy(str[9], "You can't go that\nway!\n");
    strcpy(str[10], "Taken!\n");
    strcpy(str[11], "You can't take that.\n");
    strcpy(str[12], "Nothing to take!\n");
    strcpy(str[13], "You can't carry any\nmore!\n");
    strcpy(str[14], "Dropped.\n");
    strcpy(str[15], "You can't drop that\nhere.\n");
    strcpy(str[16], "You can't drop that.\n");

    strcpy(str[17], "\nENTER PASSWORD:\n>");
    strcpy(str[18], "SUCCESS! Drones\nhave ");
    strcpy(str[19], "been \nDISABLED!\n");
    strcpy(str[20], "INCORRECT.\n");
    strcpy(str[21], "TERMINAL LOCKED.\n");
    strcpy(str[22], "You can't use that.\n");
    strcpy(str[23], "You don't have\nanything.\n");
    strcpy(str[24], "\nLeaving!\n\n");
    strcpy(str[25], "Huh?\n");
    strcpy(str[26], "The attack drones\n");
    strcpy(str[27], "have spotted you!\n");
    strcpy(str[28], "The attack drones\n");
    strcpy(str[29], "are taking aim!\n");
    strcpy(str[30], "The attack drones\n");
    strcpy(str[31], "are firing!\n");
    strcpy(str[32], "The drones find\ntheir mark! ");
    strcpy(str[33], "You\nare \022DEAD\222.\n");
    strcpy(str[34], "\nAh, you gave it\nyour ");
    strcpy(str[35], "best shot,\nright?\n\n");
    strcpy(str[36], "\n\022CONGRATS\222! ");
    strcpy(str[37], "You made\nit!\n\n");
    strcpy(str[38], "Flashlight ON!\n");
    strcpy(str[39], "Flashlight OFF!\n");
    strcpy(str[40], "That did nothing.\n");
    strcpy(str[41], "You wave the access\ncard at the");
    strcpy(str[42], " reader.\nThe elevator springs\n");
    strcpy(str[43], "to life. It whirrs\nand you are");
    strcpy(str[44], " dumped\nout the doors.\n");
    strcpy(str[45], "There's\na buzzing noise...");

    print("Saving to disk...\n");

    cbm_open(15,8,15,"s1:rsa strings");
    cbm_close(15);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(strfile);
    cbm_k_save(0x0400, 0x0FFF);

    print("Done!\n");

}

void print(char *str) {
    while (*str) {
        __A__ = *str++;
        asm("jsr $ffd2");
    }
}
