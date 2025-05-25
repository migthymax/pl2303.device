// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>
#include <proto/usbsys.h>

#include "pl2303_control_lines.h"
#include "pl2303_version.h"

#define SET_CONTROL_REQUEST_TYPE	0x21
#define SET_CONTROL_REQUEST			0x22

int pl2303_set_control_lines( struct PL2303DeviceBase *devBase,uint8 value ) {

	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	struct USBSysIFace *IUSBSys = devBase->pl2303_IUSBSys;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) bRequest = %d (0x%02x), bRequestType = 0x%02x, wValue = 0x%04x, wIndex = 0x%04x, buffer size = %d\n",DEVNAME,__func__,__LINE__,SET_CONTROL_REQUEST,SET_CONTROL_REQUEST,SET_CONTROL_REQUEST_TYPE,value,0,0 );	
	#endif

	int result = IUSBSys->USBEPControlXferA( (struct IORequest *)devBase->pl2303_UsbIO,
		IUSBSys->USBGetEndPoint( devBase->pl2303_Function,NULL,0 ),			// Endpoint	
		SET_CONTROL_REQUEST,												// Request Command		
		SET_CONTROL_REQUEST_TYPE,											// Request Type	
		value,																// Request Value	
		0,																	// Request Index		
		NULL,0,																// Data Buffer & Length	
		TAG_END );

	#if DebugLevel > 0
	if( result != USBERR_NOERROR ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed result = %d\n",DEVNAME,__func__,__LINE__,result );	
	}
	#endif

	return result;
}