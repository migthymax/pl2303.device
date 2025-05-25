// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>
#include <usb/system.h>
#include "pl2303_detect_type.h"

BOOL pl2303_is_hxd_clone( struct PL2303DeviceBase *devBase );