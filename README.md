LIR-941R driver for Linux
--

This device driver supports LIR941R pci encoders interface.

Hardware vendor: http://www.skbis.ru/

Author: Oleg Kutkov <elenbert@gmail.com>, Crimean astrophysical observatory.

Thanks skbis for device specification and support!

Building and installation:

* Install linux kernel headers
  * For example Ubuntu/Debian/Mint: sudo apt-get install linux-headers-$(uname -r)
* Build module: 
  * make
* Install
  * sudo make install
  
For now module is able to load with command: sudo modprobe lir941r

If you wish to load this module automatically at boot time - add "lir941r" (without quotes) to the end of the /etc/modules (Ubuntu based systems) or /etc/modules.conf (Red hat based systems).

After sucessfully loading you can found new char devices in you /dev directory, each device for one encoder channel:
* /dev/lir941r-0 - for channel 1
* /dev/lir941r-1 - for channel 2
* /dev/lir941r-2 - for channel 3
* /dev/lir941r-3 - for channel 4

Please checkout this simple testing program: https://github.com/olegkutkov/lir941_test_util

Tested on Linux kernel version 3.16

If you encountered some problems - feel free to contact me.
