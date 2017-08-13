# SpecializeMii

Golden pants for everyone!
Unlock any Mii for editing!

## What is this?

SpecializeMii is an application for the Nintendo 3DS that allows you to set the
*special*-Status on arbitrary Miis in your MiiMaker database.

Normally reserved only for Miis distributed by Nintendo, a *special* Mii wears
Golden Pants and triggers special events in certain applications, such as Mii
Plaza.

Additionally, this application allows for copying the System ID and the MAC address
from your Personal Mii to any other, which allows for effortlessy "stealing" Miis
received from other consoles, and reduces manual labor in using the CFL_DB
of another console on yours.
(Since FTP servers and hex editors for 3DS exist, this means it's now possible
to do the whole process without a PC. Woot!)

Finally, you can simply open this application and re-save the database to
fix the checksum, if [FixCRC](https://gbatemp.net/threads/tutorial-give-your-mii-gold-pants-and-use-it-for-streetpass.379146/page-24#post-6569186)
for personal computers is not your first choice.


## How do I use it?

Download the lastest release and then either install the `.cia` with the CIA
manager of your choice or use the `.3dsx` provided with the Homebrew Launcher.
The features are listed on-screen.

**REMINDER:** This software alters data in a Shared Extdata archive. It doesn't
really do anything harmful, but to be sure, **backup**
[archive 0xf000000b](https://www.3dbrew.org/wiki/Extdata#NAND_Shared_Extdata)
with a save file manager such as JKSM or by **holding `X` when launching the
application**. The backup will be written to `sd:/<current date>_CFL_DB.dat`.

The "Copy Ownership" feature is best enjoyed if your Personal Mii is editable.

If you are moving your Mii database to a new console, please do as follows:

1. Open Mii Maker on the target to create a legit Personal Mii, if you
   didn't already.
2. Use an extdata manager, such as JKSV, to backup shared extdata F000000B
   from both consoles.
3. Open the CFL_DB.bin from both consoles in a hex editor.
4. Copy and paste, from the new database to the one you want to import,
   the following offsets:
   * 0x0C → 0x13 (Personal Mii System ID)
   * 0x18 → 0x1D (Personal Mii MAC address)
5. Save this modified database (preferably to a new file) and import it
   into the target console. No need to import any other files from F0000000B.
6. Immediately run SpecializeMii afterwards, select every Mii to be unlocked
   and press Y on them.
7. Save changes and exit. Enjoy.


## How do I build it?

Make sure to *recursively* clone the repository (`git clone --recursive ...`)
and then build by running `make`.
