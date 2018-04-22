#!/bin/sh

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
MAVLINKDIR=$BASEDIR/../../common/mavlink

echo $BASEDIR
export MDEF=$MAVLINKDIR/message_definitions
cd $MAVLINKDIR/pymavlink

python3 setup.py bdist_wheel
pip3 install pymavlink --no-index --find-links=dist
