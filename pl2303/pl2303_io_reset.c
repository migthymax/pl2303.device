// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>

#include "pl2303_io_reset.h"
#include "pl2303_version.h"

BOOL _pl2303_io_reset( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = CMD_RESET ) Don't know how to handle\n",DEVNAME,__func__,__LINE__);
	#endif

	// Queue all request to worker task
	IExec->PutMsg( &devBase->pl2303_Unit.unit_MsgPort,(struct Message *)io );
	// Clear any possible QuickIO request ???
	io->io_Flags &= ~IOF_QUICK; 

	// We don't want to ReplyMsg, as we want to process the request in the background
	return FALSE;
}