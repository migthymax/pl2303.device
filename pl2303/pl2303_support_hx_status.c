// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */


#include <proto/exec.h>
#include <proto/usbsys.h>

#include "pl2303_support_hx_status.h"
#include "pl2303_vendor.h"
#include "pl2303_version.h"

#define PL2303_READ_TYPE_HX_STATUS	0x8080

BOOL pl2303_supports_hx_status( struct PL2303DeviceBase *devBase ) {

	struct ExecIFace *IExec = devBase->pl2303_IExec;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Checking if PL2303 supports HS Status...\n",DEVNAME,__func__,__LINE__ );
	#endif

	#undef SIZEOF_BUFFER
	#define SIZEOF_BUFFER 1
	unsigned char *buffer = IExec->AllocVecTags( SIZEOF_BUFFER,
		AVT_Type,			MEMF_SHARED,
		TAG_END );

	if( buffer != NULL ) {
		IExec->DebugPrintF( "[%s@%d] Out of memory, unable to allocate memory in desired amount\n",__func__,__LINE__ );	

		return FALSE;
	}

	BOOL result = pl2303_vendor_read( devBase,TYPE_H,PL2303_READ_TYPE_HX_STATUS,buffer,SIZEOF_BUFFER );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s@%d] Supports HX status = %d\n",__func__,__LINE__,result == USBERR_NOERROR );	
	#endif

	IExec->FreeVec( buffer );

	return result == USBERR_NOERROR;
}