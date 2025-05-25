// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <usb/system.h>

#include "usb_cdc.h"
#include "pl2303_detect_type.h"

int pl2303_get_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding );
int pl2303_set_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding );

void pl2303_debug_print_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding );