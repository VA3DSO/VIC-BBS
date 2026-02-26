#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "global.h"

typedef struct {
    int rank;
    int suit;
} Card;

typedef struct {
    char ch;
    int x, y;
    int active;
} SpaceObject;

void play_war();
void shuffleDeck(Card*);
void showCard(Card*, int);
void clearDeck(Card*);
void swapCards(Card*, int, int*, Card*, int*);
void nextCard(int*, int);
void play_mastermind();

const char suits[4] = {97, 115, 120, 122};
const char ranks[15] = {'0', '0', '2', '3', '4', '5', '6', '7', '8', '9', '1', 'j', 'q', 'k', 'a'};

void main(void) {

    char ch, id;
    char running = TRUE;

    LOCALMODE = PEEK(BS_LOCALMODE);

    if (PEEK(BS_MODULE) == 2) {

        ONLINE = carrierdetect();

        if (ONLINE == TRUE) {

            id = PEEK(BS_ID);

            if (id > 0) {
                loaduser(id);

                if (U.SECURITY >= 3) {

                    loadstats();

                    do {

                        print("\223\234\022GAME\222\005ZONE\n\n");

                        print("Hi ");
                        print(U.USERNAME);
                        print("!\n\n");

                        showfile("gamelist", 8, FALSE, 0);

                        print("\n\005GAME\234:\005 ");
                        cursor_on();

                        ch = getch();

                        if (ch == 255) {
                            ONLINE = FALSE;
                            break;
                        }

                        ch = toupper(ch);
                        putch(ch);
                        print("\n");

                        switch(ch) {
                            case '1':
                                play_war();
                                break;
                            case '2':
                                play_mastermind();
                                break;
                            case '3':
                                showfile("rsa intro", 8, TRUE, 0);
                                /* bootstrap("RSA"); */
                                break;
                            case 'X':
                                running = FALSE;
                                break;
                            case 'H':
                            case '?':
                                showfile("gamelist", 8, FALSE, 0);
                                break;
                            case 255:
                                print("\005\n\nCarrier Dropped!\n");
                                running = FALSE;
                                break;
                            default:
                                askforhelp();
                        }

                        ONLINE = carrierdetect();

                    } while ((running == TRUE) && (ONLINE == TRUE));

                }
            }
        }

        bootstrap("BBS");

    } else {
        print("\nLoad BBS instead.\n");
    }


}

