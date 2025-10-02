#include <cbm.h>
#include <vic20.h>
#include <peekpoke.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "global.h"

void play_war();

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

                    print("\223\234\022GAME\222\005ZONE\n\n");

                    print("Hi ");
                    print(U.USERNAME);
                    print("!\n\n");

                    showfile("gamelist", FALSE, 0);

                    do {
                        print("\n\005Game\234?\005 ");
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
                            case 'X':
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

    char yn;

    showfile("war_intro", TRUE, 0);

    print("\n\nInstructions?");
    yn = get_command();

    if (yn == 'Y') {
        showfile("war_inst", TRUE, 0);
    }


}
