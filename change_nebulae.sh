#!/usr/bin/env bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
if [ ! -f $SCRIPTPATH/stellarium/nebulae/applied_saturation_filter ]; then
  echo "editing nebulae saturation"
  find $SCRIPTPATH/stellarium/nebulae/default -name "*.png" -exec convert {} -grayscale Rec709Luminance {} \;
  touch $SCRIPTPATH/stellarium/nebulae/applied_saturation_filter
  echo "done"
else
  echo "nebulae saturation already fixed"
fi
