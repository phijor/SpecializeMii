# TARGET #

TARGET := 3DS
LIBRARY := 0

ifeq ($(TARGET),3DS)
    ifeq ($(strip $(DEVKITPRO)),)
        $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
    endif

    ifeq ($(strip $(DEVKITARM)),)
        $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
    endif
endif

# COMMON CONFIGURATION #

NAME := SpecializeMii

BUILD_DIR := build
OUTPUT_DIR := output
INCLUDE_DIRS :=
SOURCE_DIRS := src

EXTRA_OUTPUT_FILES :=

PORTLIBS := $(DEVKITPRO)/portlibs

LIBRARY_DIRS := $(DEVKITPRO)/libctru $(PORTLIBS)/3ds $(PORTLIBS)/armv6k
LIBRARIES := ctru

REVISION = $(shell git describe --abbrev=7 --dirty=-dirty --always)+$(shell git rev-list --count HEAD)

BUILD_FLAGS := -Wextra -O3 \
	           -DAPPLICATION_NAME=\"$(NAME)\" \
			   -DAPPLICATION_REV=\"$(REVISION)\"

RUN_FLAGS :=

VERSION_PARTS := $(subst ., ,$(shell git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0"))

VERSION_MAJOR := $(word 1, $(VERSION_PARTS))
VERSION_MINOR := $(word 2, $(VERSION_PARTS))
VERSION_MICRO := $(word 3, $(VERSION_PARTS))

REMOTE_IP := $(shell getent hosts n3ds | cut -d ' ' -f 1)

# 3DS CONFIGURATION #

TITLE := $(NAME)
DESCRIPTION := Make Miis Special
AUTHOR := phijor
PRODUCT_CODE := CTR-P-SMII
UNIQUE_ID := 0xF3D99

SYSTEM_MODE := 64MB
SYSTEM_MODE_EXT := Legacy

ICON_FLAGS :=

ROMFS_DIR := etc/romfs
BANNER_AUDIO := etc/audio.wav
BANNER_IMAGE := etc/banner.png
ICON := etc/icon.png

# INTERNAL #

include buildtools/make_base
