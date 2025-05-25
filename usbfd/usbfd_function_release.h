// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>
#include <exec/interfaces.h>
#include <interfaces/usbfd.h>

uint32 _usbfd_function_Release( struct USBFDIFace *Self );