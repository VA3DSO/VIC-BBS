#pragma charmap(147, 147)
#pragma charmap(17, 17)
#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <stdio.h>
#include <string.h>
#include "rsa.h"

/* local functions */
void print(char*);
void setstr(char*, const char*, int);

void main(void) {

    char *lowramfile = "@1:rsa low";
    char *highramfile = "@1:rsa high";

    POKE(36879UL, 8);
    print("\223\034>>> \022RED\222 \005\022SECTOR\222 \034\022A\222 <<<\005\n\n");
    print("Building worlds...\n");

    //-------------------------------------------------------------------------------
    // --- GENERAL GAMEPLAY ---
    //-------------------------------------------------------------------------------
    setstr(stra[STRA_GAME_PROMPT], "\005\n>", A);
    setstr(stra[STRA_GAME_HUH], "huh?", A);

    setstr(strb[STRB_YOU_SEE], "\nYou see:", B);
    setstr(strb[STRB_ITEMS_HERE], "Items here:\n", B);
    setstr(strb[STRB_TAKEN], "Taken!\n", B);
    setstr(strb[STRB_NOTHING_TO_TAKE], "Nothing to take!", B);
    setstr(strb[STRB_DROPPED], "Dropped.\n", B);
    setstr(strb[STRB_NOTHING_TO_DROP], "Nothing to drop!", B);

    setstr(strc[STRC_NOTHING], "\nYou see:\nnothing!\n", C);
    setstr(strc[STRC_BUZZING], "There's a buzzing\nnoise...", C);
    setstr(strc[STRC_CANT_TAKE_THAT], "You can't take that.\n", C);
    setstr(strc[STRC_CANT_CARRY_ANYMORE], "You can't carry any\nmore!\n", C);
    
    setstr(strc[STRC_YOU_UNLOCKED_DOOR], "You've unlocked the\ndoor!\n", C);
    setstr(strc[STRC_DOOR_LOCKED], "The door is locked.\n", C);
    setstr(strc[STRC_CANT_GO_THAT_WAY], "You can't go that\nway!\n", C);

    setstr(strd[STRD_RSA], "\223\034>>> \022RED\222 \022\005SECTOR\222\034 \022A\222 <<<\005", D);

    //-------------------------------------------------------------------------------
    // --- CRYO CHAMBER ---
    //-------------------------------------------------------------------------------
    rooms[CRYOCHAMBER].name[STR_T] = B;
    rooms[CRYOCHAMBER].name[STR_N] = STRB_CRYO_NAME;
    rooms[CRYOCHAMBER].locked = FALSE;
    rooms[CRYOCHAMBER].description[STR_T] = D;
    rooms[CRYOCHAMBER].description[STR_N] = STRD_CRYO_DESC;
    rooms[CRYOCHAMBER].doors[NORTH] = NO_EXIT;
    rooms[CRYOCHAMBER].doors[SOUTH] = HALLWAY1;
    rooms[CRYOCHAMBER].doors[EAST] = NO_EXIT;
    rooms[CRYOCHAMBER].doors[WEST] = NO_EXIT;
    rooms[CRYOCHAMBER].dirdesc[NORTH][STR_T] = C;
    rooms[CRYOCHAMBER].dirdesc[NORTH][STR_N] = STRC_CRYO_NORTH_CRYO_BEDS;         
    rooms[CRYOCHAMBER].dirdesc[SOUTH][STR_T] = C;
    rooms[CRYOCHAMBER].dirdesc[SOUTH][STR_N] = STRC_CRYO_SOUTH_DOORWAY;
    rooms[CRYOCHAMBER].dirdesc[EAST][STR_T] = C;
    rooms[CRYOCHAMBER].dirdesc[EAST][STR_N] = STRC_CRYO_EAST_MIRROR;
    rooms[CRYOCHAMBER].dirdesc[WEST][STR_T] = D;
    rooms[CRYOCHAMBER].dirdesc[WEST][STR_N] = STRD_CRYO_WEST_POSTER;

    items[MIRROR].name[STR_T] = B;
    items[MIRROR].name[STR_N] = STRB_CRYO_MIRROR_NAME;
    items[MIRROR].abbrev = STRA_MIRR;
    items[MIRROR].type = MOVABLE;
    items[MIRROR].room = CRYOCHAMBER;
    items[MIRROR].direction = EAST;

    items[POSTER].name[STR_T] = B;
    items[POSTER].name[STR_N] = STRB_CRYO_POSTER_NAME;
    items[POSTER].abbrev = STRA_POST;
    items[POSTER].type = MOVABLE;
    items[POSTER].room = CRYOCHAMBER;
    items[POSTER].direction = WEST;

    /* needed for testing */
    items[KEYS].name[STR_T] = B;
    items[KEYS].name[STR_N] = STRB_KEYS_NAME;
    items[KEYS].abbrev = STRA_KEYS;
    items[KEYS].type = MOVABLE;
    items[KEYS].room = CRYOCHAMBER;
    items[KEYS].direction = NORTH;

    setstr(stra[STRA_KEYS], "keys", A);
    setstr(strb[STRB_KEYS_NAME], "master keys", B);
    /* end testing zone */

    setstr(stra[STRA_MIRR], "mirr", A);
    setstr(stra[STRA_POST], "post", A);

    setstr(strb[STRB_CRYO_NAME], "Cryo Chamber", B);
    setstr(strb[STRB_CRYO_MIRROR_NAME], "mirror", B);
    setstr(strb[STRB_CRYO_POSTER_NAME], "pinup poster", B);
    setstr(strb[STRB_YOUVE_GOT], "You've got...", B);
    setstr(strb[STRB_NOTHING], "nothing!", B);

    setstr(strc[STRC_CRYO_NORTH_CRYO_BEDS], "A row of broken cryo\nbeds.", C);
    setstr(strc[STRC_CRYO_SOUTH_DOORWAY], "A door leading to a\nhallway.", C);
    setstr(strc[STRC_CRYO_EAST_MIRROR], "A dusty mirror is\non the wall.", C);

    setstr(strd[STRD_CRYO_DESC], "Dim light flickers\nacross ruined cryo\npods and stale air.", D);
    setstr(strd[STRD_CRYO_WEST_POSTER], "A torn poster of\nFarrah Fawcett in a\nred swimsuit!", D);
    setstr(strd[STRD_CRYO_POSTER_GONE], "Bits of tape... was\nthere a poster here\nbefore?", D);
    setstr(strd[STRD_CRYO_MIRROR_GONE], "A dusty oval outline.\nWas there a mirror\nhere?", D);

    //-------------------------------------------------------------------------------
    // --- HALLWAY 1 ---
    //-------------------------------------------------------------------------------
    rooms[HALLWAY1].name[STR_T] = B;
    rooms[HALLWAY1].name[STR_N] = STRB_HALLWAY1_NAME;
    rooms[HALLWAY1].locked = FALSE;
    rooms[HALLWAY1].description[STR_T] = C;
    rooms[HALLWAY1].description[STR_N] = STRC_HALLWAY1_DESC;
    rooms[HALLWAY1].doors[NORTH] = CRYOCHAMBER;
    rooms[HALLWAY1].doors[SOUTH] = SERVERROOM;
    rooms[HALLWAY1].doors[EAST] = NO_EXIT;
    rooms[HALLWAY1].doors[WEST] = NO_EXIT;         // Hallway 2 (eventually)
    rooms[HALLWAY1].dirdesc[NORTH][STR_T] = D;
    rooms[HALLWAY1].dirdesc[NORTH][STR_N] = STRD_HALLWAY1_NORTH_CRYO_CHAMBER;         
    rooms[HALLWAY1].dirdesc[SOUTH][STR_T] = C;
    rooms[HALLWAY1].dirdesc[SOUTH][STR_N] = STRC_HALLWAY1_SOUTH_SERVER_ROOM;
    rooms[HALLWAY1].dirdesc[EAST][STR_T] = B;
    rooms[HALLWAY1].dirdesc[EAST][STR_N] = STRB_HALLWAY1_EAST_MORE_HALLWAY;
    rooms[HALLWAY1].dirdesc[WEST][STR_T] = D;
    rooms[HALLWAY1].dirdesc[WEST][STR_N] = STRD_HALLWAY1_WEST_WINDOW;
    
    setstr(strb[STRB_HALLWAY1_NAME], "Hallway", B);
    setstr(strc[STRC_HALLWAY1_DESC], "There are rows of\nclosed doors.", C);
    setstr(strd[STRD_HALLWAY1_NORTH_CRYO_CHAMBER], "A door leading to\nthe cryo chamber.", D);
    setstr(strc[STRC_HALLWAY1_SOUTH_SERVER_ROOM], "A door leading to a\nserver room.", C);
    setstr(strb[STRB_HALLWAY1_EAST_MORE_HALLWAY], "More hallway.", B);
    setstr(strd[STRD_HALLWAY1_WEST_WINDOW], "A window through\nwhich you can see a\ndystopian wasteland.", D);

    //-------------------------------------------------------------------------------
    // --- SERVER ROOM ---
    //-------------------------------------------------------------------------------
    rooms[SERVERROOM].name[STR_T] = B;
    rooms[SERVERROOM].name[STR_N] = STRB_SERVERROOM_NAME;
    rooms[SERVERROOM].locked = TRUE;
    rooms[SERVERROOM].description[STR_T] = D;
    rooms[SERVERROOM].description[STR_N] = STRD_SERVERROOM_DESC;
    rooms[SERVERROOM].doors[NORTH] = HALLWAY1;
    rooms[SERVERROOM].doors[SOUTH] = NO_EXIT;
    rooms[SERVERROOM].doors[EAST] = NO_EXIT;
    rooms[SERVERROOM].doors[WEST] = NO_EXIT;
    rooms[SERVERROOM].dirdesc[NORTH][STR_T] = C;
    rooms[SERVERROOM].dirdesc[NORTH][STR_N] = STRC_SERVERROOM_NORTH_HALLWAY;         
    rooms[SERVERROOM].dirdesc[SOUTH][STR_T] = D;
    rooms[SERVERROOM].dirdesc[SOUTH][STR_N] = STRD_SERVERROOM_SOUTH_SERVER_RACKS;
    rooms[SERVERROOM].dirdesc[EAST][STR_T] = D;
    rooms[SERVERROOM].dirdesc[EAST][STR_N] = STRD_SERVERROOM_EAST_WHITEBOARD;
    rooms[SERVERROOM].dirdesc[WEST][STR_T] = D;
    rooms[SERVERROOM].dirdesc[WEST][STR_N] = STRD_SERVERROOM_WEST_WIRES;
    
    setstr(strb[STRB_SERVERROOM_NAME], "Server Room", B);
    setstr(strd[STRD_SERVERROOM_DESC], "You're in a room that\nreeks of ozone.", D);
    setstr(strc[STRC_SERVERROOM_NORTH_HALLWAY], "A door leading to a\nhallway.", C);
    setstr(strd[STRD_SERVERROOM_SOUTH_SERVER_RACKS], "Rows and rows of\ndishevelled server\nracks.", D);
    setstr(strd[STRD_SERVERROOM_EAST_WHITEBOARD], "A whiteboard with\nsmeared network\ndiagrams on it.", D);
    setstr(strd[STRD_SERVERROOM_WEST_WIRES], "The west wall is\ncracked, wires hang\nloose and sparking.", D);

    //-------------------------------------------------------------------------------
    // --- SAVE TO DISK ---
    //-------------------------------------------------------------------------------

    print("Saving 1/2... ");

    cbm_open(15,8,15,"s1:rsa low");
    cbm_close(15);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(lowramfile);
    cbm_k_save(0x0400, 0x0FFF);

    print("Done!\n");

    print("Saving 2/2... ");

    cbm_open(15,8,15,"s1:rsa high");
    cbm_close(15);

    cbm_k_setlfs(3, 8, 1);
    cbm_k_setnam(highramfile);
    cbm_k_save(0xA000, 0xC000);

    print("Done!\n");

}

void print(char *str) {
    while (*str) {
        __A__ = *str++;
        asm("jsr $ffd2");
    }
}

void setstr(char *dest, const char *src, int len) {

    int i;

    memset(dest, 0, len);                   // zero out string
    
    for (i = 0; i < len && src[i]; i++) {
        dest[i] = src[i];
    }
}
