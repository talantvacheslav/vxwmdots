#!/bin/bash

#script and rofi config is fork from https://github.com/NyxOkkotsu/vxwmdotfiles

WALL_DIR="$HOME/Pictures/Wallpapers"

mkdir -p "$WALL_DIR"

ROFI_INPUT=""
while read -r file; do
    if [ -n "$file" ]; then
        ROFI_INPUT+="${file}\0icon\x1f${WALL_DIR}/${file}\n"
    fi
done <<< "$(ls -1 "$WALL_DIR" | grep -E "\.(jpg|jpeg|png)$")"

SELECTION=$(echo -e "$ROFI_INPUT" | rofi -show-icons -dmenu -i -p "󰸉 Wallpaper" -theme ~/.config/rofi/wall-changer.rasi)

if [ -z "$SELECTION" ]; then
    exit 1
fi

WALLPAPER="$WALL_DIR/$SELECTION"

xwallpaper --stretch "$WALLPAPER"

wal -i "$WALLPAPER" -n

#comment if dont want 
hsetroot -solid "$(sed -n 's/^dwm\.selbgcolor:[[:space:]]*//p' ~/.cache/wal/dwm.Xresources)"

xrdb -merge "$HOME/.cache/wal/colors.Xresources"

xdotool key Super+F5

if [ -f "$HOME/wal-dunst.sh" ]; then
    "$HOME/wal-dunst.sh"
fi

pywalfox update

rm $HOME/.config/Vencord/themes/midnight-pywal.theme.css && ln -s $HOME/.cache/wal/vencord-midnight.css $HOME/.config/Vencord/themes/midnight-pywal.theme.css

killall spicetify
spicetify -s watch &
