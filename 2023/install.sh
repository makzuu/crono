#!/usr/bin/bash

set -xe

DEST_EX=/usr/local/bin
DEST_FONT=/usr/local/share/fonts/truetype

mkdir -p $DEST_EX
cp crono $DEST_EX
mkdir -p $DEST_FONT
cp fonts/Minecraft.ttf $DEST_FONT
