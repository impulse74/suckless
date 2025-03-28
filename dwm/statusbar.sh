#!/bin/sh

prev_status=""

while true; do
    # Get the current date and time
    datetime=$(date +"%d.%m.%Y %H:%M:%S")

    # Get volume level and mute status
    volume_info=$(amixer get Master | awk -F'[][]' '/%/ {print $2; exit}')
    if amixer get Master | grep -q '\[off\]'; then
        volume="mute"
    else
        volume="$volume_info"
    fi

    # Get network status
    network_if=$(ip route show default | awk 'NR==1 {print $5}')
    network_status=""

    if [ -z "$network_if" ]; then
        network_status=" disconnected |"
    fi

    # Get the current keyboard layout
    layout=$(setxkbmap -query | grep layout | awk '{print $2}')

    # Construct status string
    status="${network_status} $layout | vol $volume | $datetime"

    # Update only if changed
    if [ "$status" != "$prev_status" ]; then
        xsetroot -name "$status"
        prev_status="$status"
    fi

    sleep 0.5
done

