#!/bin/sh
module="scull"
device="scull"
mode="664"

/sbin/rmmod ./$module.ko
rm -f /dev/${device}0
rm -f /dev/${device}1
rm -f /dev/${device}2
rm -f /dev/${device}3

