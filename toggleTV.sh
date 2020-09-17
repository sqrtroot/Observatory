#!/bin/bash

output=`echo "pow 0" | cec-client -s -d 1`
echo $output | grep "status: on"
if [ $? -eq 0 ]; then
  echo "turning off"
  echo "standby 0" | cec-client -s -d 1
else
  echo "turning on"
  echo "as 0" | cec-client -s -d 1
fi
