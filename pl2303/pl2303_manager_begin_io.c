// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>
#include <proto/exec.h>

#include "pl2303_devicebase.h"
#include "pl2303_io_clear.h"
#include "pl2303_io_flush.h"
#include "pl2303_io_read.h"
#include "pl2303_io_reset.h"
#include "pl2303_io_start.h"
#include "pl2303_io_stop.h"
#include "pl2303_io_write.h"
#include "pl2303_manager_begin_io.h"
#include "pl2303_newstyle_query.h"
#include "pl2303_serial_break.h"
#include "pl2303_serial_query.h"
#include "pl2303_serial_setparams.h"
#include "pl2303_version.h"

/* starts all incoming io.  The IO is either queued up for the
   unit task or processed immediately.

   often is given the responsibility of making devices single
   threaded... so two tasks sending commands at the same time don't cause
   a problem.  Once this has been done, the command is dispatched via
   perform_io().

   There are many ways to do the threading.  This example uses the
   UNITF_ACTIVE flag.  Be sure this is good enough for your device before
   using!  Any method is ok.  If immediate access can not be obtained, the
   request is queued for later processing.

   Some IO requests do not need single threading, these can be performed
   immediately.

   IMPORTANT:
     The exec WaitIO() function uses the IORequest node type (ln_Type)
     as a flag.	If set to NT_MESSAGE, it assumes the request is
     still pending and will wait.  If set to NT_REPLYMSG, it assumes the
     request is finished.  It's the responsibility of the device driver
     to set the node type to NT_MESSAGE before returning to the user. */

VOID _pl2303_manager_BeginIO( struct DeviceManagerInterface *Self,struct IOStdReq *io ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;
	struct ExecIFace *IExec = devBase->pl2303_IExec;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, DevBase = 0x%lx\n",DEVNAME,__func__,__LINE__,Self,devBase );
	#endif

	// Set Type to NT_MESSAGE to indicate that the request is pending
	io->io_Message.mn_Node.ln_Type = NT_MESSAGE;

	switch( io->io_Command ) {
		case NSCMD_DEVICEQUERY:
			if( ! _pl2303_newstyle_query( IExec,io ) )
				return;

			break;

		case CMD_CLEAR:
			if( ! _pl2303_io_clear( IExec,devBase,io ) )
				return;

			break;

		case CMD_FLUSH:
			if( ! _pl2303_io_flush( IExec,devBase,io ) )
				return;
			
			break;

		case CMD_RESET:
			if( ! _pl2303_io_flush( IExec,devBase,io ) )
				return;
			
			break;

		case CMD_START:
			if( ! _pl2303_io_start( IExec,devBase,io ) )
				return;

			break;

		case CMD_STOP:
			if( ! _pl2303_io_stop( IExec,devBase,io ) )
				return;

			break;

		case SDCMD_BREAK:
			if( ! _pl2303_serial_break( IExec,devBase,io ) )
				return;

			break;

		case SDCMD_QUERY:
			if( ! _pl2303_serial_query( IExec,devBase,io ) )
				return;

			break;

		case SDCMD_SETPARAMS:	
			if( ! _pl2303_serial_setparams( IExec,devBase,io ) )
				return;

			break;

		case CMD_READ:
			if( ! _pl2303_io_read( IExec,devBase,io ) )
				return;

			break;

		case CMD_WRITE:			
			if( ! _pl2303_io_write( IExec,devBase,io ) )
				return;
			
			break;

		default:
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Command = 0x%lx ) Don't know how to handle\n",DEVNAME,__func__,__LINE__,io->io_Command );
			#endif

			io->io_Error = IOERR_NOCMD;

			break;
	}

	if( ! ( io->io_Flags & IOF_QUICK ) ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Command = 0x%lx,Flags = 0x%lx ) Quick Flag not set, thus ReplyMsg\n",DEVNAME,__func__,__LINE__,io->io_Command,io->io_Flags );
		#endif

		IExec->ReplyMsg( (struct Message *)io );
	}
}