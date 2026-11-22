#!/bin/bash
 
xclip &
xset r rate 250 30 &
xinput --set-prop 10 "libinput Accel Speed" -1 &
wal -R 
hsetroot -solid "$(sed -n 's/^dwm\.normbgcolor:[[:space:]]*//p' ~/.cache/wal/dwm.Xresources)" & &
dunst &
setxkbmap -layout us,ru -option 'grp:alt_shift_toggle'
picom &

vxwm 
