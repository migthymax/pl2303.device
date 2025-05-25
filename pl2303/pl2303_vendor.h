// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>
#include <usb/system.h>

#include "pl2303_devicebase.h"
#include "pl2303_detect_type.h"

BOOL pl2303_vendor_read( struct PL2303DeviceBase *devBase,enum pl2303_type type,uint16 value,unsigned char *buffer,uint32 size );
BOOL pl2303_vendor_write( struct PL2303DeviceBase *devBase,enum pl2303_type type,uint16 value,uint16 index );