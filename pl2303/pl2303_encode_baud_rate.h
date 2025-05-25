// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/io.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "pl2303_devicebase.h"
#include "usb_cdc.h"


/**
 * Returns the nearest supported baud rate that can be set directly without
 * using divisors, if divisors is supported the requested baud rate is returned
 */
uint32 _pl2303_get_supported_baud_rate( struct ExecIFace *IExec,uint32 requested_baud );

uint32 _pl2303_encode_baud_rate( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,enum pl2303_type type,uint32 baud,struct UsbCDCLineCoding *lineCoding );