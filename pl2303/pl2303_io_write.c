// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>

#include "pl2303_io_write.h"
#include "pl2303_version.h"

BOOL _pl2303_io_write( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = CMD_WRITE, Data = 0x%lx, Offset = 0x%lx, Length = 0x%lx\n",DEVNAME,__func__,__LINE__,io->io_Data,io->io_Offset,io->io_Length );
	#endif

	if( io->io_Data == NULL ) {
		io->io_Error	= IOERR_BADADDRESS;

		return TRUE;
	}
	if( io->io_Length == 0 ) {
		io->io_Error	= IOERR_BADLENGTH;

		return TRUE;
	} 

	// Queue all request to worker task
	IExec->PutMsg( &devBase->pl2303_Unit.unit_MsgPort,(struct Message *)io );
	// Clear any possible QuickIO request ???
	io->io_Flags &= ~IOF_QUICK; 

	// We don't want to ReplyMsg, as we want to process the request in the background
	return FALSE;
}

BOOL _pl2303_io_write_abort( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Abort Command = CMD_WRITE, Data = 0x%lx, Offset = 0x%lx, Length = 0x%lx\n",DEVNAME,__func__,__LINE__,io->io_Data,io->io_Offset,io->io_Length );
	#endif

	BOOL result = FALSE;

	IExec->MutexObtain( devBase->pl2303_Mutex );

	struct Node *node = IExec->GetTail( (struct List *)&devBase->pl2303_writeRequests );
	if( node == (struct Node *)io ) {
		
		IExec->Remove( node );

		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) The write request is currently in 'progress', abort it\n",DEVNAME,__func__,__LINE__ );
		#endif

		IExec->AbortIO( (struct IORequest *)devBase->pl2303_writeUsbIO );
		IExec->WaitIO( (struct IORequest *)devBase->pl2303_writeUsbIO );

		result = TRUE;
	}
	else {
		// We loop through our list, to ensure that the passed in IOStdReq is really pedning at our device
		for( struct Node *node = IExec->GetHead( (struct List *)&devBase->pl2303_writeRequests );node != NULL;node = IExec->GetSucc( node ) ) {
			if ( node == (struct Node *)io ) {
				IExec->Remove( node );

				result = TRUE;

				// No return here, we must ensure that the mutex is releases
				break;
			}
		}
	}

	IExec->MutexRelease( devBase->pl2303_Mutex );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) The write request aborted successful = %d\n",DEVNAME,__func__,__LINE__,result );
	#endif

	return result;
}