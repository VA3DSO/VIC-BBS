#pragma charmap(147, 147)
#pragma charmap(17, 17)

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

/* BOOTSTRAP ADDRESSES */
#define BS_LOCALMODE 251
#define BS_ID        252
#define BS_MODULE    253
#define BS_MSG_HI    0xA100
#define BS_MSG_LO    0xA101
#define BS_REPLY     0x0FFF
#define BS_ADDRESS   0xA000
#define BS_JUMP      "jmp $a000"
#define BS_ACTION    0xA100

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

/* common functions */
void chatmode(char);
void loadstats();
void savestats();
void saveuser(char);
int showfile(char*, char, char, int);
char open_userfile();
char gotouser(char);
char loaduser(char);
void close_userfile();
void settimelimit();

void lprint(char*);                   /* for printing locally only */
void putch(char);
void input(char, int, int, char);
char getch();
char pause(char);
char errorcheck();
void showerr(char*);
void clear(char*);
void trim(char*);
char hardchar(char);
void gettime();
void getdate();
int RTCconvert(int);
char gpause();

void print(char*);
void cursor_on();
void cursor_off();
void beep();
void sleep();
char carrierdetect();
char getch();
void putch(char ch);
void bootstrap(char*);

/* utility functions */
char confirm();
void askforhelp();
char get_command();
void clearbuffer(void);

/* global variables */
extern USER U;                         /* current logged in user */
extern SYSTEM S;                       /* system stats */
extern char CURSORSTATUS;              /* cursor status */
extern char DATE[11];                  /* date in yyyy-mm-dd format */
extern long TIME;                      /* seconds since midnight */
extern char AD;                        /* add day to time up calc */
extern char LOCALMODE;                 /* local mode toggle */
extern char SYSOP;                     /* Sysop in/out toggle */
extern char LOGGINGON;                 /* toggle for logging on - for times up calc */
extern char ONLINE;                    /* is there a user online? */
extern char I[40];                     /* system input variable */
extern char O[40];                     /* system output variable */
extern char W[40];                     /* for word wrap in the editor */
