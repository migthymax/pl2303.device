// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>
#include <proto/usbsys.h>

#include "pl2303_vendor.h"
#include "pl2303_version.h"

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01
#define VENDOR_WRITE_NREQUEST		0x80

#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST			0x01
#define VENDOR_READ_NREQUEST		0x81

BOOL pl2303_vendor_read( struct PL2303DeviceBase *devBase,enum pl2303_type type,uint16 value,unsigned char *buffer,uint32 size ) {

	struct ExecIFace *IExec = devBase->pl2303_IExec;
	struct USBSysIFace *IUSBSys = devBase->pl2303_IUSBSys;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) bRequest = %d (0x%02x), bRequestType = 0x%02x, wValue = 0x%04x, wIndex = 0x%04x, buffer size = %d\n",DEVNAME,__func__,__LINE__,type == TYPE_HXN ? VENDOR_READ_NREQUEST : VENDOR_READ_REQUEST,type == TYPE_HXN ? VENDOR_READ_NREQUEST : VENDOR_READ_REQUEST,VENDOR_READ_REQUEST_TYPE,value,0,size );	
	#endif

	int result = IUSBSys->USBEPControlXferA( (struct IORequest *)devBase->pl2303_UsbIO,	
		IUSBSys->USBGetEndPoint( devBase->pl2303_Function,NULL,0 ),			// Endpoint
		type == TYPE_HXN ? VENDOR_READ_NREQUEST : VENDOR_READ_REQUEST,		// Request Command
		VENDOR_READ_REQUEST_TYPE,											// Request Type		
		value,																// Request Value			
		0,																	// Request Index		
		buffer,size,														// Data Buffer & Length	
		TAG_END );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Result (%d) = ",DEVNAME,__func__,__LINE__,result );	
		for( int i = 0; i < size; i++ ) {
			IExec->DebugPrintF( "0x%02x ",((char *)buffer)[ i ] );
		}
		IExec->DebugPrintF( "\n" );
	#endif

	return result == USBERR_NOERROR;
}

BOOL pl2303_vendor_write( struct PL2303DeviceBase *devBase,enum pl2303_type type,uint16 value,uint16 index ) {

	struct ExecIFace 	*IExec		= devBase->pl2303_IExec;
	struct USBSysIFace 	*IUSBSys	= devBase->pl2303_IUSBSys;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) bRequest = %d (0x%02x), bRequestType = 0x%02x, wValue = 0x%04x, wIndex = 0x%04x\n",DEVNAME,__func__,__LINE__,type == TYPE_HXN ? VENDOR_WRITE_NREQUEST : VENDOR_WRITE_REQUEST,type == TYPE_HXN ? VENDOR_WRITE_NREQUEST : VENDOR_WRITE_REQUEST,VENDOR_WRITE_REQUEST_TYPE,value,index );
	#endif


	int result = IUSBSys->USBEPControlXferA( (struct IORequest *)devBase->pl2303_UsbIO,
		IUSBSys->USBGetEndPoint( devBase->pl2303_Function,NULL,0 ),			// Endpoint
		type == TYPE_HXN ? VENDOR_WRITE_NREQUEST : VENDOR_WRITE_REQUEST,	// Request Command
		VENDOR_WRITE_REQUEST_TYPE,											// Request Type	
		value,																// Request Value	
		index,																// Request Index
		NULL,0,																// Data Buffer & Length
		TAG_END );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Result (%d)\n",DEVNAME,__func__,__LINE__,result );	
	#endif

	return result == USBERR_NOERROR;
}