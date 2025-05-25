// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>

#include "pl2303_serial_break.h"
#include "pl2303_version.h"

BOOL _pl2303_serial_break( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io ) {

	struct IOExtSer *ios = (struct IOExtSer *)io;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_BREAK\n",DEVNAME,__func__,__LINE__);
	#endif

	// Queue all request to worker task
	IExec->PutMsg( &devBase->pl2303_Unit.unit_MsgPort,(struct Message *)ios );
	// Clear any possible QuickIO request ???
	io->io_Flags &= ~IOF_QUICK; 

	// We don't want to ReplyMsg, as we want to process the request in the background
	return FALSE;
}