#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "common.h"
#include "futils.h"
#include "global.h"

/* local functions */
char file_editor(int);
char file_edit(char*, char, int);
unsigned int file_load(char*, char);
void file_save(char*, char, unsigned int);
void file_viewer(unsigned int, char, int);

void main(void) {

    int msgnum;
    char id, hi, lo;
    char done = FALSE;

    hi = PEEK(BS_MSG_HI);
    lo = PEEK(BS_MSG_LO);
    msgnum = (hi * 256) + lo;

    LOCALMODE = PEEK(BS_LOCALMODE);
    ONLINE = carrierdetect();

    if (msgnum > 0) {

        /* we are editing a message! */

        if (ONLINE == TRUE) {

            id = PEEK(BS_ID);

            if (id > 0) {
                loaduser(id);
            }

            loadstats();

            file_editor(msgnum);

            savestats();
            saveuser(id);

            POKE(BS_MSG_HI, 0);
            POKE(BS_MSG_LO, 0);

        }

        bootstrap("BBS");

    } else {

        /* we are the sysop editing a file */
        id = PEEK(BS_ID);
        U.SECURITY = 0;

        if (id > 0) {

            loaduser(id);

            if (U.SECURITY >= 5) {

                do {
                    done = file_editor(0);
                } while (done == FALSE);

                bootstrap("SYSOP");

            } else {
                /* turns out we weren't a sysop! */
                bootstrap("BBS");
            }

        } else {

            bootstrap("BBS");

        }

    }

}

char file_editor(int msgnum) {

    char filename[32], fullname[32], drive, err[32], yn, res = 1, e, done = TRUE, partition;

    if (msgnum == 0) {
        print("\223\005\022FILE\222EDITOR\n\n");
        print("Filename ($ = dir)?\n>");
        input('A', 0, 32, FALSE);
        if (strlen(I) > 0) {
            strcpy(filename, I);
            trim(filename);
            print("\nDrive? (8-10)\n>");
            input('A', 0, 2, FALSE);
            print("\nPart? (0-9)\n>");
            partition = getch(); putch(partition); putch(13);
            if (strlen(I) > 0) {
                drive = (char)atoi(I);
                if (strcmp(filename,"$") == 0) {
                    directory(drive, partition);
                    done = FALSE;
                } else {

                    sprintf(I, "@%c:%s", partition, filename);
                    strcpy(filename, I);
                    sprintf(fullname, "%s,s,r", I);

                    cbm_open(15,drive,15,"");
                    cbm_open(2,drive,2,fullname);

                    cbm_read(15, err, 2);
                    err[2] = 0;

                    sprintf(O, "i%c", partition);
                    cbm_write(15, O, strlen(O));

                    cbm_close(2);
                    cbm_close(15);

                    if (strcmp(err, "62") == 0) {
                        print("New file... create?\n");
                        yn = confirm();
                        if (yn == TRUE) {

                            sprintf(fullname, "%s,s,w", filename);
                            cbm_open(2,drive,2,fullname);
                            cbm_close(2);
                            file_edit(filename, drive, 0);
                        }
                    } else {
                        file_edit(filename, drive, 0);
                    }


                }

            }
        }
    } else {

        res = file_edit("@0:temp", 8, msgnum);

        if (res == 0) {
            /* rename temp to m xxxx */
            sprintf(O, "r0:m %i=temp", msgnum);
            cbm_open(15, 8, 15, O);
            e = errorcheck();
            cbm_close(15);
        } else {
            /* abort! scratch temp */
            strcpy(O, "s0:temp");
            cbm_open(15, 8, 15, O);
            cbm_close(15);
            /* decrement S.NUMMESGS */
            S.NUMMESGS--;
            savestats();
        }
    }

    return done;

}

