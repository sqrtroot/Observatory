#!/bin/bash 
set -e 
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"


software(){
figlet Software
# ____         __ _                          
#/ ___|  ___  / _| |___      ____ _ _ __ ___ 
#\___ \ / _ \| |_| __\ \ /\ / / _` | '__/ _ \
# ___) | (_) |  _| |_ \ V  V / (_| | | |  __/
#|____/ \___/|_|  \__| \_/\_/ \__,_|_|  \___|

apt update
apt install -y \
	cmake \
	gcc \
	qt5-default  \
	libqt5serialport5-dev \
	qtscript5-dev \
	qtmultimedia5-dev \
	qtpositioning5-dev \
	qttools5-dev \
	libdrm-dev \
	libgpiod-dev \
	gpiod \
	imagemagick \
	libxfixes-dev \
	libxi-dev \
	libev-dev
}

user(){
figlet User
#  _   _ ___  ___ _ __ 
# | | | / __|/ _ \ '__|
# | |_| \__ \  __/ |   
#  \__,_|___/\___|_|   

if ! `id -u stellarium 2>/dev/null 1>/dev/null`; then
  useradd -m -d /home/stellarium -G tty,gpio stellarium
  sudo -u stellarium mkdir -p /home/stellarium/.stellarium/
fi

cp -f $SCRIPTPATH/stellarium-config.ini /home/stellarium/.stellarium/config.ini
chown stellarium:stellarium /home/stellarium/.stellarium/config.ini
sudo -u stellarium touch /home/stellarium/.hushlogin
}


stellarium(){
figlet Stellarium
# ____  _       _ _            _                 
#/ ___|| |_ ___| | | __ _ _ __(_)_   _ _ __ ___  
#\___ \| __/ _ \ | |/ _` | '__| | | | | '_ ` _ \ 
# ___) | ||  __/ | | (_| | |  | | |_| | | | | | |
#|____/ \__\___|_|_|\__,_|_|  |_|\__,_|_| |_| |_|
                                                
sudo -u `stat -c "%U" .` bash << EOF
git submodule update --init --depth=1 --recursive -- stellarium
$SCRIPTPATH/change_nebulae.sh
cd stellarium
git apply ../stellarium.patch
mkdir -p build/unix
cd build/unix
cmake ../.. \
-DUSE_PLUGIN_TELESCOPECONTROL:BOOL="0" \
-DUSE_PLUGIN_COMPASSMARKS:BOOL="0" \
-DUSE_PLUGIN_ANGLEMEASURE:BOOL="0" \
-DUSE_PLUGIN_POINTERCOORDINATES:BOOL="0" \
-DCMAKE_BUILD_TYPE:STRING="Release" \
-DUSE_PLUGIN_OCULARS:BOOL="0" \
-DUSE_PLUGIN_EXOPLANETS:BOOL="0" \
-DUSE_PLUGIN_OCULUS:BOOL="0" \
-DUSE_PLUGIN_ARCHAEOLINES:BOOL="0" \
-DUSE_PLUGIN_TEXTUSERINTERFACE:BOOL="0" \
-DENABLE_GPS:BOOL="0" \
-DUSE_PLUGIN_EQUATIONOFTIME:BOOL="1"
make -j $(nproc)
EOF
cd stellarium/build/unix
make install
cd $SCRIPTPATH
}

unclutter(){
sudo -u `stat -c "%U" .` bash << EOF
git submodule update --init --depth=1 --recursive -- unclutter-xfixes
cd unclutter-xfixes
make unclutter
EOF
cd unclutter-xfixes
install -Dm 0755 unclutter /usr/bin/unclutter
cd $SCRIPTPATH
}

control-plugin(){
figlet Control-plugin
#                 _             _             _             _       
#  ___ ___  _ __ | |_ _ __ ___ | |      _ __ | |_   _  __ _(_)_ __  
# / __/ _ \| '_ \| __| '__/ _ \| |_____| '_ \| | | | |/ _` | | '_ \ 
#| (_| (_) | | | | |_| | | (_) | |_____| |_) | | |_| | (_| | | | | |
# \___\___/|_| |_|\__|_|  \___/|_|     | .__/|_|\__,_|\__, |_|_| |_|
#                                      |_|            |___/         

cd control-plugin/
sudo -u `stat -c "%U" .` bash << EOF
git submodule update --init --depth=1 --recursive
mkdir -p build
cd build
cmake .. \
-DCMAKE_BUILD_TYPE:STRING="Release"
make
EOF
sudo -u stellarium mkdir -p /home/stellarium/.stellarium/modules/control_plugin/
cp build/libcontrol_plugin.so /home/stellarium/.stellarium/modules/control_plugin/ 
chown stellarium:stellarium /home/stellarium/.stellarium/modules/control_plugin/libcontrol_plugin.so
}

startup(){
figlet Startup
#      _             _               
#  ___| |_ __ _ _ __| |_ _   _ _ __  
# / __| __/ _` | '__| __| | | | '_ \ 
# \__ \ || (_| | |  | |_| |_| | |_) |
# |___/\__\__,_|_|   \__|\__,_| .__/ 
#                             |_|    

cp -f $SCRIPTPATH/startup-files/stellarium.xinit /etc/X11/xinit/stellarium.xinit

cp -f $SCRIPTPATH/startup-files/stellarium.sh /home/stellarium/stellarium.sh
chown stellarium:stellarium /home/stellarium/stellarium.sh
chsh -s /home/stellarium/stellarium.sh stellarium

systemctl disable display-manager || true
systemctl set-default graphical
cat > /etc/systemd/system/getty@tty1.service.d/autologin.conf << EOF
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin stellarium --skip-login --noclear %I \$TERM
EOF

printf "" > /etc/issue
printf "" > /etc/issue.net
printf "" > /etc/motd
if ! grep -q "^disable_splash=1" /boot/config.txt; then
	echo "disable_splash=1" >> /boot/config.txt
fi
if ! grep -q logo.nologo /boot/cmdline.txt; then
	sed -i '$s/$/ logo.nologo/' /boot/cmdline.txt
fi
if ! grep -q vt.global_cursor_default=0 /boot/cmdline.txt; then
	sed -i '$s/$/ vt.global_cursor_default=0/' /boot/cmdline.txt
fi

cp -r $SCRIPTPATH/stellarium_plymouth_theme /usr/share/plymouth/themes/stellarium/
cat > /etc/plymouth/plymouthd.conf << EOF
[Daemon]
Theme=stellarium
ShowDelay=5
EOF
update-initramfs -u
}

rtc(){
figlet rtc

#      _       
# _ __| |_ ___ 
#| '__| __/ __|
#| |  | || (__ 
#|_|   \__\___|
              	
if ! grep -q "^dtparam=i2c_arm=on" /boot/config.txt; then
	echo "^dtparam=i2c_arm=on" >> /boot/config.txt
fi
if ! grep -q "^i2c-dev" /etc/modules; then
	echo i2c-dev >> /etc/modules
fi
}



all(){
	software
	user
	stellarium
  unclutter
	control-plugin
	startup
	rtc
}

if [[ $# -eq 0 ]]; then
	all
else
for argval in "$@"; do
	$argval
done
fi

