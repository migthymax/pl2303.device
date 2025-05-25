# pl2303.device

AmigaOS USB serial device driver for PL2303-based adapters.

## Requirements

AmigaOS 4.1 Final Edition Update 2. Is the only version tested. Might even work on older version of AmigaOS 4.

## Known issues

- Handshake isn't implemneted
- Clean isn't done correctly

## Building

Regardless if you want to build on AmigaOS or cross comile it on another system. Initial you need to check out the source. 

### on AmigaOS 4

```sh
cd pl2303.device
make 
```

### on other systems

On other system it is recommended to use a docker image to croos build.

```sh
cd pl2303.device
docker run -it --rm --name usbserial -v "${PWD}:/opt/code" -w /opt/code walkero/amigagccondocker:os4-gcc11  /bin/bash
su amidev
CROSS_COMPILE=ppc-amigaos- make
```

## Installation

After the build the following files need to copied in the appropriate directories:

```sh
COPY pl2303.usbfd TO DEVS:USB/fd/
COPY pl2303.fdclass TO DEVS:USB/fdclasses/
```

## Configuration

The `pl2303.fdclass` is a text file, which contains the USB Vendor and Product Id, so that the USB stack knows that it can use the `pl2303.usbfd` driver for that combination.

The default USB Vendor and Product Id is like this configured:

```
FUNCTIONDRIVER
NAME		"Serial PL2303 Driver"
VENDOR		0x067b
PRODUCT		0x2303
TYPE		FUNCTION
PRI			6
DRIVER		"pl2303.usbfd"
```

If you own a PL2303 dongle with different Vendor/Product Id, you can modify the `pl2303.fdclass` to match your dongle and see if this driver can handle your variant.

The [Linux kernel sources](https://github.com/torvalds/linux/blob/master/drivers/usb/serial/pl2303.h) has an overview of possible USB Vendor and Product Ids for PL2303 based adapters, which are theoretically supported but not tested.

## How to use it

After plugging your PL2303 adpater into your machine this driver will register a `pl2303.device` at the system, which more or less behaves like the classic `serial.device`. 
If your serial program allows you to choose which device to use for cimmunciation just select `pl2303.device` and Unit `0`. Other units than `0` aren't supported.

### My serial program is hard coded to serial.device

In that case you can add an entry to the `DEVS:NSDPatch.cfg` which will redirect access from the the `serial.device` to the `pl2303.device`.

```
DEVICE serial.device UNIT 0 MAPTODEVICE pl2303.device MAPTOUNIT 0
```

An additional restart should reolve the situation with the serial program.

Of course that will hide access to the build in serial port.

### DOSDriver

To use it in a DOSDriver go to `Sys:Devs/DOSDrivers`and duplicate the `SER` driver and rename it to `PL2303` or whatever you like. Open the file in a text editor and adopt the `Device` entry to have the value `pl2303.device`.  

### QEMU

If you are running AmigaOS 4 under QEMU you can add a PL2303 USB device like this during startup:

```sh
-device usb-host,vendorid=0x067b,productid=0x2303
```

If you don't want to have the USB device attached during startup of QEMU you can attach the device in the control monitor like this:

```sh
device_add usb-host,vendorid=0x067b,productid=0x2303,id=pl2303
```

and detached like this:

```sh
device_del pl2303
```

If you have the serial debug output of the kernel redirect to the calling qemu shell you can let qemu offer the monitor via a TCP/IP port like this as a startup parameter:

```sh
-monitor telnet::45454,server,nowait
```

Thus you can access the monitor with letent like this and than perform the commands for attaching/detaching like this:

```sh
% telnet 127.0.0.1 45454
(qemu)device_add usb-host,vendorid=0x067b,productid=0x2303,id=pl2303
(qemu) device_del pl2303
```

## Contributing

Pull requests are welcome! Please open issues for bugs or feature requests.

## License

[GPL-3.0-or-later](LICENSE)