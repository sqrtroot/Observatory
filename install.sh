#!/usr/bin/sh
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
git submodule update --depth=1 --recursive

# ____         __ _                          
#/ ___|  ___  / _| |___      ____ _ _ __ ___ 
#\___ \ / _ \| |_| __\ \ /\ / / _` | '__/ _ \
# ___) | (_) |  _| |_ \ V  V / (_| | | |  __/
#|____/ \___/|_|  \__| \_/\_/ \__,_|_|  \___|

sudo apt update
sudo apt install stellarium cmake gcc

cd control-plugin
mkdir -p build
cd build
cmake ..
make install


#  _   _ ___  ___ _ __ 
# | | | / __|/ _ \ '__|
# | |_| \__ \  __/ |   
#  \__,_|___/\___|_|   


`id -u stellarium 2>/dev/null 1>/dev/null`
if [ $? -ne 0 ]; then
  useradd -m -d /home/stellarium -G tty stellarium
  mkdir -p /home/stellarium/.stellarium/
fi

cp -f $SCRIPTPATH/stellarium-config.ini /home/stellarium/.stellarium/config.ini
chown stellarium:stellarium /home/stellarium/.stellarium/config.ini


#      _             _               
#  ___| |_ __ _ _ __| |_ _   _ _ __  
# / __| __/ _` | '__| __| | | | '_ \ 
# \__ \ || (_| | |  | |_| |_| | |_) |
# |___/\__\__,_|_|   \__|\__,_| .__/ 
#                             |_|    

cp -f $SCRIPTPATH/startup-files/stellarium.service /lib/systemd/system/stellarium.service
cp -f $SCRIPTPATH/startup-files/stellarium.xinit /etc/X11/xinit/stellarium.xinit


systemctl disable display-manager
systemctl enable stellarium
