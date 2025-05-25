// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>
#include <proto/usbsys.h>
#include <usb/usb.h>

#include "pl2303_line_request.h"
#include "pl2303_version.h"

#define SET_LINE_REQUEST_TYPE		0x21
#define SET_LINE_REQUEST			0x20

#define GET_LINE_REQUEST_TYPE		0xa1
#define GET_LINE_REQUEST			0x21

void pl2303_update_state_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding ) {
	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	
	devBase->io_Baud		= LE_LONG( lineCoding->uclc_dwDTERate );
	devBase->io_ReadLen		= lineCoding->uclc_bDataBits;
	devBase->io_WriteLen 	= lineCoding->uclc_bDataBits;
	switch( lineCoding->uclc_bCharFormat ) {
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) Unsupported stop bit schema with id = %d\n",DEVNAME,__func__,__LINE__,lineCoding->uclc_bCharFormat );	
		case CDC_STOP_BITS_1:
			devBase->io_StopBits = 1;
			break;
		case CDC_STOP_BITS_2:
			devBase->io_StopBits = 2;
			break;
	}
	
	switch( lineCoding->uclc_bParityType ) {
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) Unsupported parity schema with id = %d\n",DEVNAME,__func__,__LINE__,lineCoding->uclc_bParityType );	
		case CDC_PARITY_NONE:
			devBase->io_SerFlags &= ~SERF_PARTY_ODD;
			devBase->io_SerFlags &= ~SERF_PARTY_ON;
			devBase->io_ExtFlags &= ~SEXTF_MARK;
			devBase->io_ExtFlags &= ~SEXTF_MSPON;
			break;
		case CDC_PARITY_ODD:
			devBase->io_SerFlags |= SERF_PARTY_ODD;
			break;
		case CDC_PARITY_EVEN:
			devBase->io_SerFlags |= SERF_PARTY_ON;
			break;
		case CDC_PARITY_MARK:
			devBase->io_ExtFlags |= SEXTF_MARK;
			break;
		case CDC_PARITY_SPACE:
			devBase->io_ExtFlags |= SEXTF_MSPON;
			break;
	}
}

void pl2303_update_state_line_request_from_buffer( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding ) {
	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	
	lineCoding->uclc_dwDTERate	= LE_LONG( devBase->io_Baud );
	lineCoding->uclc_bDataBits	= devBase->io_ReadLen;
	switch( devBase->io_StopBits ) {
		case 1:
			lineCoding->uclc_bCharFormat = CDC_STOP_BITS_1;
			break;
		case 2:
			lineCoding->uclc_bCharFormat = CDC_STOP_BITS_2;
			break;
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) Unsupported stop bit schema with id = %d\n",DEVNAME,__func__,__LINE__,devBase->io_StopBits );	

			lineCoding->uclc_bCharFormat = CDC_STOP_BITS_1;
	}	
	
	lineCoding->uclc_bParityType = CDC_PARITY_NONE;
	if( ( devBase->io_SerFlags & SERF_PARTY_ODD ) ==  SERF_PARTY_ODD ) {
		lineCoding->uclc_bParityType = CDC_PARITY_ODD;
	} 
	else if( ( devBase->io_SerFlags & SERF_PARTY_ON ) ==  SERF_PARTY_ON ) {
		lineCoding->uclc_bParityType = CDC_PARITY_EVEN;
	}
	else if( ( devBase->io_ExtFlags & SEXTF_MARK ) ==  SEXTF_MARK ) {
		lineCoding->uclc_bParityType = CDC_PARITY_MARK;
	}
	else if( ( devBase->io_ExtFlags & SEXTF_MSPON ) ==  SEXTF_MSPON ) {
		lineCoding->uclc_bParityType = CDC_PARITY_SPACE;
	}
}