void play_war() {

    char ch, yn;
    Card cardDeck[52], userDeck[52], vicDeck[52], warDeck[52];
    int userCard, userTotal, vicCard, vicTotal, warCard, warTotal;
    char atWar = FALSE, playing = TRUE;
    int i, j;

    shuffleDeck(cardDeck);
    clearDeck(userDeck);
    clearDeck(vicDeck);
    clearDeck(warDeck);

    userCard = 0;
    userTotal = 0;
    vicCard = 0;
    vicTotal = 0;

    /* deal cards */
    for (i = 0; i < 52; i = i + 2) {
        userDeck[userTotal] = cardDeck[i];
        vicDeck[vicTotal] = cardDeck[i + 1];
        userTotal++;
        vicTotal++;
    }

    showfile("war intro", 8, FALSE, 0);

    print("\n\nInstructions (Y/N)?");
    yn = get_command();

    if (yn == 'Y') {
        showfile("war inst", 8, TRUE, 0);
    }

    while(playing == TRUE) {

        sprintf(O, "\223\216\022   user:%2d   vic:%2d   \222", userTotal, vicTotal); print(O);

        putch(19); putch(17); putch(17); putch(17);
        putch(29);

        showCard(userDeck, userCard);

        putch(19); putch(17); putch(17); putch(17);
        putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29);

        showCard(vicDeck, vicCard);

        print("\n\n");

        if (userDeck[userCard].rank > vicDeck[vicCard].rank) {
            print("   user wins card!   \n\n");
            swapCards(vicDeck, vicCard, &vicTotal, userDeck, &userTotal);
            if(vicTotal < 1) {
                playing = FALSE;
                break;
            }
        } else if (userDeck[userCard].rank < vicDeck[vicCard].rank) {
            print("    vic wins card!   \n\n");
            swapCards(userDeck, userCard, &userTotal, vicDeck, &vicTotal);
            if(userTotal < 1) {
                playing = FALSE;
                break;
            }
        } else {
            atWar = TRUE;
            warCard = 0;
            warTotal = 0;

            while (atWar == TRUE) {

                /* check if either player has less that 4 cards left */
                if(userTotal < 4) {
                    userTotal = 0;
                    playing = FALSE;
                    break;
                } else if (vicTotal < 4) {
                    vicTotal = 0;
                    playing = FALSE;
                    break;
                }

                /* move current plus 3 cards from each deck to war pile */
                for (j = 0; j <= 3; j++) {
                    swapCards(userDeck, userCard, &userTotal, warDeck, &warTotal);
                    nextCard(&userCard, userTotal);
                    swapCards(vicDeck, vicCard, &vicTotal, warDeck, &warTotal);
                    nextCard(&vicCard, vicTotal);
                }

                putch(19); putch(17); putch(17); putch(17); putch(17); putch(17);
                putch(29); putch(29);

                showCard(warDeck, -1);

                putch(145); putch(145); putch(145); putch(145); putch(157); putch(157); putch(157); putch(157);
                sprintf(O, "x%d", (warTotal - 1) / 2); print(O);

                putch(19); putch(17); putch(17); putch(17); putch(17); putch(17);
                putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29);

                showCard(warDeck, -1);

                putch(145); putch(145); putch(145); putch(145); putch(157); putch(157); putch(157); putch(157);
                sprintf(O, "x%d", (warTotal - 1) / 2); print(O);

                print("\n\n\n\n\n\n");

                print("        \022war!\222         \n\n");

                gpause();

                /* draw 4th card from each deck and compare */
                putch(19); putch(17); putch(17); putch(17); putch(17); putch(17); putch(17); putch(17);
                putch(29); putch(29); putch(29);

                showCard(userDeck, userCard);

                putch(19); putch(17); putch(17); putch(17); putch(17); putch(17); putch(17); putch(17);
                putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29); putch(29);

                showCard(vicDeck, vicCard);

                print("\n\n");

                if (userDeck[userCard].rank > vicDeck[vicCard].rank) {
                    print("   user wins war!    \n\n");
                    for (j = warTotal - 1; j >= 0; j--) {
                        swapCards(warDeck, j, &warTotal, userDeck, &userTotal);
                    }
                    atWar = FALSE;
                } else if (userDeck[userCard].rank < vicDeck[vicCard].rank) {
                    print("    vic wins war!    \n\n");
                    for (j = warTotal -1; j >= 0; j--) {
                        swapCards(warDeck, j, &warTotal, vicDeck, &vicTotal);
                    }
                    atWar = FALSE;
                } else {
                    print("     \022war again!\222      \n\n");
                }

                if (atWar == TRUE) {
                    gpause();
                }

            }


        }

        nextCard(&vicCard, vicTotal);
        nextCard(&userCard, userTotal);

        ch = gpause();

        if (ch == 'q') {
            playing = FALSE;
            break;
        }

    }

    print("\n\n");

    if (vicTotal == 0) {
        print("   \022user wins!!!\222    \n\n");
    } else if (userTotal == 0) {
        print("    \022vic wins!!!\222    \n\n");
    } else {
        print("     \022aborting!\222     \n\n");
    }

    gpause();

    print("\223\016");

}

void shuffleDeck(Card *deck) {

    int i = 0, j;
    int suit, rank;
    Card temp;

    /* seed random number generator based on Jiffy clock */
    srand(PEEK(162));

    for (suit = 0; suit < 4; suit++) {
        for (rank = 2; rank <= 14; rank++) {
            deck[i].rank = rank;
            deck[i].suit = suit;
            i++;
        }
    }

    /* shuffle deck */
    for (i = 51; i > 0; i--) {
        j = rand() % (i + 1);
        temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }

}

void showCard(Card *card, int c) {

    int i;

    putch(176); putch(96); putch(96); putch(96); putch(96); putch(174);
    putch(17); putch(157); putch(157); putch(157); putch(157); putch(157); putch(157);

    putch(125);

    if (c > -1) {
        if((card[c].suit == 0) || (card[c].suit == 2)) {
            putch(5);
        } else {
            putch(28);
        }
        if (ranks[card[c].rank] == '1') {
            sprintf(O, "%c0%c \005", ranks[card[c].rank], suits[card[c].suit]); print(O);
        } else {
            sprintf(O, "%c%c  \005", ranks[card[c].rank], suits[card[c].suit]); print(O);
        }
    } else {
        putch(166); putch(166); putch(166); putch(166);
    }

    putch(125);

    putch(17); putch(157); putch(157); putch(157); putch(157); putch(157); putch(157);

    for (i = 0; i < 6; i++) {
        if (c > -1) {
            putch(125); putch(32); putch(32); putch(32); putch(32); putch(125);
        } else {
            putch(125); putch(166); putch(166); putch(166); putch(166); putch(125);
        }
        putch(17); putch(157); putch(157); putch(157); putch(157); putch(157); putch(157);
    }

    putch(173); putch(96); putch(96); putch(96); putch(96); putch(189);

}

void clearDeck(Card *deck) {

    int i;

    for (i = 0; i < 52; i++) {
        deck[i].rank = 0;
        deck[i].suit = 0;
    }
}

