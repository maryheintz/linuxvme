#!/bin/sh

DEPMOD=`which depmod`

install -m 700 usbcanII/usbcanII.ko /lib/modules/`uname -r`/kernel/drivers/usb/misc/
install -m 700 usbcanII/usbcanII.sh /usr/sbin/
install -m 777 usbcanII/usbcanII /etc/hotplug/usb/
install -m 644 usbcanII/usbcanII.usermap /etc/hotplug/usbcanII.usermap
install -m 644 10-kvaser.rules /etc/udev/rules.d 
udevcontrol reload_rules

$DEPMOD -a
if [ "$?" -ne 0 ] ; then
    echo Failed to execute $DEPMOD -a
fi
