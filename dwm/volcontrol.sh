#!/bin/bash

VOLUME_STEP=5
VOLUME_CONTROL="Master"
NOTIFY_ID=9999

get_volume() {
    amixer get "$VOLUME_CONTROL" | grep -o '[0-9]*%' | head -n1 | tr -d '%'
}

send_notification() {
    local vol="$1"
    notify-send -t 1000 -h int:value:"$vol" -r "$NOTIFY_ID" "volume: $vol%"
}

case "$1" in
    up)
        amixer set "$VOLUME_CONTROL" "$VOLUME_STEP%+" > /dev/null
        send_notification "$(get_volume)"
        ;;
    down)
        amixer set "$VOLUME_CONTROL" "$VOLUME_STEP%-" > /dev/null
        send_notification "$(get_volume)"
        ;;
    toggle)
        if amixer get "$VOLUME_CONTROL" | grep -q '\[off\]'; then
            amixer set "$VOLUME_CONTROL" unmute > /dev/null
            notify-send -t 1000 "unmuted"
        else
            amixer set "$VOLUME_CONTROL" mute > /dev/null
            notify-send -t 1000 "muted"
        fi
        ;;
    *)
        echo "Usage: $0 {up|down|toggle}"
        exit 1
        ;;
esac

