// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "pl2303_devicebase.h"
#include "pl2303_manager_close.h"
#include "pl2303_version.h"

APTR _pl2303_manager_Close( struct DeviceManagerInterface *Self,struct IOExtSer *io ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->pl2303_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",DEVNAME,__func__,__LINE__,Self,devBase );
	#endif

	// Decrement the reference count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	// Even see https://solie.ca/articles/map/basics/basics.html why we don do fancy Expnge stuff if 0 cnt is reached
	#if DebugLevel > 0
		devBase->pl2303_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, OpenCnt = %ld ) Decrementing...\n",DEVNAME,__func__,__LINE__,Self,devBase->pl2303_Device.dd_Library.lib_OpenCnt );
	#endif

	__sync_sub_and_fetch( &devBase->pl2303_Device.dd_Library.lib_OpenCnt,1 );

    return NULL;
}