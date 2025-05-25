// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/interfaces.h>
#include <devices/serial.h>

struct UsbDeviceBase *_usbfd_manager_Open( struct LibraryManagerInterface *Self,uint32 version );