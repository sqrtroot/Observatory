#!/bin/bash
read -r -t 2 -N 1 input
echo "$input"
if [[ $input == "s" ]]; then
	/bin/bash
else
  if [ -d "/dev/input/by-id" ]; then
    find /dev/input/by-id -name "*mouse" -print0 2>/dev/null | grep -qz "^"
    mouse=$?
  else
    mouse=-1
  fi
  if [[ $mouse -eq 0 || "$input" = "c" ]]; then
	  /usr/bin/startx /etc/X11/xinit/stellarium.xinit -- -quiet > /dev/null 2>&1
	else
	  /usr/bin/startx /etc/X11/xinit/stellarium.xinit -- -nocursor -quiet > /dev/null 2>&1
	fi
fi