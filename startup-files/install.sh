#!/usr/bin/sh

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

`id -u stellarium 2>/dev/null 1>/dev/null`
if [ $? -ne 0 ]; then
  useradd -m -d /home/stellarium -G tty stellarium
  mkdir -p /home/stellarium/.stellarium/
fi

cp -f $SCRIPTPATH/stellarium.service /lib/systemd/system/stellarium.service
cp -f $SCRIPTPATH/stellarium.xinit /etc/X11/xinit/stellarium.xinit
cp -f $SCRIPTPATH/../stellarium-config.ini /home/stellarium/.stellarium/config.ini
chown stellarium:stellarium /home/stellarium/.stellarium/config.ini

systemctl disable display-manager
systemctl enable stellarium
