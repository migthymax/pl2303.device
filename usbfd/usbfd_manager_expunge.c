// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_manager_obtain.h"
#include "usbfd_version.h"

APTR _usbfd_manager_Expunge( struct LibraryManagerInterface *Self ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

    if ( devBase->sldb_Device.dd_Library.lib_OpenCnt != 0 ) {
	
		#if DebugLevel > 0
			devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) OpenCnt = 0x%lx ): Cannot expunge open count not zero\n",LIBNAME,__func__,__LINE__,devBase->sldb_Device.dd_Library.lib_OpenCnt );
		#endif

		devBase->sldb_Device.dd_Library.lib_Flags |= LIBF_DELEXP;
        
		return  NULL;
	}
	// Copy values, beacsue devBase gets freed
	struct ExecIFace *IExec = devBase->sldb_IExec;
	APTR seglist = devBase->sldb_SegmentList;

	/* Remove Library from Public */
	IExec->Remove( (struct Node *)devBase );

	/* Undo what the init code did */

	// Delete Library finally
	IExec->DeleteLibrary( (struct Library *)devBase );

	// Release IExec, undo Obtain from _manager_init
	IExec->Release();
        
	return seglist;
}