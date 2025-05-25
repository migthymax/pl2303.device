// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/devices.h>
#include <exec/types.h>



struct UsbDeviceBase {
	struct Device				sldb_Device;			// Device node
	UWORD						sldb_pad;

	/* now longword aligned */
	struct ExecIFace *          sldb_IExec;
	APTR                        sldb_SegmentList;	// Segment list
};