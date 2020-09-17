#!/bin/bash

if echo "pow 0" | cec-client -s -d 1 | grep on; then
  echo "as 0" | cec-client -s -d 1
else
  echo "standby 0" | cec-client -s -d 1
fi
