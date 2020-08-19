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
apt install -y stellarium cmake gcc qt5-default  libqt5serialport5-dev qtscript5-dev qtmultimedia5-dev qtpositioning5-dev qttools5-dev libdrm-dev
}

user(){
figlet User
#  _   _ ___  ___ _ __ 
# | | | / __|/ _ \ '__|
# | |_| \__ \  __/ |   
#  \__,_|___/\___|_|   

if ! `id -u stellarium 2>/dev/null 1>/dev/null`; then
  useradd -m -d /home/stellarium -G tty stellarium
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
git submodule update --depth=1 --recursive
mkdir -p stellarium/build/unix
cd stellarium/build/unix
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
git submodule update --depth=1 --recursive
mkdir -p build
cd build
cmake ..
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
systemctl enable stellarium
systemctl set-default graphical
cat > /etc/systemd/system/getty@tty1.service.d/autologin.conf << EOF
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin stellarium --skip-login --noclear %I \$TERM
EOF
echo Stellar > /etc/issue
echo Stellar > /etc/issue.net
echo > /etc/motd
if ! grep disable_splash=1 /boot/config.txt; then
	echo "disable_splash=1" >> /boot/config.txt
fi
if ! grep logo.nologo /boot/cmdline.txt; then
	sed -i '$s/$/ logo.nologo/' /boot/cmdline.txt
fi
}

all(){
	software
	user
	stellarium
	control-plugin
	startup
}

if [[ $# -eq 0 ]]; then
	all
else
for argval in "$@"; do
	$argval
done
fi

