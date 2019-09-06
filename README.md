Firmware for the hasseb DALI Master
===================================

Note
----

**This is incomplete!  Don't use it!  The USB hardware is not
initialised reliably!**

Building
--------

This project is built using [platformio](https://platformio.org/).
These instructions assume you have cloned the project and the current
working directory is the root of the project.

On Linux, you can setup a virtualenv and install platformio inside it,
to avoid having to install it at system level:

```
virtualenv --system-site-packages -p python3 venv
. venv/bin/activate
pip install platformio
```

Once platformio is installed, you can build the firmware with the
command `pio run`.  The binary to be installed on the hasseb DALI
Master is `build/hasseb/firmware.cbin`.  You can remove the built
firmware and all temporary files using `pio run -t clean`.

Installing
----------

Connect the DALI Master to the host in firmware update mode, by
shorting the two pins of JP1 while plugging it in.  The device should
appear as a USB mass storage device with volume label `CRP DISABLD`.

On Linux, the most reliable way to update the firmware is to copy it
to the block device using `dd`.  The file `scripts/flash.sh` will do
this:

```
sudo ./scripts/flash.sh
```

If you are updating the firmware regularly, it may be convenient to
prevent the desktop environment auto-mounting USB mass storage
devices.  If you're using gnome, you can run "dconf Editor" and change
the `org.gnome.desktop.media-handling.automount` setting to Off.

On Windows, you may just be able to delete the existing `firmware.bin`
on the mass storage device and copy `build/hasseb/firmware.cbin` to
it.  [See this application note for more information.](https://www.nxp.com/docs/en/application-note/AN10986.pdf)

Credits
-------

The USB code in this project is inspired by
[JustHIDUSB](https://bitbucket.org/dmasyukov/justhidusb/src/master/)
and [USB Made Simple](http://www.usbmadesimple.co.uk/).
