#ifndef MII_H
#define MII_H

#include "common.h"

#include <3ds/types.h>

typedef enum MII_SPECIALNESS_t {
    MII_SPECIAL    = 0,
    MII_NONSPECIAL = 1,
} MII_SPECIALNESS;

typedef enum MII_COPYABLE_t {
    MII_COPYABLE_OFF = 0,
    MII_COPYABLE_ON  = 1,
} MII_COPYABLE;

/* Fuck yeah! Bitfields and compiler dependent struct padding!
 *
 * This is absolutely *NOT* portable in any way, shape or form.
 * */

typedef union MiiPosition_t {
    u8 raw;
    struct {
        u8 page : 4;
        u8 slot : 4;
    };
} MiiPosition;

typedef union MiiEyebrow_t {
    u32 raw;
    struct {
        u32 rotation : 4;
        u32 _unk_bit_4 : 1;
        u32 xspacing : 4;
        u32 ypos : 5;
        u32 _unk_bit_14_15 : 2;
        u32 style : 5;
        u32 color : 3;
        u32 xscale : 4;
        u32 yscale : 4;
    };
} MiiEyebrow;

typedef struct Mii_t {
    union {
        struct {
            u8 _unk_0x00;

            u8 copyable;

            MiiPosition position;

            u8 category;
        };
        u32 mii_id;
    };
    u32 sys_id;

    u32 _unk_0x08;

    union {
        // This unsigned 32bit integer is stored in big-endian and holds the
        // date of creation in its lower 28 bit:
        //
        // seconds since 01/01/2010 00:00:00
        //   = (date_of_creation[bit 0 .. bit 27]) * 2
        u32 date_of_creation;

        // Non special Miis have bit 31 of aforementioned big-endian word set,
        // which corresponds to bit 8 in little endian, which the 3DS uses.
        struct {
            u32 : 7;
            u32 specialness : 1;
            u32 : 24;
        };
    };

    u8 mac[6];
    u8 _pad_0x16[2];

    u16 gender : 1;
    u16 bday_month : 4;
    u16 bday_day : 5;
    u16 color : 4;
    u16 favorite : 1;
    u16 _unk_0x19 : 1;

    u16 name[10];

    u8 width;
    u8 height;

    u8 disable_sharing : 1;
    u8 face_shape : 4;
    u8 skin_color : 3;

    u8 wrinkles : 4;
    u8 makeup : 4;

    u8 hair_style;

    u8 hair_color : 3;
    u8 hair_flip : 1;
    u8 _unk_0x33 : 4;

    u32 _unk_0x34;

    MiiEyebrow eyebrow;

    u8 _unk_0x3c[12];

    u16 author[10];
} Mii;

_assert_offset(Mii, mii_id, 0x00);
_assert_offset(Mii, copyable, 0x01);
_assert_offset(Mii, position.raw, 0x02);
_assert_offset(Mii, category, 0x03);
_assert_offset(Mii, sys_id, 0x04);
_assert_offset(Mii, date_of_creation, 0x0c);
_assert_offset(Mii, mac, 0x10);
_assert_offset(Mii, name, 0x1a);
_assert_offset(Mii, width, 0x2e);
_assert_offset(Mii, height, 0x2f);
_assert_offset(Mii, hair_style, 0x32);
_assert_offset(Mii, eyebrow.raw, 0x38);
_assert_offset(Mii, author, 0x48);

_Static_assert(sizeof(Mii) == 0x5c, "Mii is of wrong size!\n");

MII_COPYABLE mii_is_copyable(Mii *mii);
void mii_set_copyable(Mii *mii, MII_COPYABLE copyable);

MII_SPECIALNESS mii_get_special(Mii *mii);
void mii_set_special(Mii *mii, MII_SPECIALNESS special);

bool mii_is_shareable(Mii *mii);
void mii_set_shareable(Mii *mii, bool shareable);

bool mii_is_valid(Mii *mii);

#endif /* ----- #ifndef MII_H ----- */
