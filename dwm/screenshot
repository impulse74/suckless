#!/bin/bash

SCREENSHOT_DIR="$HOME/screenshots"
TIMESTAMP=$(date "+%d.%m.%Y_%T")
FILENAME="$SCREENSHOT_DIR/screenshot_$TIMESTAMP.png"

# Ensure the screenshot directory exists
mkdir -p "$SCREENSHOT_DIR"

# Determine screenshot mode
case "$1" in
    s)
        scrot -s "$FILENAME"
        ;;
    w)
        scrot -u "$FILENAME"
        ;;
    *)
        scrot "$FILENAME"
        ;;
esac

# Exit silently if scrot failed
if [ $? -ne 0 ]; then
    exit 0
fi

# Notify the user if screenshot was successful
notify-send -t 1500 "screenshot saved" "$FILENAME"

