# My vxwm dotfiles 

![Screenshot](rice.jpg)

## Installation

```bash
git clone https://github.com/talantvacheslav/vxwmdots.git
cd vxwmdots/

cp -rf config/* ~/.config/
cp -rf xinitrc ~/.xinitrc

cd vxwm/
make 
sudo make install

cd ..
cd scripts/
gcc zixclip.c -o zixclip -lX11 -lXfixes -lsqlite3 -lcrypto
```

## Usage

```bash
startx
```

## Dependencies

base-devel fontconfig freetype2 openssl sqlite \
libx11 libxinerama libxfixes libxft \
xorg-server xorg-xinit xorg-xset xorg-xinput xorg-setxkbmap xorg-xrdb \
xwallpaper xclip xdotool hsetroot \
picom rofi dunst pywal pywalfox flameshot \
fish kitty fastfetch neofetch cmatrix lavat eza nemo firefox \
gnu-free-fonts ttf-jetbrains-mono-nerd

## Applications binds

`super+e` nemo

`super+t` kitty

`super+w` firefox

`super+s` ayugram

`super+d` discord

`super+a` spotify

`super+shift+s` flameshot


#### see another binds in config.def.h

