// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma	once

#include <exec/types.h>

#include "pl2303_devicebase.h"

#define CONTROL_DTR					0x01
#define CONTROL_RTS					0x02

int pl2303_set_control_lines( struct PL2303DeviceBase *devBase,uint8 value );