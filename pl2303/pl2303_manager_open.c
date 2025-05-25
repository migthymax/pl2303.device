// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>
#include <usb/usb.h>

#include "pl2303_devicebase.h"
#include "pl2303_manager_open.h"
#include "pl2303_version.h"
#include "usb_cdc.h"

// We asssume that anyone might open our device witha normal IOReqquest, just for querying the newstyle command
APTR _pl2303_manager_Open( struct DeviceManagerInterface *Self,struct IORequest *ioReq, uint32 unit,uint32 flags ) {

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)Self->Data.LibBase;
	struct ExecIFace *IExec = devBase->pl2303_IExec;

	// Increment the OpenCnt to avoid to get expunge during opening the device / unit
	// Increment the open count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, OpenCnt = %ld ) Incrementing...\n",DEVNAME,__func__,__LINE__,Self,devBase->pl2303_Device.dd_Library.lib_OpenCnt );
	#endif
	uint32 opencnt = __sync_fetch_and_add( &devBase->pl2303_Device.dd_Library.lib_OpenCnt,1 );

	// We need to check that the used IORequest has at least the size of IOStdReq
	if( ioReq->io_Message.mn_Length < sizeof( struct IOExtSer ) ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Size of IO Request %ld not enough for sizeof( struct IOExtSer ) = %d  \n",DEVNAME,__func__,__LINE__,ioReq->io_Message.mn_Length,sizeof( struct IOExtSer ) );
		
		ioReq->io_Error	= IOERR_BADLENGTH;

		goto error;
	}

	struct IOExtSer *io = (struct IOExtSer *)ioReq;
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, IORequest = 0x%lx, Unit = %ld, Flags = 0x%08lx, DevBase = 0x%lx, SerialFlags = 0x%08lx\n",DEVNAME,__func__,__LINE__,Self,io,unit,flags,devBase,io->io_SerFlags );
	#endif
	
    /* Add any specific open code here
       Return 0 before incrementing OpenCnt to fail opening */

	// We only support only unit 0
	if( unit != 0 ) {
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Unit = %ld ) Unit not supported\n",DEVNAME,__func__,__LINE__,Self,unit );
		#endif
	
		/* unit number out of range or already opened */
		io->IOSer.io_Error = IOERR_OPENFAIL;

		goto error;
	}

	if( opencnt > 0 ) {
		if( ( devBase->io_SerFlags & SERF_SHARED ) != SERF_SHARED ) {

			/* unit opened exclusive */
			io->IOSer.io_Error = IOERR_UNITBUSY;

			goto error;
		}
	}

	io->IOSer.io_Unit = (struct Unit *)&devBase->pl2303_Unit;

	if( io->io_SerFlags & SERF_SHARED ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Unit = %ld Shared access 0x%08lx\n",DEVNAME,__func__,__LINE__,Self,unit,io->io_SerFlags );

		devBase->io_SerFlags |= SERF_SHARED;
	}

	// Report current line configuration like baud 8n1 etc
	io->io_CtlChar		= devBase->io_CtlChar;
    io->io_RBufLen		= devBase->io_RBufLen;
    io->io_ExtFlags		= devBase->io_ExtFlags;
    io->io_Baud			= devBase->io_Baud;	
    io->io_BrkTime		= devBase->io_BrkTime;
    io->io_TermArray	= devBase->io_TermArray;
    io->io_ReadLen		= devBase->io_ReadLen;
    io->io_WriteLen		= devBase->io_WriteLen;	
    io->io_StopBits		= devBase->io_StopBits;	
    io->io_SerFlags		= devBase->io_SerFlags;	
    io->io_Status		= devBase->io_Status;

	// mark us as having an/another opener 
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, UnitOpenCnt = %ld ) Incrementing...\n",DEVNAME,__func__,__LINE__,Self,io->IOSer.io_Unit->unit_OpenCnt );
	#endif

	__sync_add_and_fetch( &io->IOSer.io_Unit->unit_OpenCnt,1 );
	// IMPORTANT: Mark IORequest as "complete" 
	io->IOSer.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
	
	// prevent delayed expunges 
	devBase->pl2303_Device.dd_Library.lib_Flags &= ~LIBF_DELEXP;

	return devBase;	

error:

	// Increment the open count atomic, so that it we don't need Forbid()/Permit() or/and we don't care if the kernel calls us atomic
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, OpenCnt = %ld ) Decrementing...\n",DEVNAME,__func__,__LINE__,Self,devBase->pl2303_Device.dd_Library.lib_OpenCnt );
	#endif
	__sync_sub_and_fetch( &devBase->pl2303_Device.dd_Library.lib_OpenCnt,1 );

    return devBase;
}