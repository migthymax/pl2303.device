// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_manager_close.h"
#include "usbfd_version.h"

APTR _usbfd_manager_Close( struct LibraryManagerInterface *Self ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

	// Decrement the reference count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	// Even see https://solie.ca/articles/map/basics/basics.html why we don do fancy Expnge stuff if 0 cnt is reached
	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, OpenCnt = %ld ) Decrementing...\n",LIBNAME,__func__,__LINE__,Self,devBase->sldb_Device.dd_Library.lib_OpenCnt );
	#endif

	__sync_sub_and_fetch( &devBase->sldb_Device.dd_Library.lib_OpenCnt,1 );

    return NULL;
}