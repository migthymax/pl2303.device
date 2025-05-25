// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <devices/serial.h>
#include <exec/devices.h>
#include <exec/types.h>

#include "pl2303_types.h"

struct PL2303DeviceBase {
	struct Device					pl2303_Device;			// Device node
	struct Unit						pl2303_Unit;
	UWORD							pl2303_Pad;

	enum pl2303_type				pl2303_Type;
	unsigned long 					pl2303_Quirks;			// Quirks for this device

	struct ExecIFace *          	pl2303_IExec;
	APTR                        	pl2303_SegmentList;		// Segment list
	
	// The following fields are read only
	struct Task *					pl2303_USBFDTask;
	struct USBSysIFace *			pl2303_IUSBSys;			
	struct USBIOReq *				pl2303_UsbIO;	
	struct USBIOReq *				pl2303_readUsbIO;
	struct USBIOReq *				pl2303_writeUsbIO;
	struct UsbFunction *			pl2303_Function;

	// Mutex for accing the floowing read/write lists
	APTR							pl2303_Mutex;
	struct MinList					pl2303_readRequests;
	struct MinList					pl2303_writeRequests;

	// Copy of the IOExtSer fields to hold the current state
	ULONG							io_CtlChar;				/* control char's (order = xON,xOFF,INQ,ACK) */
    ULONG							io_RBufLen;					/* length in bytes of serial port's read buffer */
    ULONG							io_ExtFlags;				/* additional serial flags (see bitdefs below) */
    ULONG							io_Baud;					/* baud rate requested (true baud) */
    ULONG							io_BrkTime;				/* duration of break signal in MICROseconds */
    struct IOTArray					io_TermArray;			/* termination character array */
    UBYTE							io_ReadLen;					/* bits per read character (# of bits) */
    UBYTE							io_WriteLen;				/* bits per write character (# of bits) */
    UBYTE							io_StopBits;				/* stopbits for read (# of bits) */
    UBYTE							io_SerFlags;				/* see SerFlags bit definitions below  */
    UWORD							io_Status;				//
};