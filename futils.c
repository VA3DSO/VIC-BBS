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

void directory(char drive, char partition) {

    struct cbm_dirent dir;
    char type, e, ch = 0;
    unsigned char res;
    int c = 1;

    cbm_open(15, drive, 15, "");
    sprintf(O, "$%c", partition);
    res = cbm_opendir(1, drive, O);

    e = errorcheck();

    if ((res == 0) && (e == 0)) {

        cbm_readdir(1, &dir); // disk header

        sprintf(O, "\n\n\022disk:%-15s\n", dir.name);
        print(O);

        while (cbm_readdir(1, &dir) == 0)
        {
            switch (dir.type) {
                case 16:
                    type = 'S';
                    break;
                case 17:
                    type = 'P';
                    break;
                case 19:
                    type = 'R';
                    break;
                default:
                    type = '?';
            }
            sprintf(O, "%-3u %-15s %c\n", dir.size, dir.name, type);
            print(O);
            c++;
            if (c >= 21) {
                ch = pause(TRUE);
                if (ch == 'Q') {
                    break;
                }
                c = 0;
            }
        }

        if (ch != 'Q') {
            sprintf(O, "\n%u blocks free.\n\n", dir.size);
            print(O);
        }

        cbm_closedir(1);
        cbm_close(15);

        if (ch != 'Q') {
            pause(FALSE);
        }

    }

}
