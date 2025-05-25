// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>

#include "usbfd_devicebase.h"
#include "usbfd_function_get_attrs.h"
#include "usbfd_version.h"

#include <stdarg.h>

void _usbfd_function_GetAttrsA( struct USBFDIFace *Self,struct TagItem *taglist ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

	/* We don't support any tags - so we simply return */
}

void VARARGS68K _usbfd_function_GetAttrs( struct USBFDIFace *Self,... ) {

	struct UsbDeviceBase *devBase = (struct UsbDeviceBase *)Self->Data.LibBase;

	#if DebugLevel > 0
		devBase->sldb_IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,devBase );
	#endif

	va_list ap;
	va_startlinear( ap,Self );
    struct TagItem *tags = va_getlinearva( ap,struct TagItem* );

    Self->USBFDGetAttrsA( tags );
}