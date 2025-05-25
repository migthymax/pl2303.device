// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "pl2303_devicebase.h"
#include "pl2303_manager_expunge.h"
#include "pl2303_version.h"

APTR _pl2303_manager_Expunge( struct DeviceManagerInterface *Self ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->pl2303_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",DEVNAME,__func__,__LINE__,Self,devBase );
	#endif

    if ( devBase->pl2303_Device.dd_Library.lib_OpenCnt != 0 ) {
	
		#if DebugLevel > 0
			devBase->pl2303_IExec->DebugPrintF( "[%s] (%s@%ld) OpenCnt = 0x%lx ): Cannot expunge open count not zero\n",DEVNAME,__func__,__LINE__,devBase->pl2303_Device.dd_Library.lib_OpenCnt );
		#endif

		devBase->pl2303_Device.dd_Library.lib_Flags |= LIBF_DELEXP;
        
		return  NULL;
	}
	// Copy values, beacuse devBase gets freed
	struct ExecIFace *IExec = devBase->pl2303_IExec;
	APTR seglist = devBase->pl2303_SegmentList;

	/* Remove Library from Public */
	IExec->Remove( (struct Node *)devBase );

	/* Undo what the init code did */

	// Delete Library finally
	IExec->DeleteLibrary( (struct Library *)devBase );

	// Release IExec, undo Obtain from _manager_init
	IExec->Release();

	// One major issue is that we don't know who expunge us:
	// - The OS beacsue of memroy low, in that case the USB FD must be notified that the device is about vanish
	// - Via the USB Stack which notified our USB FD that it got dettached
	// That's taken care by the signaling to the USB FD Task
	IExec->Signal( devBase->pl2303_USBFDTask,SIGF_SINGLE );
	
	return seglist;
}