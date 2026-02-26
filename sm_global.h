/* global variables */
USER U;                         /* current logged in user */
SYSTEM S;                       /* system stats */
char DATE[11];                  /* date in yyyy-mm-dd format */
long TIME;                      /* seconds since midnight */
char AD;                        /* add day to time up calc */
char LOCALMODE = TRUE;          /* local mode toggle */
char CURSORSTATUS = FALSE;      /* cursor status toggle */
char SYSOP = FALSE;             /* Sysop in/out toggle */
char LOGGINGON = TRUE;          /* toggle for logging on - for times up calc */
char ONLINE = FALSE;            /* is user currently online? */
char I[40];                     /* system input variable */
char O[40];                     /* system output variable */
char W[40];                     /* for word wrap in the editor */

