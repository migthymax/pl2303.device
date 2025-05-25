// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>
#include <usb/usb.h>
#include <usb/system.h>

#include "pl2303_control_lines.h"
#include "pl2303_initialze.h"
#include "pl2303_is_hxd_clone.h"
#include "pl2303_vendor.h"
#include "pl2303_version.h"

uint16 initseq[] = {
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 1,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_HOSTTODEV, 0x0000, 1,
	USBSDT_DIR_HOSTTODEV, 0x0001, 0,
	USBSDT_DIR_HOSTTODEV, 0x0002, 0x44,
	0xffff
};

uint16 initseq_legacy[] = {
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 1,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_HOSTTODEV, 0x0000, 1,
	USBSDT_DIR_HOSTTODEV, 0x0001, 0,
	USBSDT_DIR_HOSTTODEV, 0x0002, 0x24,
	0xffff
};

uint16 inithxs[] = {
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_HOSTTODEV, 0x0404, 1,
	USBSDT_DIR_DEVTOHOST, 0x8484, 0,
	USBSDT_DIR_DEVTOHOST, 0x8383, 0,
	USBSDT_DIR_HOSTTODEV, 0x0000, 1,
	USBSDT_DIR_HOSTTODEV, 0x0002, 0x44,
	USBSDT_DIR_HOSTTODEV, 0x0008, 0,		// These are extra in aros
	USBSDT_DIR_HOSTTODEV, 0x0009, 0,		// These are extra in aros
	0xffff
};

BOOL pl2303_initialze_chip( struct PL2303DeviceBase *devBase ) {
	
	struct ExecIFace *IExec	= devBase->pl2303_IExec;
	const enum pl2303_type type = devBase->pl2303_Type;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Initialze Chip of type = %d\n",DEVNAME,__func__,__LINE__,type );
	#endif

	#undef SIZEOF_BUFFER
	#define SIZEOF_BUFFER 1
	unsigned char *buffer = IExec->AllocVecTags( SIZEOF_BUFFER,
		AVT_Type,			MEMF_SHARED,
		TAG_END );

	if( buffer == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Out of memory, unable to allocate memory in desired amount\n",DEVNAME,__func__,__LINE__);	

		return FALSE;
	}

	if( type != TYPE_HX ) {
		uint16 *initsequnecestep = initseq;

		if( ( devBase->pl2303_Quirks & PL2303_QUIRK_LEGACY ) == PL2303_QUIRK_LEGACY ) {
			IExec->DebugPrintF( "[%s] (%s@%ld) Enabled quirk 'LEGACY SETUP'\n",DEVNAME,__func__,__LINE__);	

			initsequnecestep = initseq_legacy;
		}

		if( type == TYPE_HXD && pl2303_is_hxd_clone( devBase ) ) {
			IExec->DebugPrintF( "[%s] (%s@%ld) Detected HXD clone, enabling quirk 'NO BREAK GETLINE'\n",DEVNAME,__func__,__LINE__);	

			devBase->pl2303_Quirks |= PL2303_QUIRK_NO_BREAK_GETLINE;
		}

		pl2303_set_control_lines( devBase,0x03 );  // TODO; ????

		/* Output init sequence */
		while( *initsequnecestep != 0xffff ) {
			uint16 direction = *initsequnecestep++;
			uint16 value = *initsequnecestep++;
			uint16 index = *initsequnecestep++;


			if( direction == USBSDT_DIR_DEVTOHOST ) {
				if( ! pl2303_vendor_read( devBase,type,value,buffer,SIZEOF_BUFFER ) ) {
					IExec->DebugPrintF( "[%s] (%s@%ld) Vendor read failed for value 0x%04x!\n",DEVNAME,__func__,__LINE__,value );	

					IExec->FreeVec( buffer );

					return FALSE;
				}
			}
			else { // USBSDT_DIR_HOSTTODEV
				if( ! pl2303_vendor_write( devBase,type,value,index ) ) {
					IExec->DebugPrintF( "[%s] (%s@%ld) Vendor write failed for value 0x%04x, index = 0x%04x!\n",DEVNAME,__func__,__LINE__,value,index );	

					IExec->FreeVec( buffer );

					return FALSE;
				}

			}
		}


	}

	IExec->FreeVec( buffer );

	return TRUE;
}