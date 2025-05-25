// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_manager_open.h"
#include "usbfd_version.h"

struct UsbDeviceBase *_usbfd_manager_Open( struct LibraryManagerInterface *Self,uint32 version ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, Version = 0x%08lx\n",LIBNAME,__func__,__LINE__,Self,version );
	#endif

	// Increment the OpenCnt to avoid to get expunge during opening the device / unit
	// Increment the open count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, OpenCnt = %ld ) Incrementing...\n",LIBNAME,__func__,__LINE__,Self,devBase->sldb_Device.dd_Library.lib_OpenCnt );
	#endif

	__sync_add_and_fetch( &devBase->sldb_Device.dd_Library.lib_OpenCnt,1 );

	return devBase;	
}