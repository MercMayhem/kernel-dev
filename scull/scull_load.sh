#!/bin/sh
module="scull"
device="scull"
MODE="664"

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
/sbin/insmod ./$module.ko $* || exit 1

# remove stale nodes
rm -f /dev/${device}[0-3]

major=`grep $device /proc/devices | cut -d " " -f1`
echo "Major is $major"

mknod "/dev/${device}0" c $major 0
mknod "/dev/${device}1" c $major 1
mknod "/dev/${device}2" c $major 2
mknod "/dev/${device}3" c $major 3

grep -q "^${SUDO_USER}:" /etc/group || echo "Group ${SUDO_USER} doesn't exists"

chgrp ${SUDO_USER} /dev/${device}[0-3]
chmod ${MODE} /dev/${device}[0-3]
