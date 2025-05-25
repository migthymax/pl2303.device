// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>

#include <usb/usb.h>
#include <usb/system.h>

#include "pl2303_detect_type.h"
#include "pl2303_support_hx_status.h"
#include "pl2303_version.h"

enum pl2303_type pl2303_detect_type( struct PL2303DeviceBase *devBase,struct USBBusDevDsc *deviceDescriptor ) {

	struct ExecIFace *IExec = devBase->pl2303_IExec;
	
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Detecting PL2303 type, DeviceClass = 0x%02x, MaxPacketSize = 0x%02x, UsbVersion = 0x%03x, DeviceVersion = 0x%03x, Supports HX Status = %d\n",DEVNAME,__func__,__LINE__,deviceDescriptor->dd_Class,deviceDescriptor->dd_MaxPacketSize0,LE_WORD( deviceDescriptor->dd_USBVer ),LE_WORD( deviceDescriptor->dd_DevVer ),pl2303_supports_hx_status( devBase ) );	
	#endif

	// Legacy PL2303H, variants 0 and 1 (difference unknown).
	if( deviceDescriptor->dd_Class == 0x02 ) {
		return TYPE_H; // Variant 0
	}

	if( deviceDescriptor->dd_MaxPacketSize0 != 0x40 ) {
		if( deviceDescriptor->dd_Class == 0x00 || deviceDescriptor->dd_Class == 0xFF ) {
			return TYPE_H; // Variant 1
		}

		return TYPE_H; // Variant 0
	}

	switch( LE_WORD( deviceDescriptor->dd_USBVer ) ) {
		case 0x101: // USB 1.0.1? Let's assume they meant 1.1...
			// fallthrough
		case 0x110:
			switch( LE_WORD( deviceDescriptor->dd_DevVer )  ) {
				case 0x300:
					return TYPE_HX;
				case 0x400:
					return TYPE_HXD;
				default:
					return TYPE_HX;
			}
			break;
		case 0x200:
			switch( LE_WORD( deviceDescriptor->dd_DevVer )  ) {
				case 0x100:	/* GC */
				case 0x105:
					return TYPE_HXN;
				case 0x300:	/* GT / TA */
					if( pl2303_supports_hx_status( devBase ) )
						return TYPE_TA;
					// fallthrough
				case 0x305:
				case 0x400:	/* GL */
				case 0x405:
					return TYPE_HXN;
				case 0x500:	/* GE / TB */
					if( pl2303_supports_hx_status( devBase ) )
						return TYPE_TB;
						// fallthrough
				case 0x505:
				case 0x600:	/* GS */
				case 0x605:
				case 0x700:	/* GR */
				case 0x705:
					return TYPE_HXN;
				}
			break;
	}

	IExec->DebugPrintF( "[%s] [%s@%d] Couldn't detect PL2303 type, DeviceClass = 0x%02x, MaxPacketSize = 0x%02x, UsbVersion = 0x%03x, DeviceVersion = 0x%03x, Supports HX Status = %d\n",DEVNAME,__func__,__LINE__,deviceDescriptor->dd_Class,deviceDescriptor->dd_MaxPacketSize0,LE_WORD( deviceDescriptor->dd_USBVer ),LE_WORD( deviceDescriptor->dd_DevVer ),pl2303_supports_hx_status( devBase ) );	

	return UNKNOWN;
}