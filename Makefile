# docker run -it --rm --name usbserial -v "${PWD}:/opt/code" -w /opt/code walkero/amigagccondocker:os4-gcc11  /bin/bash
# su amidev
# CROSS_COMPILE=ppc-amigaos- make ...
#
# qemu-system-ppc -L pc-bios -M pegasos2 -m 2048 -kernel bboot/bboot -initrd bboot/Kickstart_striped.zip -cpu apollo7 -vga none -device sm501 -drive if=none,id=cd -device ide-cd,drive=cd,bus=ide.1 -drive if=none,id=hd,file=hd.img,format=raw -device ide-hd,drive=hd,bus=ide.0 -drive if=none,id=sdk,file=sdk.img,format=raw -device ide-hd,drive=sdk,bus=ide.1 -device rtl8139,netdev=net0 -netdev user,id=net0 -drive file=fat:rw:/Users/maxlarsson/Public,id=ufat,format=raw,if=none -device usb-storage,drive=ufat -rtc base=localtime -monitor telnet::45454,server,nowait -serial mon:stdio -display cocoa -append "serial debuglevel=5"
# QEMU Monitor:
# > telnet 127.0.0.1 45454
# > device_add usb-host,vendorid=0x067b,productid=0x2303,id=pl2303
# > device_del pl2303


CC     = $(CROSS_COMPILE)gcc 
CXX    = $(CROSS_COMPILE)c++ 
AS     = $(CROSS_COMPILE)as 
LD     = $(CROSS_COMPILE)ld 
RANLIB = $(CROSS_COMPILE)ranlib 
RM     = rm

# Change these as required
OPTIMIZE = 
DEBUG = -gstabs -fno-omit-frame-pointer -DDebugLevel=1
CFLAGS = -I. -Wall $(OPTIMIZE) $(DEBUG)

# Flags passed to gcc during linking
LINK = -nostartfiles -N -nostdlib

# Name of the "thing" to build
TARGET = pl2303.usbfd

# Additional linker libraries
LIBS = 

# Source code files used in this project
# Add any additional files to this line

SRCS = \
	$(wildcard **/*.c) \
	$(wildcard *.c)

# -------------------------------------------------------------
# Nothing should need changing below this line

OBJS = $(SRCS:.c=.o)

# Rules for building
$(TARGET): $(OBJS)
	$(CC) $(LINK) -o $(TARGET) $(OBJS) $(LIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)

