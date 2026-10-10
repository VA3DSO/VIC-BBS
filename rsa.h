/* RED SECTOR A */

/* MEASURE STRINGS

   <A         <B              <C                              <D
    5          16              32                              64
----|----------|---------------|-------------------------------|


*/

/* A STRINGS */
#define STRA_GAME_PROMPT 0
#define STRA_GAME_HUH 1
#define STRA_MIRR 2
#define STRA_POST 3
#define STRA_KEYS 4

/* B STRINGS */
#define STRB_YOU_SEE 0
#define STRB_ITEMS_HERE 1
#define STRB_CRYO_NAME 2
#define STRB_CRYO_MIRROR_NAME 3
#define STRB_CRYO_POSTER_NAME 4
#define STRB_HALLWAY1_NAME 5			// shared with all Hallways
#define STRB_HALLWAY1_EAST_MORE_HALLWAY 6	// shared with some Hallways
#define STRB_SERVERROOM_NAME 7
#define STRB_KEYS_NAME 8
#define STRB_TAKEN 9
#define STRB_NOTHING_TO_TAKE 10
#define STRB_YOUVE_GOT 11
#define STRB_NOTHING 12
#define STRB_DROPPED 13
#define STRB_NOTHING_TO_DROP 14

/* C STRINGS */
#define STRC_BUZZING 0
#define STRC_NOTHING 1
#define STRC_YOU_UNLOCKED_DOOR 2
#define STRC_DOOR_LOCKED 3
#define STRC_CANT_GO_THAT_WAY 4
#define STRC_CRYO_NORTH_CRYO_BEDS 5
#define STRC_CRYO_SOUTH_DOORWAY 6
#define STRC_CRYO_EAST_MIRROR 7
#define STRC_HALLWAY1_DESC 8			// shared with all Hallways
#define STRC_HALLWAY1_SOUTH_SERVER_ROOM 9
#define STRC_SERVERROOM_NORTH_HALLWAY 10
#define STRC_CANT_TAKE_THAT 11
#define STRC_CANT_CARRY_ANYMORE 12

/* D STRINGS */
#define STRD_RSA 0
#define STRD_CRYO_DESC 1
#define STRD_CRYO_WEST_POSTER 2
#define STRD_HALLWAY1_NORTH_CRYO_CHAMBER 3
#define STRD_HALLWAY1_WEST_WINDOW 4
#define STRD_SERVERROOM_DESC 5
#define STRD_SERVERROOM_SOUTH_SERVER_RACKS 6
#define STRD_SERVERROOM_EAST_WHITEBOARD 7
#define STRD_SERVERROOM_WEST_WIRES 8
#define STRD_CRYO_POSTER_GONE 9
#define STRD_CRYO_MIRROR_GONE 10

/* GAME PARAMETERS */
#define MAXROOMS 3
#define MAXITEMS 3

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3

#define A 5
#define B 16
#define C 32
#define D 64

#define STR_T 0
#define STR_N 1

#define IMMOVABLE 0
#define MOVABLE   1
#define DONTSHOW  2
#define NO_EXIT   255
#define TAKEN     255

/* ROOMS */
#define CRYOCHAMBER 0
#define HALLWAY1    1
#define SERVERROOM  2

#define HALLWAY2    4

#define HALLWAY3    7

#define BARRACKS    9
#define HALLWAY4    10

#define ELEVATOR    12

/* ITEMS */
#define MIRROR      0
#define POSTER      1
#define KEYS        2
#define CATCABLE    3
#define STAPLER     4
#define CELLPHONE   5
#define MICROSCOPE  6
#define ACCESSCARD  7
#define BEAKER      8
#define TURTLE      9
#define MEDKIT      10
#define SCALPEL     11
#define FLASHLIGHT  13
#define CRUTCHES    14
#define COOKIES     15
#define CANDYBAR    16
#define SINK        17
#define FOODTRAY    18
#define WIRECUTTERS 19
#define BUCKET      20
#define COMPUTER    21
#define COFFEECUP   22
#define BOOTS       23
#define GOLFCLUB    24
#define HELMET      25
#define GUN         26
#define BELT        27
#define CLIPBOARD   28
#define VEST        29

/* BOOLEANS */
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0
#define SUCCESS 0
#define ERROR 1

/* CHARACTERS */
#define DASH 45
#define RETURN 13

/* MEMORY ADDRESSES */
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

typedef struct {
    char name[2];                           // char[0] = A/B/C/D (string len), char[1] = string number
    char locked;                            // TRUE / FALSE
    char description[2];                    // char[0] = A/B/C/D (string len), char[1] = string number
    char doors[4];                          // pointer to other room or 255 for no exit
    char dirdesc[4][2];                     // N/S/E/W description
} Room;
// Room Size = 17 bytes

typedef struct {
    char name[2];                           // char[0] = A/B/C/D (string len), char[1] = string number
    char abbrev;                            // pointer to Abbreviation string
    char type;                              // IMMOVABLE / MOVABLE / DONTSHOW
    char room;                              // pointer to room where item is (255 if taken)
    char direction;                         // NORTH / SOUTH / EAST / WEST
} Item;
// Item Size = 6 bytes

#pragma bss-name ("LDATA")
Room rooms[MAXROOMS];               // 289 bytes
Item items[MAXITEMS];               // 180 bytes
char stra[30][5];                    // A Abbreviations 150 bytes
char strb[153][16];                  // B Short Strings 2425 bytes
#pragma bss-name ("HDATA")
char strc[128][32];                  // C Medium Strings 4096 bytes
char strd[64][64];                   // D Long Strings 4096 bytes
#pragma bss-name ("BSS")             // switch back to default

void play_red_sector_a(void);
void showRoom(char, char);
int dirLookup(char*);
char inHallway(char);

void showstr_by_type(char, char, char);
void showstr(const char*, char, char);

void parse_input(const char *, char *, char *);

char O[40];
