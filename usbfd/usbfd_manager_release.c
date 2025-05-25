// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_manager_release.h"
#include "usbfd_version.h"

uint32 _usbfd_manager_Release( struct LibraryManagerInterface *Self ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

	// Decrement the reference count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	return __sync_sub_and_fetch( &Self->Data.RefCount,1 );
}