#!/usr/bin/env bash

magick $1 -resize 64x64 64px-$2.png
magick $1 -resize 32x32 32px-$2.png
magick $1 -resize 16x16 16px-$2.png
magick $1 -resize 128x128 128px-$2.png
magick $1 -resize 256x256 256px-$2.png
