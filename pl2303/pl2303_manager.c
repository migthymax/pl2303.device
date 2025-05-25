// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */


#include <interfaces/exec.h>
#include <interfaces/usbsys.h>
#include <usb/usb.h>

#include "pl2303_devicebase.h"
#include "pl2303_version.h"

struct Library *_pl2303_Init( struct PL2303DeviceBase *devBase,APTR seglist,struct ExecIFace *IExec ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) DevBase = 0x%lx, seglist = 0x%lx, IExec = 0x%lx\n",DEVNAME,__func__,__LINE__,devBase,seglist,IExec );	
	#endif

	devBase->pl2303_Device.dd_Library.lib_Node.ln_Type	= NT_DEVICE;
	devBase->pl2303_Device.dd_Library.lib_Node.ln_Pri	= DEVPRI;
	devBase->pl2303_Device.dd_Library.lib_Node.ln_Name	= DEVNAME;
	devBase->pl2303_Device.dd_Library.lib_Flags			= LIBF_SUMUSED|LIBF_CHANGED;
	devBase->pl2303_Device.dd_Library.lib_Version		= DEVVERSION;
	devBase->pl2303_Device.dd_Library.lib_Revision		= DEVREVISION;
	devBase->pl2303_Device.dd_Library.lib_IdString		= VSTRING;
	devBase->pl2303_SegmentList							= seglist;
	devBase->pl2303_IExec								= IExec;

	// Ontain IExec, so that it doesn't get Expunge as long we aren't expunged
	devBase->pl2303_IExec->Obtain();

	// Allocate a signal and make message port "live" for I/O request 
	// This functions called in the context of _usbfd_function_RunFunction, which calls 
	// IExec->CreateLibrary Pratically this cpuld ven be done in _usbfd_function_RunFunction
	IExec->NewList( &devBase->pl2303_Unit.unit_MsgPort.mp_MsgList );
	devBase->pl2303_Unit.unit_MsgPort.mp_SigTask	= IExec->FindTask( NULL );
	devBase->pl2303_Unit.unit_MsgPort.mp_SigBit		= IExec->AllocSignal( -1 );
	devBase->pl2303_Unit.unit_MsgPort.mp_Flags		= PA_SIGNAL;

	/*
	* "Normally" we would here claim the desired USB fubction configure it etc,
	* but that is all already done and handled by the usbfd_function_run_function
	*/

	return( (struct Library *)devBase );
}