int pl2303_get_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding ) {

	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	struct USBSysIFace 	*IUSBSys = devBase->pl2303_IUSBSys;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Get the line status\n",DEVNAME,__func__,__LINE__);
	#endif

	if( ( devBase->pl2303_Quirks & PL2303_QUIRK_NO_BREAK_GETLINE ) == PL2303_QUIRK_NO_BREAK_GETLINE ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Get the line status quirk 'NO_BREAK_GETLINE' active!\n",DEVNAME,__func__,__LINE__);
		#endif

		pl2303_update_state_line_request_from_buffer( devBase,lineCoding );
		
		return USBERR_NOERROR;
	}

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) bRequest = %d (0x%02x), bRequestType = 0x%02x, wValue = 0x%04x, wIndex = 0x%04x, buffer size = %d\n",DEVNAME,__func__,__LINE__,GET_LINE_REQUEST,GET_LINE_REQUEST,GET_LINE_REQUEST_TYPE,0,0,sizeof(struct UsbCDCLineCoding) );	
	#endif

	int result = IUSBSys->USBEPControlXferA( (struct IORequest *)devBase->pl2303_UsbIO,
		IUSBSys->USBGetEndPoint( devBase->pl2303_Function,NULL,0 ),			// Endpoint	
		GET_LINE_REQUEST,													// Request Command			
		GET_LINE_REQUEST_TYPE,												// Request Type
		0,																	// Request Value		
		0,																	// Request Index	
		lineCoding,sizeof(struct UsbCDCLineCoding),							// Data Buffer & Length
		TAG_END );

	if( result == USBERR_NOERROR ) {
		pl2303_update_state_line_request( devBase,lineCoding );
	}
	#if DebugLevel > 0
	else {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed result = %d\n",DEVNAME,__func__,__LINE__,result );	
	}
	#endif

	return result;
}


int pl2303_set_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding ) {

	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	struct USBSysIFace 	*IUSBSys = devBase->pl2303_IUSBSys;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) bRequest = %d (0x%02x), bRequestType = 0x%02x, wValue = 0x%04x, wIndex = 0x%04x, buffer size = %d\n",DEVNAME,__func__,__LINE__,SET_LINE_REQUEST,SET_LINE_REQUEST,SET_LINE_REQUEST_TYPE,0,0,sizeof(struct UsbCDCLineCoding) );	
	#endif

	int result = IUSBSys->USBEPControlXferA( (struct IORequest *)devBase->pl2303_UsbIO,
		IUSBSys->USBGetEndPoint( devBase->pl2303_Function,NULL,0 ),			// Endpoint	
		SET_LINE_REQUEST,													// Request Command		
		SET_LINE_REQUEST_TYPE,												// Request Type	
		0,																	// Request Value	
		0,																	// Request Index		
		lineCoding,sizeof(struct UsbCDCLineCoding),							// Data Buffer & Length	
		TAG_END );

	
	if( result == USBERR_NOERROR ) {
		pl2303_update_state_line_request( devBase,lineCoding );
	}
	#if DebugLevel > 0
	else {
		IExec->DebugPrintF( "[%s@%d] Failed result = %d\n",__func__,__LINE__,result );	
	}
	#endif

	return result;
}


void pl2303_debug_print_line_request( struct PL2303DeviceBase *devBase,struct UsbCDCLineCoding *lineCoding ) {

	struct ExecIFace *IExec	= devBase->pl2303_IExec;

	IExec->DebugPrintF( "[%s] (%s@%ld) Current line coding:\n",DEVNAME,__func__ ,__LINE__);
	IExec->DebugPrintF( "[%s] (%s@%ld) \tBaudrate  = %ld\n",DEVNAME,__func__ ,__LINE__,LE_LONG( lineCoding->uclc_dwDTERate ) );
	switch( lineCoding->uclc_bDataBits ) {
		case CDC_DATA_BITS_5:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = 5\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_DATA_BITS_6:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = 6\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_DATA_BITS_7:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = 7\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_DATA_BITS_8:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = 8\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_DATA_BITS_16:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = 16\n",DEVNAME,__func__ ,__LINE__ );
			break;
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tData Bits = <Unkown> (%d)\n",DEVNAME,__func__ ,__LINE__,lineCoding->uclc_bDataBits );
			break;
	}
	switch( lineCoding->uclc_bParityType ) {
		case CDC_PARITY_NONE:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = None\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_PARITY_ODD:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = Odd\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_PARITY_EVEN:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = Even\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_PARITY_MARK:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = Mark\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_PARITY_SPACE:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = Space\n",DEVNAME,__func__ ,__LINE__ );
			break;
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tPartity   = <Unkown> (%d)\n",DEVNAME,__func__ ,__LINE__,lineCoding->uclc_bParityType  );
			break;
	}
	switch( lineCoding->uclc_bCharFormat ) {
		case CDC_STOP_BITS_1:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tStop Bits = 1\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_STOP_BITS_1_5:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tStop Bits = 1.5\n",DEVNAME,__func__ ,__LINE__ );
			break;
		case CDC_STOP_BITS_2:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tStop Bits = 2\n",DEVNAME,__func__ ,__LINE__ );
			break;
		default:
			IExec->DebugPrintF( "[%s] (%s@%ld) \tStop Bits = <Unkown> (%d)\n",DEVNAME,__func__ ,__LINE__,lineCoding->uclc_bCharFormat );
			break;
	}	
}