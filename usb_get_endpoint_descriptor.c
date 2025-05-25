// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/usbsys.h>
#include <usb/usb.h>

#include "usb_get_endpoint_descriptor.h"
#include "usbfd/usbfd_version.h"

struct USBBusDscHead *usb_get_endpoint_descriptor( struct ExecIFace *IExec,struct USBSysIFace *IUSBSys,struct USBBusDscHead *descriptorsList,BYTE type ) {
	struct USBBusDscHead *descriptor = descriptorsList;
	for( ;descriptor != NULL;descriptor = IUSBSys->USBPrevDescriptor( descriptor ) ) {		
		IExec->DebugPrintF( "[%s] (%s@%ld) Checking Descriptor of type %d\n",LIBNAME,__func__,__LINE__,descriptor->dh_Type );
		if( descriptor->dh_Type == type ) {
			return descriptor;
		}
	}

	descriptor = descriptorsList;
	for( ;descriptor != NULL;descriptor = IUSBSys->USBNextDescriptor( descriptor ) ) {		
		IExec->DebugPrintF( "[%s] (%s@%ld) Checking Descriptor of type %d\n",LIBNAME,__func__,__LINE__,descriptor->dh_Type );
		if( descriptor->dh_Type == type ) {
			return descriptor;
		}
	}

	return NULL;
}
