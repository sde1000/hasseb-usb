#!/bin/bash

# Flash a hasseb DALI Master in USB In-System programming mode.  To
# enter this mode, short JP1 and plug the device in.

# If only one device is plugged in, it appears as a volume with ID
# usb-NXP_LPC134X_IFLASH_ISP000000000-0:0

set -e

dev=/dev/disk/by-id/usb-NXP_LPC134X_IFLASH_ISP000000000-0:0

if test -L $dev ; then

    if umount $dev ; then
	echo Unmounted $dev
    fi

    dd bs=512 seek=4 oflag=sync if=build/hasseb/firmware.cbin of=$dev

    echo Done
else
    echo Device not present
fi