void swapCards(Card* fromDeck, int fromCard, int* fromTotal, Card* toDeck, int* toTotal) {

    int i, fTotal, tTotal;

    fTotal = *fromTotal;
    tTotal = *toTotal;

    toDeck[tTotal] = fromDeck[fromCard];
    tTotal++;

    for (i = fromCard; i < fTotal - 1; i++) {
        fromDeck[i] = fromDeck[i + 1];
    }
    fromDeck[i].rank = 0;
    fromDeck[i].suit = 0;
    fTotal--;

    *fromTotal = fTotal;
    *toTotal = tTotal;

}

void nextCard(int* theCard, int theTotal) {

    int tempCard;

    tempCard = *theCard;

    tempCard++;

    if (tempCard > theTotal - 1) {
        tempCard = 0;
    }

    *theCard = tempCard;

}

void play_mastermind() {

    /*
     * COLORS: WHT, RED, PUR, GRN, BLU, YEL
     * WRPGBY
     *
     * xxxxxxxxxxxxxxxxxxxxx
     * codebreaker W R P G B Y
     */

    char playing, inGame = TRUE;
    char yn;
    char code[4];
    char guess[4];
    char cols[6];
    char keys[6];
    char used_guess[4];
    char used_code[4];
    char exact, partial;
    char tries;

    int i, j, g;

    showfile("master intro", 8, FALSE, 0);

    print("\n\nInstructions (Y/N)?");
    yn = get_command();

    if (yn == 'Y') {
        showfile("master inst", 8, TRUE, 0);
    }

    do {

        playing = TRUE;
        tries = 0;

        print("\223\216\234\022code\222\005breaker    ");

        cols[0] = 5;
        keys[0] = 'w';

        cols[1] = 28;
        keys[1] = 'r';

        cols[2] = 156;
        keys[2] = 'p';

        cols[3] = 30;
        keys[3] = 'g';

        cols[4] = 31;
        keys[4] = 'b';

        cols[5] = 158;
        keys[5] = 'y';

        for (i = 0; i < 6; i++) {
            putch(cols[i]);
            putch(keys[i]);
        }
        putch(5); putch(13); putch(13);

        srand(PEEK(162));

        for (i = 0; i < 4; i++) {
            code[i] = rand() % (4 + 1);
        }

        print("guess vic's code. go!\n");

        while(playing == TRUE) {

            for (i = 0; i < 4; i++) {
                used_guess[i] = 0;
                used_code[i] = 0;
            }

            exact = 0;
            partial = 0;

            tries++;

            sprintf(O, "\nguess[%d]? ", tries); print(O);
            input('G', 4, 4, FALSE);

            for (i = 0; i < 14; i++) {
                putch(20);
            }
            if (tries > 9) {
                putch(20);
            }

            strlower(I);

            if (strcmp(I, "quit") == 0) {
                print("\n\n");
                playing = FALSE;
            }

            /* convert text to guess[] */
            g = 0;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 6; j++) {
                    if (keys[j] == I[i]) {
                        guess[g] = j;
                        g++;
                        break;
                    }
                }
            }

            for (i = 0; i < 4; i++) {
                putch(cols[guess[i]]);
                putch(113);
            }

            putch(5); putch(32);

            /* first pass - exact matches */
            for (i = 0; i < 4; i++) {
                if (guess[i] == code[i]) {
                    exact++;
                    used_guess[i] = 1;
                    used_code[i] = 1;
                }
            }

            /* second pass - partial matches */
            for (i = 0; i < 4; i++) {
                if (!used_guess[i]) {
                    for (j = 0; j < 4; j++) {
                        if (!used_code[j] && guess[i] == code[j]) {
                            partial++;
                            used_code[j] = 1;
                            break;
                        }
                    }
                }
            }

            if (exact > 0) {
                for (i = 1; i <= exact; i++) {
                    putch(113);
                }
            }

            if (partial > 0) {
                for (i = 1; i <= partial; i++) {
                    putch(119);
                }
            }

            /*
             *   sprintf(O, "\ncode: %d%d%d%d\n", code[0], code[1], code[2], code[3]); print(O);
             *   sprintf(O, "\nguess:%d%d%d%d\n", guess[0], guess[1], guess[2], guess[3]); print(O);
             */

            if (exact == 4) {
                print("\n\n\022user wins!\n\n");
                playing = FALSE;
            }

            if ((tries == 12) && (exact < 4)) {
                print("\n\n\022user loses!\222 it was:\n");
                for (i = 0; i < 4; i++) {
                    putch(cols[code[i]]);
                    putch(113);
                }
                putch(5); putch(13); putch(13);
                playing = FALSE;
            }
        }

        beep();
        print("play again (y/n)?");
        yn = get_command();

        if (yn != 'Y') {
            inGame = FALSE;
        }

    } while (inGame == TRUE);

    print("\223\016");

}
