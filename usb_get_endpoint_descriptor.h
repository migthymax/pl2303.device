// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>
#include <interfaces/exec.h>
#include <interfaces/usbsys.h>
#include <usb/system.h>

struct USBBusDscHead *usb_get_endpoint_descriptor( struct ExecIFace *IExec,struct USBSysIFace *IUSBSys,struct USBBusDscHead *descriptorsList,BYTE type );
