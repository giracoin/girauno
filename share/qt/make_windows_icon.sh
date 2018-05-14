#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/giracoin.png
ICON_DST=../../src/qt/res/icons/giracoin.ico
convert ${ICON_SRC} -resize 16x16 giracoin-16.png
convert ${ICON_SRC} -resize 32x32 giracoin-32.png
convert ${ICON_SRC} -resize 48x48 giracoin-48.png
convert giracoin-16.png giracoin-32.png giracoin-48.png ${ICON_DST}

