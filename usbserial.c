// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

// ppc-amigaos-gcc -nostartfiles -N -DDebugLevel=1 -gstabs -fno-omit-frame-pointer -o usbserial.usbfd lib_manager_obtain.c usbserial.c

#include <interfaces/exec.h>
#include <exec/types.h>
#include <dos/dos.h>

#include "usbfd/usbfd_function.h"
#include "usbfd/usbfd_manager.h"
#include "usbfd/usbfd_version.h"


static uint8 __attribute__((used))verstag[] = VERSTAG;

/*
 * The system (and compiler) rely on a symbol named _start which marks
 * the beginning of execution of an ELF file. To prevent others from
 * executing this library, and to keep the compiler/linker happy, we
 * define an empty _start symbol here.
 *
 * On the classic system (pre-AmigaOS4) this was usually done by
 * moveq #0,d0
 * rts
 */
 int32 _start( STRPTR argstring,int32 arglen,struct ExecBase *SysBase ) {
 
	#if DebugLevel > 0
		((struct ExecIFace *)(SysBase->MainInterface))->DebugPrintF( "--> _start()\n" );
 	#endif
 
	/* If you feel like it, open DOS and print something to the user */
 
	 return( RETURN_OK );
 }


/* The ROMTAG Init Function */
struct Library *libInit( struct UsbDeviceBase *libBase,APTR seglist,struct ExecIFace *IExec ) {

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) DevBase = 0x%lx, seglist = 0x%lx, IExec = 0x%lx\n",LIBNAME,__func__,__LINE__,libBase,seglist,IExec );	
	#endif

    libBase->sldb_Device.dd_Library.lib_Node.ln_Type	= NT_LIBRARY;
    libBase->sldb_Device.dd_Library.lib_Node.ln_Pri		= LIBPRI;
    libBase->sldb_Device.dd_Library.lib_Node.ln_Name	= LIBNAME;
    libBase->sldb_Device.dd_Library.lib_Flags			= LIBF_SUMUSED|LIBF_CHANGED;
    libBase->sldb_Device.dd_Library.lib_Version			= LIBVERSION;
    libBase->sldb_Device.dd_Library.lib_Revision		= LIBREVISION;
    libBase->sldb_Device.dd_Library.lib_IdString		= VSTRING;

    libBase->sldb_SegmentList							= seglist;
	libBase->sldb_IExec									= IExec;

	// Ontain IExec, so that it doesn't get Expunge as long we aren't expunged
    libBase->sldb_IExec->Obtain();

	// Setup stuff here

	return( (struct Library *)libBase );
}

static APTR libInterfaces[] = {
    UsbFDManager_Tags,
	FunctionDriver_Tags,
    NULL
};

static struct TagItem libCreateTags[] = {
    { CLT_DataSize,     (uint32)sizeof(struct UsbDeviceBase)	},
    { CLT_InitFunc,     (uint32)libInit							},
    { CLT_Interfaces,   (uint32)libInterfaces					},
    { CLT_NoLegacyIFace,(uint32)TRUE							},
    { TAG_DONE,         (uint32)0								}
};

static struct Resident __attribute__((used)) lib_res = {
    RTC_MATCHWORD,										// rt_MatchWord
    &lib_res,											// rt_MatchTag
    &lib_res+1,											// rt_EndSkip
    RTF_NATIVE | RTF_AUTOINIT | RTF_COLDSTART,			// rt_Flags
    LIBVERSION,											// rt_Version
    NT_LIBRARY,											// rt_Type
    LIBPRI,												// rt_Pri
    LIBNAME,											// rt_Name
    VSTRING,											// rt_IdString
    libCreateTags										// rt_Init
};