// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <proto/exec.h>

#include "pl2303_devicebase.h"
#include "pl2303_io_read.h"
#include "pl2303_io_write.h"
#include "pl2303_manager_abort_io.h"
#include "pl2303_version.h"

/* Is a REQUEST to "hurry up" processing of an IORequest.
   If the IORequest was already complete, nothing happens (if an IORequest
   is quick or ln_Type=NT_REPLYMSG, the IORequest is complete).
   The message must be replied with ReplyMsg(), as normal. */
VOID _pl2303_manager_AbortIO( struct DeviceManagerInterface *Self,struct IOStdReq *io ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;
	struct ExecIFace *IExec = devBase->pl2303_IExec;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",DEVNAME,__func__,__LINE__,Self,devBase );
	#endif

	switch( io->io_Command ) {

		case CMD_READ:
			if( ! _pl2303_io_read_abort( IExec,devBase,io ) ) {

				io->io_Error = IOERR_ABORTED;
			}

			break;

		case CMD_WRITE:			
			if( ! _pl2303_io_write_abort( IExec,devBase,io ) ) {

				io->io_Error = IOERR_ABORTED;
			}

			break;

		default:
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Command = 0x%lx ) Cannot be aborted!\n",DEVNAME,__func__,__LINE__,io->io_Command );
			#endif

			io->io_Error = IOERR_NOCMD;

			break;
	}

	IExec->ReplyMsg( (struct Message *)io );
}