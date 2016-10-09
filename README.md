# SpecializeMii

Golden pants for everyone!

## What is this?

SpecializeMii is an application for the Nintendo 3DS that allows you to set the
*special*-Status on abitrary Miis in your MiiMaker database.

Normally reserved only for Miis distributed by Nintendo, a *special* Mii wears
Golden Pants and triggers special events in certain applications.

## How do I use it?

Download the lastest release and then either install the `.cia` with the CIA
manager of your choice or use the `.3dsx` provided with the Homebrew Launcher.

**REMINDER:** This software alters data in a Shared Extdata archive. It doesn't
really do anything harmful, but to be sure, backup
[archive 0xf000000b](https://www.3dbrew.org/wiki/Extdata#NAND_Shared_Extdata)
with save file manager such as JKSM.

## How do I build it?

Make sure to *recursively* clone the repository (`git clone --recursive ...`)
and then build with by running `make`.