char file_edit(char *filename, char drive, int msgnum) {

    int i, j, line, ln, df, offset, sto;
    unsigned int QX, Q1, Q2, K;
    char editing = TRUE, a, yn, res = 1, wrap = 'W';

    QX = file_load(filename, drive);

    if (msgnum > 0) {
        offset = 7;
    } else {
        offset = 0;
    }

    if (QX > 0) {

        file_viewer(QX, FALSE, 0);

        print("  /s:save, /?:help\n\n");

        do {

            if ((wrap == 'W') && (strlen(W) > 0)) {
                strcpy(I, W);
                clear(W);
                print(I);
                input(wrap, 0, 21, TRUE);
            } else {
                input(wrap, 0, 21, FALSE);
            }

            if ((strlen(I) == 2) && (I[0] == 47)) {

                /* Slash Commands */

                strupper(I);

                switch(I[1]) {
                    case '@':
                        /* debug! */
                        sprintf(O, "\nLOCALMODE: %i\n", LOCALMODE); print(O);
                        sprintf(O, "\nONLINE: %i\n", ONLINE); print(O);
                        sprintf(O, "\nQR:%u\n", QR); print(O);
                        sprintf(O, "QX:%u\n", QX); print(O);
                        sprintf(O, "Q1:%u\n", Q1); print(O);
                        sprintf(O, "Q2:%u\n", Q2); print(O);
                        sprintf(O, "QZ:%u\n", QZ); print(O);
                        print("\nFirst 5 Bytes:\n");
                        for (K = QR; K < QR + 5; K++) {
                            sprintf(O,"%u:%i ", K, PEEK(K)); print(O);
                        }
                        print("\n\nLast 5 Bytes:\n");
                        for (K = QR + QX - 5; K < QR + QX; K++) {
                            sprintf(O,"%u:%i ", K, PEEK(K)); print(O);
                        }
                        print("\n");
                        pause(FALSE);
                        file_viewer(QX, FALSE, 0);
                        break;
                    case '!':
                        /* dump! */
                        print("\223");
                        j = 0;
                        for (K = QR; K < QR + QX; K++) {
                            sprintf(O,"%u:%c [%i]\n", K, PEEK(K), PEEK(K)); print(O);
                            j++;
                            if (j >= 21) {
                                j = 0;
                                if(pause(TRUE) == 'Q') {
                                    K = QR + QX;
                                    break;
                                }
                            }
                        }
                        pause(FALSE);
                        break;
                    case 'A':
                        /* abort */
                        editing = FALSE;
                        res = 1;
                        break;
                    case 'S':
                        /* save */
                        sprintf(O, "saving: %s, %i\n");
                        file_save(filename, drive, QX);
                        editing = FALSE;
                        res = 0;
                        break;
                    case 'L':
                        /* list */
                        file_viewer(QX, TRUE, offset);
                        break;
                    case 'P':
                        /* preview */
                        file_viewer(QX, FALSE, 0);
                        break;
                    case 'W':
                        if (wrap == 'W') {
                            wrap = 'A';
                            print("\n\022word wrap OFF!\n\n");
                        } else {
                            wrap = 'W';
                            print("\n\022word wrap ON!\n\n");
                        }
                        break;
                    case 'M':
                        /* show free memory */
                        K = QZ - (QR + QX);
                        sprintf(O, "\n%u bytes free.\n", K);
                        print(O);
                        pause(FALSE);
                        file_viewer(QX, FALSE, 0);
                        break;
                    case 'E':
                        /* edit line */
                        print("\nLine # to edit? ");
                        input('A', 0, 3, FALSE);
                        line = atoi(I) + offset;

                        if ((line > 0) && (line < 1000)) {

                            print("Searching...");
                            Q1 = QR;            /* first address of string */
                            Q2 = QR;            /* last address of string */
                            ln = 0;

                            /* this should return Q1 = first byte addres, Q2 = last byte address */
                            for (K = QR; K < QR + QX; K++) {
                                a = PEEK(K);
                                if (a == 13) {
                                    ln++;
                                    if (ln == line) {
                                        Q2 = K - 1;
                                        K = QR + QX;
                                        break;
                                    } else {
                                        Q1 = K + 1;
                                    }
                                }
                            }
                            if ((Q1 >= QR) && (Q1 <= QR + QX) && (Q2 >= QR) && (Q2 <= QR + QX)) {

                                /* we have our line! */
                                clear(I);
                                j = 0;
                                for (K = Q1; K <= Q2; K++) {
                                    I[j] = PEEK(K);
                                    j++;
                                }
                                I[j] = 0;

                                sto = strlen(I);

                                print("\nEdit Line:\n>");
                                print(I);
                                input('A', 0, 21, TRUE);

                                if (strlen(I) > 0) {

                                    df = strlen(I) - sto;

                                    print("\nWorking...");

                                    if (df < 0) {
                                        for (K = Q2; K < QR + QX; K++) {
                                            POKE(K + df, PEEK(K));
                                        }
                                    } else if (df > 0) {
                                        for (K = QR + QX - 1; K >= Q1; K--) {
                                            POKE(K + df, PEEK(K));
                                        }
                                    } else {
                                        /* do nothing because DF = 0! */
                                    }

                                    QX = QX + df;

                                    if (QR + QX >= QZ) {
                                        print("\n\n\022OUT OF MEMORY!\222\n");
                                        QX = QZ;
                                    }

                                    for (i=0; i < strlen(I); i++) {
                                        POKE(i+Q1,I[i]);
                                    }

                                    print(" DONE!\n");

                                } else {
                                    print("\nNO CHANGES MADE.\n");
                                }
                            } else {
                                print("\022NOT FOUND\n");
                            }
                        }
                        pause(FALSE);
                        file_viewer(QX, FALSE, 0);
                        break;
                        case 'D':
                            /* delete line */
                            if (QX > 0) {
                                print("\nLine # to delete? ");
                                input('A', 0, 3, FALSE);
                                line = atoi(I) + offset;
                                if ((line > 0) && (line < 1000)) {
                                    print("Searching...");
                                    Q1 = QR;
                                    Q2 = QR;
                                    ln = 0;
                                    for (K = QR; K < QR + QX; K++) {
                                        a = PEEK(K);
                                        if (a == 13) {
                                            ln++;
                                            if (ln == line) {
                                                Q2 = K;
                                                K = QR + QX;
                                                break;
                                            } else {
                                                Q1 = K + 1;
                                            }
                                        }
                                    }
                                    if ((Q1 >= QR) && (Q1 <= QR + QX) && (Q2 >= QR) && (Q2 <= QR + QX)) {
                                        /* we have our line! */
                                        clear(O);
                                        j = 0;
                                        for (K = Q1; K < Q2; K++) {
                                            O[j] = PEEK(K);
                                            j++;
                                        }
                                        O[j] = 0;
                                        print("\nDelete Line:\n>");
                                        print(O);
                                        yn = confirm();
                                        if (yn == TRUE) {
                                            print("\nWorking...");
                                            df = strlen(O) + 1;
                                            for (K=Q2; K<=QR+QX; K++) {
                                                POKE(K-df, PEEK(K));
                                            }
                                            QX = QX - df;
                                            print(" DONE!\n");
                                        }
                                    } else {
                                        print("\022NOT FOUND\n");
                                    }

                                }
                            } else {
                                print("\nNOTHING TO DELETE!\n");
                            }
                            pause(FALSE);
                            file_viewer(QX, FALSE, 0);
                            break;
                        case 'I':
                            /* insert line */
                            print("\nLine # to insert? ");
                            input('A', 0, 3, FALSE);
                            line = atoi(I) + offset;
                            if ((line > 0) && (line < 1000)) {
                                print("Searching...");
                                Q1 = QR;
                                Q2 = QR;
                                ln = 0;
                                for (K = QR; K < QR + QX; K++) {
                                    a = PEEK(K);
                                    if (a == 13) {
                                        ln++;
                                        if (ln == line) {
                                            Q2 = K;
                                            K = QR + QX;
                                            break;
                                        } else {
                                            Q1 = K + 1;
                                        }
                                    }
                                }
                                if ((Q1 >= QR) && (Q1 <= QR + QX) && (Q2 >= QR) && (Q2 <= QR + QX)) {
                                    /* we have our line! */
                                    print("\nWorking...");
                                    df = 1;
                                    for (K = QR + QX - 1; K >= Q1; K--) {
                                        POKE(K + df, PEEK(K));
                                    }
                                    POKE(Q1, 13);
                                    QX = QX + df;
                                    if (QR + QX >= QZ) {
                                        print("\n\n\022OUT OF MEMORY!\222\n");
                                        QX = QZ;
                                    } else {
                                        print(" DONE!\n");
                                    }
                                } else {
                                    print("\022NOT FOUND\n");
                                }

                            }
                            pause(FALSE);
                            file_viewer(QX, FALSE, 0);
                            break;
                        case 'C':
                            print("\nCLEAR RAM?\n");
                            yn = confirm();
                            if (yn == TRUE) {
                                QX = 0;
                                print("DONE!\n");
                                pause(FALSE);
                            }
                            file_viewer(QX, FALSE, 0);
                            break;
                        case 'H':
                        case '?':
                            print("\nSlash Commands:\n");
                            print("/a:Abort /s:Save \n/l:List  /p:Preview \n/e:Edit  /d:Delete\n/w:Wrap  /i:Insert\n");
                            break;
                        default:
                            print("? unknown ?\nEnter /? for help!\n");
                }

            } else {
                /* update RAM */
                strcat(I, "\n");
                for (i=0; i<=strlen(I); i++) {
                    if ((QR + QX >= QR) && (QR + QX < QZ)) {
                        POKE(QR + QX,I[i]);
                        QX++;
                    } else {
                        print("\n\n\022OUT OF MEMORY!\222\n");
                        i = strlen(I);
                        break;
                    }
                }
                QX--;
                clear(I);
            }

        } while (editing == TRUE);

    }

    return res;

}

