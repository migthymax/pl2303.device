// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_function_run_function.h"
#include "usbfd_version.h"

uint32 _usbfd_function_RunInterface( struct USBFDIFace *Self,struct USBFDStartupMsg *msg ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

	return USBERR_UNSUPPORTED;
}