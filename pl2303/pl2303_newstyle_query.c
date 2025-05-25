// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <devices/newstyle.h>
#include <devices/serial.h>

#include "pl2303_newstyle_query.h"
#include "pl2303_version.h"

/* NewStyle Supported Comamnds */
static const uint16 supported_command[] = {
	NSCMD_DEVICEQUERY,
	CMD_CLEAR,
	CMD_FLUSH,
	CMD_READ,
	CMD_START,
	CMD_STOP,
	CMD_WRITE,
	SDCMD_BREAK,
	SDCMD_QUERY,
	SDCMD_SETPARAMS,
	0
};

BOOL _pl2303_newstyle_query( struct ExecIFace *IExec,struct IOStdReq *io ) {
	
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Command = NSCMD_DEVICEQUERY\n",DEVNAME,__func__,__LINE__);
	#endif

	// We need to check that the used IORequest has at least the size of IOStdReq
	if( io->io_Message.mn_Length < sizeof( struct IOStdReq ) ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Size of IO Request %ld not enough for sizeof( struct IOStdReq ) = %d  \n",DEVNAME,__func__,__LINE__,io->io_Message.mn_Length,sizeof( struct IOStdReq ) );
		io->io_Error	= IOERR_BADLENGTH;

		return TRUE;
	}

	if( io->io_Data == NULL ) {
		io->io_Error	= IOERR_BADADDRESS;

		return TRUE;
	}
	if( io->io_Length < sizeof(struct NSDeviceQueryResult) ) {
		io->io_Error	= IOERR_BADLENGTH;

		return TRUE;
	} 

	struct NSDeviceQueryResult *deviceQueryResults = (struct NSDeviceQueryResult *)io->io_Data;
	deviceQueryResults->DevQueryFormat		= 0;
	deviceQueryResults->SizeAvailable		= sizeof(struct NSDeviceQueryResult);
	deviceQueryResults->DeviceType			= NSDEVTYPE_SERIAL;
	deviceQueryResults->DeviceSubType		= 0;
	deviceQueryResults->SupportedCommands	= (uint16 *)supported_command;

	io->io_Actual	= deviceQueryResults->SizeAvailable;
	io->io_Error	= IOERR_SUCCESS;

	// Tell to Reply to Message
	return TRUE;
}