unsigned int file_load(char *filename, char drive) {

    unsigned int QX = 0;
    char fullname[32], st, e, a;

    /* clear upper memory */
    memset((void *)40960UL, 0, 8191UL);

    /* LOAD FILE INTO MEMORY */
    strcpy(fullname, filename);
    strcat(fullname, ",s,r");

    cbm_open(15, drive, 15, "");
    cbm_open(2, drive, 2, fullname);

    e = errorcheck();

    if (e == 0) {

        st = 0;
        print("\nLoading text...");

        do {

            cbm_k_chkin(2);
            a = cbm_k_basin();
            cbm_k_clrch();
            st = cbm_k_readst();

            if ((st != 0) && (st != 64)) {
                e = errorcheck();
                QX = 0;
                break;
            } else if ((a != 0) && (a != 2)) {
                POKE((QR + QX), a);
                QX++;
                if (QR + QX >= QZ) {
                    print("\n\n\022OUT OF MEMORY!\222\n");
                    st = 64;
                }
            }

        } while (st == 0);

    } else {
        QX = 0;
    }

    /* QX = byte where next char would go */

    cbm_close(2);
    cbm_close(15);

    return QX;

}

void file_save(char *filename, char drive, unsigned int QX) {

    unsigned int K;
    char fullname[32], e;

    /* SAVE FILE FROM MEMORY */
    sprintf(fullname, "%s,s,w", filename);

    cbm_open(15,drive,15,"");
    cbm_open(2,drive,2,fullname);

    e = errorcheck();

    if (e == 0) {

        print("\223\005Saving...");

        for (K = QR; K < QR + QX; K++) {
            cbm_k_ckout(2);
            cbm_k_bsout(PEEK(K));
            cbm_k_clrch();

            e = errorcheck();

            if (e != 0) {
                break;
            }
        }

        /* clear upper memory */
        memset((void *)40960UL, 0, 8191UL);

        print("DONE!\n");

    } else {
        pause(FALSE);
    }

    cbm_close(2);
    cbm_close(15);

}

void file_viewer(unsigned int QX, char listing, int offset) {

    int k, line = 2, ln = 0;
    char ch;
    unsigned int K, lqr;

    print("\223\005");

    if (listing == TRUE) {
        print("\223Line 1:\n");
    }

    if (offset > 0) {
        k = 0;
        for (K = QR; K < QR + QX; K++) {
            if (PEEK(K) == 13) {
                k++;
                if (k >= offset) {
                    lqr = K + 1;
                    K = QR + QX;
                    break;
                }
            }
        }
    } else {
        lqr = QR;
    }

    for (K = lqr; K < QR + QX; K++) {

        ch = PEEK(K);

        if ((ch == 13) && (listing == TRUE)) {
            sprintf(O, "\n\nLine %i:\n", line); print(O);
            line++;
            ln = ln + 3;
        } else if (ch == 13) {
            putch(ch);
            ln++;
        } else {
            if ((ch == 147) && (listing == TRUE)) {
                print("\022C\222");
            } else {
                putch(ch);
            }
        }

        if (ln >= 21) {
            ch = pause(TRUE);
            ln = 0;
            if (ch == 'Q') {
                K = QX + QR;
            }
        }
    }
}

