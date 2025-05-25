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

