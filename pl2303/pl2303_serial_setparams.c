// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>

#include "pl2303_encode_baud_rate.h"
#include "pl2303_serial_setparams.h"
#include "pl2303_version.h"

BOOL _pl2303_serial_setparams( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io ) {

	// We need to check that the used IORequest has at least the size of IOStdReq
	if( io->io_Message.mn_Length < sizeof( struct IOExtSer ) ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Size of IO Request %ld not enough for sizeof( struct IOExtSer ) = %d  \n",DEVNAME,__func__,__LINE__,io->io_Message.mn_Length,sizeof( struct IOExtSer ) );
		
		io->io_Error	= IOERR_BADLENGTH;

		return TRUE;
	}

	struct IOExtSer *ios = (struct IOExtSer *)io;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Request baud = %d, Read Len = %d, Write Len = %d, Stop Bits = %d, Flags = 0x%lx\n",DEVNAME,__func__,__LINE__,ios->io_Baud,ios->io_ReadLen,ios->io_WriteLen,ios->io_StopBits,ios->io_SerFlags );
	#endif

	if( devBase->io_RBufLen != ios->io_RBufLen ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Changing read buffer size from = %d to %d isn't supported\n",DEVNAME,__func__,__LINE__,devBase->io_RBufLen,ios->io_RBufLen );

		/* We ignore it, beacause to be able to set baud etc
		io->io_Error	= SerErr_BufErr;

		return TRUE;
		*/
	}

	if( ios->io_ReadLen != ios->io_WriteLen ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Having different data bits for reading %d and writing  %d isn't supported\n",DEVNAME,__func__,__LINE__,ios->io_ReadLen,ios->io_WriteLen  );

		io->io_Error	= SerErr_InvParam;

		return TRUE;
	}

	if( ( ios->io_ReadLen < 5 || ios->io_ReadLen > 8 ) && ios->io_ReadLen != 16 ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, %d Data bits isn't supported\n",DEVNAME,__func__,__LINE__,ios->io_ReadLen);

		io->io_Error	= SerErr_InvParam;

		return TRUE;
	}

	if( ios->io_StopBits < 1 || ios->io_StopBits > 2 ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, %d Stop bits isn't supported\n",DEVNAME,__func__,__LINE__,ios->io_StopBits );

		io->io_Error	= SerErr_InvParam;

		return TRUE;
	}

	if( ( ios->io_SerFlags & SERF_PARTY_ON) && ( ios->io_ExtFlags & SEXTF_MSPON ) ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Having MARK/SPACE and ODD/EVEN parity enable simultaneously isn't supported\n",DEVNAME,__func__,__LINE__ );

		io->io_Error	= SerErr_InvParam;

		return TRUE;
	}

	// TODO Parity check 

	if( ( ios->io_SerFlags & SERF_7WIRE ) != 0 ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, 7-wire requested\n",DEVNAME,__func__,__LINE__ );
	}

	if( ios->io_Baud > pl2303_type_data[ devBase->pl2303_Type ].max_baud_rate ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Requested baud rate %d exceed max supported baud rate %d\n",DEVNAME,__func__,__LINE__,ios->io_Baud,pl2303_type_data[ devBase->pl2303_Type ].max_baud_rate );

		io->io_Error	= SerErr_InvParam;

		return TRUE;
	}

	if( ! pl2303_type_data[ devBase->pl2303_Type ].no_divisors ) {
		uint32 supported_baud = _pl2303_get_supported_baud_rate( IExec,ios->io_Baud );

		if( ios->io_Baud != supported_baud ) {
			IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS, Requested baud rate %d not supported neareast by supported baud rate is %d\n",DEVNAME,__func__,__LINE__,ios->io_Baud,supported_baud );

			io->io_Error	= SerErr_InvParam;

			return TRUE;	
		}
	}


	// Queue all request to worker task
	IExec->PutMsg( &devBase->pl2303_Unit.unit_MsgPort,(struct Message *)ios );
	// Clear any possible QuickIO request ???
	io->io_Flags &= ~IOF_QUICK; 

	// We don't want to ReplyMsg, as we want to process the request in the background
	return FALSE;
}