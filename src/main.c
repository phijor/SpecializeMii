/* SpecializeMii - Golden pants for everyone!
 * Copyright (C) 2016 phijor (mail [at] phijor [dot] me)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cfldb.h"
#include "crc.h"
#include "mii.h"

#define PAGELEN 25

#define COLORIZE(color, str) "\e[" #color "m" str "\e[0m"

PrintConsole c_info;
PrintConsole c_default;

void print_mii_special_list(Mii *miis,
                            char **desc,
                            size_t len,
                            size_t indicator)
{
    printf("\033[2J"
           "Special Miis: ([special] @ (page:slot))\n");
    size_t offset = indicator - (indicator % PAGELEN);
    size_t end    = (len > offset + PAGELEN) ? offset + PAGELEN : len;
    for (size_t i = offset; i < end; i++) {
        if (desc[i] == NULL || !mii_is_valid(&miis[i])) {
            printf("%c " COLORIZE(2;37, "[---] === empty slot ===")"\n",
                   i == indicator ? '>' : ' ');
            continue;
        }

        printf("%c [%s] @ (%02d:%02d) %s\n",
               i == indicator ? '>' : ' ',
               mii_get_special(&miis[i]) == MII_SPECIAL ? COLORIZE(1;33, "yes")
                                                        : " no",
               miis[i].position.page + 1,
               miis[i].position.slot + 1,
               desc[i]);
    }
}

void print_usage()
{
    PrintConsole *prev = consoleSelect(&c_info);
    // clang-format off
    printf(APPLICATION_NAME " - " APPLICATION_REV ":\n"
            "\n"
           "[A]      - Toggle special/non-special\n"
           "[<>^v]   - Navigate Mii list\n"
           "[SELECT] - Save Changes\n"
           "[START]  - Exit\n"
           "\n"
           "page:slot refers to a Mii's position\n"
           "in MiiMaker.\n"
           "\n"
           "Note that this application currently\n"
           "does *not* support  UTF16-symbols in\n"
           "names, such as Hiragana, Kanji, etc.\n"
           "\n"
           COLORIZE(1;31, "Important:\n")
           "Setting a special Mii "COLORIZE(33, "*SHAREABLE*")" or\n"
           COLORIZE(33, "*COPYABLE*")" in MiiMaker "COLORIZE(33, "*CRASHES*")" the\n"
           "system and is generally a bad idea.\n"
           "\n"
           "I AM NOT RESPONSIBLE FOR EVENTUAL\n"
           "DAMAGES TO YOUR SYSTEM OR DATA.\n"
           );
    // clang-format on
    consoleSelect(prev);
}

bool prompt(char const *const msg, char const *const yes, char const *const no)
{
    printf("\033[2J"
           "%s\n"
           "[A] - %s\n"
           "[B] - %s\n",
           msg,
           yes,
           no);
    u32 key_down = 0;
    while (1) {
        hidScanInput();
        key_down = hidKeysDown();

        if (key_down & KEY_A) {
            return true;
        } else if (key_down & KEY_B) {
            return false;
        }
    }
}

void __attribute__((noreturn))
hang(char const *const desc, Result const errcode)
{
    consoleSelect(&c_info);
    fprintf(stderr,
            "\n"
            "\033[31mAn error occured!\033[0m\n"
            "  Description: %s\n"
            "  Error-Code:  0x%lx\n"
            "\n"
            "Press START to exit the application.\n",
            desc,
            (long int) errcode);

    while (aptMainLoop() && !(hidKeysDown() & KEY_START)) {
        hidScanInput();
    }
    exit(errcode);
}

int main(void)
{
    gfxInitDefault();

    consoleInit(GFX_TOP, &c_default);
    consoleSetWindow(&c_default, 0, 0, 60, 30);

    consoleInit(GFX_BOTTOM, &c_info);
    consoleSetWindow(&c_info, 0, 0, 40, 30);

    consoleSelect(&c_default);

    CFL_DB db;
    Result res        = 0;
    Mii *miis         = NULL;
    int mii_count     = 0;
    char **miistrings = NULL;

    res = cfldb_open(&db);
    if (R_FAILED(res)) {
        hang("Failed to open CFL_DB.dat", res);
    }

    res = cfldb_read(&db);
    if (R_FAILED(res)) {
        hang("Failed to read CFL_DB.dat", res);
    }

    hidScanInput();
    if (hidKeysDown() & KEY_X) {
        bool choice = prompt("Dump database to file?", "yes", "no");

        if (choice) {
            char path[40]        = {'\0'};
            const time_t curtime = time(NULL);
            struct tm *date      = gmtime(&curtime);
            strftime(path, 40, "/%F_%H-%M-%S_CFL_DB.dat", date);

            printf("Dumping datebase to:\n  sdmc:%s...\n", path);
            res = cfldb_dump_to_sdmc(&db, path);
            if (R_FAILED(res)) {
                cfldb_close(&db);
                hang("Failed to dump database", res);
            }
        }
    }

    miis      = cfldb_get_mii_array(&db);
    mii_count = cfldb_get_last_mii_index(&db) + 1;

    if (miis == NULL || mii_count <= 0) {
        cfldb_close(&db);
        hang("Failed to find Mii in database",
             MAKERESULT(RL_INFO, RS_NOTFOUND, RM_APPLICATION, RD_NOT_FOUND));
    }

    miistrings = malloc(mii_count * sizeof(miistrings[0]));

    for (int i = 0; i < mii_count; i++) {
        u8 utf8name[20];
        u8 utf8author[20];

        if (!mii_is_valid(&miis[i])) {
            miistrings[i] = NULL;
            continue;
        }

        memset(utf8name, '\0', 20);
        memset(utf8author, '\0', 20);

        utf16_to_utf8(utf8name, miis[i].name, 10);
        utf16_to_utf8(utf8author, miis[i].author, 10);

        miistrings[i] = malloc(80 * sizeof(char));

        snprintf(miistrings[i],
                 80,
                 COLORIZE(36, "%s") " by " COLORIZE(35, "%s"),
                 utf8name,
                 utf8author);
    }

    print_usage();
    u8 index = 0;
    print_mii_special_list(miis, miistrings, mii_count, index);

    while (aptMainLoop()) {

        gspWaitForVBlank();
        gfxSwapBuffers();

        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) {
            break;
        }

        if (kDown & KEY_UP) {
            index += mii_count - 1;
            index %= mii_count;
            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        if (kDown & KEY_DOWN) {
            index++;
            index %= mii_count;
            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        if (kDown & KEY_LEFT && mii_count > PAGELEN) {
            index += PAGELEN - 1;
            index %= mii_count;
            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        if (kDown & KEY_RIGHT && mii_count > PAGELEN) {
            index += PAGELEN;
            index %= mii_count;
            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        if (kDown & KEY_A) {
            Mii *cur_mii = &miis[index];

            MII_SPECIALNESS specialness = mii_get_special(cur_mii);
            if (index == 0 && specialness == MII_NONSPECIAL) {
                bool choice =
                    prompt("Do you really want to make your personal\n"
                           "Mii special? This is reported to cause issues.",
                           "Yes, I am aware that there might be issues.",
                           "No, I would rather not.");
                if (choice)
                    mii_set_special(cur_mii, !specialness);
            } else if (mii_is_valid(cur_mii)) {
                mii_set_special(cur_mii, !specialness);
            }

            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        if (kDown & KEY_SELECT) {
            bool choice =
                prompt("Do you really want to save your changes?", "yes", "no");
            if (choice) {
                printf("Writing database... ");
                cfldb_fix_checksum(&db);
                Result res = cfldb_write(&db);
                if (R_FAILED(res)) {
                    hang("Failed to write database", res);
                }
                printf("done.");
            }
            print_mii_special_list(miis, miistrings, mii_count, index);
        }

        gfxFlushBuffers();
    }

    for (int i = 0; i < mii_count; i++) {
        free(miistrings[i]);
    }
    free(miistrings);

    res = cfldb_close(&db);
    if (R_FAILED(res)) {
        hang("Failed to close CFL_DB.dat", res);
    }

    gfxExit();
    return 0;
}
