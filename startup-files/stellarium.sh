#!/bin/bash
read -t 2 -N 1 input
echo $input
if [[ $input == "s" ]]; then
	/bin/bash
elif [[ $input == "c" ]]; then
	/usr/bin/startx /etc/X11/xinit/stellarium.xinit
else
	/usr/bin/startx /etc/X11/xinit/stellarium.xinit -- -nocursor -quiet 2>&1 1>/dev/null
fi