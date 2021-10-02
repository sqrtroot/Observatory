#!/bin/bash
read -r -t 2 -N 1 input
echo "$input"
if [[ $input == "c" ]]; then
	/bin/bash
elif [[ $input == "s" ]]; then
	stty rows 50 cols 512
	printf "\n\n\n\n\n\n" > /tmp/support
	figlet "Support Mode" >> /tmp/support
	sed -i s/^/\\t\\t\\t\\t/g /tmp/support
	cat /tmp/support
	figlet "Checking for wifi" > /tmp/support
	sed -i s/^/\\t\\t\\t\\t/g /tmp/support
	cat /tmp/support
	while ! ping -c 1 1.1.1.1 1>/dev/null 2>&1; do
		figlet "Waiting for wifi" > /tmp/support
		sed -i s/^/\\t\\t\\t\\t/g /tmp/support
		cat /tmp/support
	done
	ssh -f -N -T -R 2210:localhost:22 support@support.sqrtroot.com
	if [[ $? -ne 0 ]]; then
		figlet error > /tmp/support
		sed -i s/^/\\t\\t\\t\\t/g /tmp/support
		cat /tmp/support
		while true; do
			sleep 10;
		done
	fi
	sleep 10
  	/usr/bin/startx /etc/X11/xinit/stellarium.xinit 
else
  if [ -d "/dev/input/by-id" ]; then
    find /dev/input/by-id -name "*mouse" -print0 2>/dev/null | grep -qz "^"
    export mouse=$?
  else
    export mouse=-1
  fi
  /usr/bin/startx /etc/X11/xinit/stellarium.xinit -- -quiet > /dev/null 2>&1
fi
