// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>
#include <proto/usbsys.h>

#include "pl2303_is_hxd_clone.h"
#include "pl2303_line_request.h"
#include "pl2303_version.h"

BOOL pl2303_is_hxd_clone( struct PL2303DeviceBase *devBase ) {

	struct ExecIFace *IExec	= devBase->pl2303_IExec;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Cheking if PL2303 is a HXD clone..\n",DEVNAME,__func__,__LINE__);
	#endif

	struct UsbCDCLineCoding *lineCoding = (struct UsbCDCLineCoding *)IExec->AllocVecTags( sizeof(struct UsbCDCLineCoding),
		AVT_Type,			MEMF_SHARED,
		TAG_END );

	if( lineCoding == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Out of memory, unable to allocate memory in desired amount\n",DEVNAME,__func__,__LINE__);	

		return FALSE;
	}

	int result = pl2303_get_line_request( devBase,lineCoding );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Detected HXD clone = %d\n",DEVNAME,__func__,__LINE__,result == USBERR_STALL );	
	#endif

	IExec->FreeVec( lineCoding );

	return result == USBERR_STALL;
}