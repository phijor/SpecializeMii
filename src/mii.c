#include "mii.h"

#include <3ds/types.h>

#include <stdbool.h>

MII_COPYABLE mii_is_copyable(Mii *mii)
{
    return mii->copyable;
}

void mii_set_copyable(Mii *mii, MII_COPYABLE copyable)
{
    mii->copyable = copyable;
    if (copyable == MII_COPYABLE_ON) {
        mii->specialness = MII_NONSPECIAL;
    }
}

MII_SPECIALNESS mii_get_special(Mii *mii)
{
    return mii->specialness;
}

void mii_set_special(Mii *mii, MII_SPECIALNESS special)
{
    mii->specialness = special;
    if (special == MII_SPECIAL) {
        mii->copyable = 0;
        mii->disable_sharing = 1;
    }
}
