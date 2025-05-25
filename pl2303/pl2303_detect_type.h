// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

#include "pl2303_devicebase.h"
#include "pl2303_types.h"

enum pl2303_type pl2303_detect_type( struct PL2303DeviceBase *devBase,struct USBBusDevDsc *deviceDescriptor );