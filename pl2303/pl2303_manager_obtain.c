// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "pl2303_devicebase.h"
#include "pl2303_manager_obtain.h"
#include "pl2303_version.h"

uint32 _pl2303_manager_Obtain( struct DeviceManagerInterface *Self ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->pl2303_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",DEVNAME,__func__,__LINE__,Self,devBase );
	#endif

	// Increment the reference count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	return __sync_add_and_fetch( &Self->Data.RefCount,1 );
}