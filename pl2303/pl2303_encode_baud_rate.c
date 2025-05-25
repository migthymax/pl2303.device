// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>
#include <interfaces/usbsys.h>
#include <usb/usb.h>

#include "pl2303/pl2303_devicebase.h"
#include "pl2303/pl2303_encode_baud_rate.h"
#include "pl2303_version.h"

static uint32 supported_bauds[] = {
	    75,
	   150,
	   300,
	   600,
	  1200,
	  1800,
	  2400,
	  3600,
	  4800,
	  7200,
	  9600,
	  14400,
	  19200,
	  28800,
	  38400,
	  57600,
	 115200,
	 230400,
	 460800,
	 614400,
	 921600,
	1228800,
	2457600,
	3000000,
	6000000,
	0
};


uint32 _pl2303_get_supported_baud_rate( struct ExecIFace *IExec,uint32 requested_baud ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Requested Baud %d \n",DEVNAME,__func__,__LINE__,requested_baud );
	#endif

	if( requested_baud < supported_bauds[ 0 ] ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Supported Baud %d \n",DEVNAME,__func__,__LINE__,supported_bauds[ 0 ] );
		#endif

		return supported_bauds[ 0 ];
	}

	for( uint32 *baud = supported_bauds;baud != 0;baud++ ) {
		if( *baud > requested_baud ) {
			baud--;

			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Supported Baud %d \n",DEVNAME,__func__,__LINE__,*baud );
			#endif

			return *baud;
		}
	}

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Supported Baud %d \n",DEVNAME,__func__,__LINE__,supported_bauds[ 24 ] );
	#endif

 	return supported_bauds[ 24 ];
}

uint32 _pl2303_encode_baud_rate_divisor_alt( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,enum pl2303_type type,uint32 baud,struct UsbCDCLineCoding *lineCoding ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Encode Baud %d for type %d with 'alternate' divisor\n",DEVNAME,__func__,__LINE__,baud,type );
	#endif

	// Apparently, for the TA version the formula is:
	//   baudrate = 12M * 32 / (mantissa * 2^exponent)
	uint32 baseline = 12000000 * 32;
	uint32 mantissa = baseline / baud;

	// Avoid dividing by zero if baud > 32*12M./
	if( mantissa == 0 )
		mantissa = 1;

	uint32 exponent = 0;
	while( mantissa >= 2048 ) {
		if( exponent < 15 ) {
			mantissa >>= 1; /* divide by 2 */
			exponent++;
		} 
		else {
			/* Exponent is maxed. Trim mantissa and leave. */
			mantissa = 2047;
			break;
		}
	}

	char *buffer = (char *)&lineCoding->uclc_dwDTERate;
	buffer[3] = 0x80;
	buffer[2] = exponent & 0x01;
	buffer[1] = (exponent & ~0x01) << 4 | mantissa >> 8;
	buffer[0] = mantissa & 0xff;

	/* Calculate and return the exact baud rate. */
	return (baseline / mantissa) >> exponent;
}

uint32 _pl2303_encode_baud_rate_divisor( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,enum pl2303_type type,uint32 baud,struct UsbCDCLineCoding *lineCoding ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Encode Baud %d for type %d with 'normal' divisor\n",DEVNAME,__func__,__LINE__,baud,type );
	#endif

	// Apparently the formula is:
	//   baudrate = 12M * 32 / (mantissa * 4^exponent)
	uint32 baseline = 12000000 * 32;
	uint32 mantissa = baseline / baud;

	// Avoid dividing by zero if baud > 32*12M./
	if( mantissa == 0 )
		mantissa = 1;

	uint32 exponent = 0;
	while( mantissa >= 512 ) {
		if( exponent < 7 ) {
			mantissa >>= 2;	/* divide by 4 */
			exponent++;
		} 
		else {
			/* Exponent is maxed. Trim mantissa and leave. */
			mantissa = 511;
			break;
		}
	}

	char *buffer = (char *)&lineCoding->uclc_dwDTERate;
	buffer[ 3 ] = 0x80;
	buffer[ 2 ] = 0;
	buffer[ 1 ] = exponent << 1 | mantissa >> 8;
	buffer[ 0 ] = mantissa & 0xff;

	/* Calculate and return the exact baud rate. */
	return (baseline / mantissa) >> (exponent << 1);
}


uint32 _pl2303_encode_baud_rate( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,enum pl2303_type type,uint32 baud,struct UsbCDCLineCoding *lineCoding ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Encode Baud %d for type %d\n",DEVNAME,__func__,__LINE__,baud,type );
	#endif

	if( pl2303_type_data[ type ].no_divisors ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) No Divisors supported just request baud\n",DEVNAME,__func__,__LINE__ );
		#endif

		lineCoding->uclc_dwDTERate = LE_LONG( baud );

		return baud;
	}

	if( pl2303_type_data[ type ].alt_divisors ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Alternate Divisors supported\n",DEVNAME,__func__,__LINE__ );
		#endif

		uint32 encoded = _pl2303_encode_baud_rate_divisor_alt( IExec,devBase,type,baud,lineCoding );

		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Alternate Divisors encodes baud %d\n",DEVNAME,__func__,__LINE__,encoded );
		#endif

		return encoded;
	}

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Divisors supported\n",DEVNAME,__func__,__LINE__ );
	#endif

	uint32 encoded = _pl2303_encode_baud_rate_divisor( IExec,devBase,type,baud,lineCoding );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Divisors encodes baud %d\n",DEVNAME,__func__,__LINE__,encoded );
	#endif

	return encoded;
}