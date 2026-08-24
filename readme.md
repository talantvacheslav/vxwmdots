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

spicetify setup
```bash
curl -fsSL https://raw.githubusercontent.com/spicetify/cli/main/install.sh | sh
#ensure pywal cache exists before linking
mkdir -p ~/.config/spicetify/Themes/pywal
ln -sf ~/.cache/wal/spicetify-color.ini ~/.config/spicetify/Themes/pywal/color.ini
ln -sf ~/.cache/wal/spicetify-user.css ~/.config/spicetify/Themes/pywal/user.css
spicetify config current_theme pywal color_scheme pywal inject_css 1 replace_colors 1
spicetify backup apply
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
gnu-free-fonts ttf-jetbrains-mono-nerd \
discord vencord ayugram spotify 

## Applications binds

`super+e` nemo

`super+t` kitty

`super+w` firefox

`super+s` ayugram

`super+d` discord

`super+a` spotify

`super+shift+s` flameshot


#### another binds in config.def.h

