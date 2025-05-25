// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#include <interfaces/exec.h>
#include <interfaces/usbsys.h>
#include <usb/usb.h>

#include "pl2303/pl2303_devicebase.h"
#include "pl2303/pl2303_detect_type.h"
#include "pl2303/pl2303_encode_baud_rate.h"
#include "pl2303/pl2303_initialze.h"
#include "pl2303/pl2303_manager.h"
#include "pl2303/pl2303_line_request.h"
#include "usb_get_endpoint_descriptor.h"
#include "usbfd_devicebase.h"
#include "usbfd_function_run_interface.h"
#include "usbfd_version.h"

uint32 _usbfd_function_RunFunction( struct USBFDIFace *Self,struct USBFDStartupMsg *msg ) {

	struct UsbDeviceBase *usbBase = (struct UsbDeviceBase *)Self->Data.LibBase;
	struct ExecIFace *IExec = usbBase->sldb_IExec;

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Self = 0x%lx, UsbBase = 0x%lx\n",LIBNAME,__func__,__LINE__,Self,usbBase );
	#endif

	struct UsbRawFunction *rawFunction = (struct UsbRawFunction *)msg->Object;
	struct USBBusDevDsc *functionDescriptor = (struct USBBusDevDsc *)msg->Descriptor;

	// The usbsys.device has already be opened for us with an ready to user IORequest & MsgPort

	struct Device *USBSysBase = (struct Device*)msg->USBReq->io_Device;
	struct USBSysIFace *IUSBSys = (struct USBSysIFace*)IExec->GetInterface( (struct Library*)USBSysBase,"main",1L,NULL );
	if( IUSBSys == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to aqcuire main interface of usbsys.device",LIBNAME,__func__,__LINE__ );

		goto exit_opendevice_failed;
	}

	struct USBIOReq	*UsbIO = IUSBSys->USBAllocRequest( msg->USBReq,
		TAG_END );
	if( UsbIO == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allocate desired memory",LIBNAME,__func__,__LINE__ );
			
		goto exit_usbio_failed;
	}

	struct MsgPort *UsbStackMsgPort  = IExec->AllocSysObject( ASOT_PORT,TAG_END );
	if(  UsbStackMsgPort == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allocate message port",LIBNAME,__func__,__LINE__ );
			
		goto exit_alloc_msg_failed;
	}

	// It is important to claim it as fast as posisble, to not get "closed" !?
	struct UsbFunction *UsbFunction = IUSBSys->USBClaimFunction( rawFunction,(APTR)-1,UsbStackMsgPort );
	if( UsbFunction == NULL) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to claim function\n",LIBNAME,__func__,__LINE__ );

		goto exit_claim_failed;
	}
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Claimed function = %p\n",LIBNAME,__func__,__LINE__,UsbFunction );
	#endif

	struct USBBusDevDsc *deviceDescriptor = (struct USBBusDevDsc *)IUSBSys->USBEPGetDescriptor( (struct IORequest *)UsbIO,
		IUSBSys->USBGetEndPoint( UsbFunction,NULL,0 ),
		USBSDT_TYP_STANDARD | USBSDT_REC_DEVICE,
		USBDESC_DEVICE,
		0,
		TAG_END
	);
	if( deviceDescriptor == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to get device descriptor\n",LIBNAME,__func__,__LINE__ );

		goto exit_getdevicedescriptor_failed;
	}
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Vendor = 0x%04x, Product = 0x%04x , Number of configurations = %d\n",LIBNAME,__func__,__LINE__,LE_WORD( deviceDescriptor->dd_VendorID ),LE_WORD( deviceDescriptor->dd_Product ),deviceDescriptor->dd_NumConfigs );
	#endif

	struct USBBusCfgDsc *configurationDescriptors = IUSBSys->USBFktGetCfgDescriptors( (struct IORequest *)UsbIO,UsbFunction,0,TAG_END );;
	if( configurationDescriptors == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to get configuration descriptor\n",LIBNAME,__func__,__LINE__ );

		goto exit_getconfigurationdescriptor_failed;
	}
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Configuration Id = %d (should/must be 1), Number of participating interfaces = %d\n",LIBNAME,__func__,__LINE__,configurationDescriptors->cd_ConfigID,configurationDescriptors->cd_NumInterfaces );
	#endif

	int32 configurationResult = IUSBSys->USBFktSetConfiguration( (struct IORequest *)UsbIO,UsbFunction,configurationDescriptors,TAG_END );
	if( configurationResult != USBSETCONFIG_OK ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to set configuration descriptor (Configuration Id = %d)\n",LIBNAME,__func__,__LINE__,configurationDescriptors->cd_ConfigID );

		goto exit_setconfiguration_failed;
	}
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Successfull configured function\n",LIBNAME,__func__,__LINE__ );
	#endif

	struct USBBusIntDsc *interfaceDescriptor	= NULL; 
	struct USBBusEPDsc *interruptDescriptor		= NULL; 
	struct USBBusEPDsc *readDescriptor			= NULL; 
	struct USBBusEPDsc *writeDescriptor			= NULL; 
	for( struct USBBusDscHead *descriptor = (struct USBBusDscHead *)configurationDescriptors;descriptor != NULL;descriptor = IUSBSys->USBNextDescriptor( descriptor ) ) {
		if( descriptor->dh_Type == USBDESC_ENDPOINT ) {
			struct USBBusEPDsc *endpointDescriptor = (struct USBBusEPDsc *)descriptor;

			if( ( endpointDescriptor->ed_Address & USBEPADRM_DIRECTION ) ==  USBEPADR_DIR_IN ) {
				if( ( endpointDescriptor->ed_Attributes & USBEPATRM_TRANSFERTYPE ) ==  USBEPTT_INTERRUPT ) {
					interruptDescriptor = endpointDescriptor;
				}
				else if( ( endpointDescriptor->ed_Attributes & USBEPATRM_TRANSFERTYPE ) ==  USBEPTT_BULK ) {
					readDescriptor = endpointDescriptor;
				}
			}
			else if( ( endpointDescriptor->ed_Address & USBEPADRM_DIRECTION ) ==  USBEPADR_DIR_OUT ) {
				if( ( endpointDescriptor->ed_Attributes & USBEPATRM_TRANSFERTYPE ) ==  USBEPTT_BULK ) {
					writeDescriptor = endpointDescriptor;
				}			
			}
		}
		else if( descriptor->dh_Type == USBDESC_INTERFACE ) {
			interfaceDescriptor = (struct USBBusIntDsc *)descriptor;
		}
	}
	if( interfaceDescriptor == NULL || interruptDescriptor == NULL || readDescriptor == NULL || writeDescriptor == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to find required endpoint descriptors\n",LIBNAME,__func__,__LINE__ );

		goto exit_getendpointdescriptors_failed;
	}

	struct UsbInterface *UsbInterface = IUSBSys->USBGetInterface( UsbFunction,interfaceDescriptor->id_InterfaceID );
	if( UsbInterface == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to claim/get interface with id = %ld\n",LIBNAME,__func__,__LINE__,interfaceDescriptor->id_InterfaceID );

		goto exit_getinterface_failed;
	}

	struct UsbEndPoint *readEndpoint = IUSBSys->USBGetEndPoint( NULL,UsbInterface,readDescriptor->ed_Address );
	if ( readEndpoint == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to get read endpoint\n",LIBNAME,__func__,__LINE__ );

		goto exit_readendpoint_failed;
	}

	struct UsbEndPoint *writeEndpoint = IUSBSys->USBGetEndPoint( NULL,UsbInterface,writeDescriptor->ed_Address );
	if ( writeEndpoint == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to get write endpoint\n",LIBNAME,__func__,__LINE__ );

		goto exit_writeendpoint_failed;
	}

	struct UsbEndPoint *interruptEndpoint = IUSBSys->USBGetEndPoint( NULL,UsbInterface,interruptDescriptor->ed_Address );
	if ( interruptEndpoint == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to get interrupt endpoint\n",LIBNAME,__func__,__LINE__ );

		goto exit_interruptendpoint_failed;
	}

	struct USBIOReq *InterruptUsbIO = IUSBSys->USBAllocRequest( (struct IORequest *)UsbIO,TAG_END );
	if( InterruptUsbIO == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate USB IO Interrupt request\n",LIBNAME,__func__,__LINE__ );

		goto exit_interruptrequest_failed;
	}

	APTR lineStatus = IExec->AllocVecTags( LE_WORD( interruptDescriptor->ed_MaxPacketSize ),
		AVT_Type,		MEMF_SHARED,
		TAG_END );
	if( lineStatus == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate memory for line status\n",LIBNAME,__func__,__LINE__ );

		goto exit_alloclinestatus_failed;
	}
	
	InterruptUsbIO->io_Command		= CMD_READ;
	InterruptUsbIO->io_SetupData	= NULL;
	InterruptUsbIO->io_SetupLength	= 0;
	InterruptUsbIO->io_Data			= lineStatus;
	InterruptUsbIO->io_Length		= LE_WORD( interruptDescriptor->ed_MaxPacketSize );
	InterruptUsbIO->io_Actual		= 0;
	InterruptUsbIO->io_Offset		= 0;
	InterruptUsbIO->io_EndPoint		= interruptEndpoint;

	struct USBIOReq *ReadUsbIO = IUSBSys->USBAllocRequest( (struct IORequest *)UsbIO,TAG_END );
	if( ReadUsbIO == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate USB IO Read request\n",LIBNAME,__func__,__LINE__ );

		goto exit_readrequest_failed;
	}

	APTR inBuffer = IExec->AllocVecTags( LE_WORD( readDescriptor->ed_MaxPacketSize ),
		AVT_Type,		MEMF_SHARED,
		TAG_END );
	if( inBuffer == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate memory buffer for in endpoint\n",LIBNAME,__func__,__LINE__ );

		goto exit_allocinbuffer_failed;
	}

	ReadUsbIO->io_Command		= CMD_READ;
	ReadUsbIO->io_SetupData		= NULL;
	ReadUsbIO->io_SetupLength	= 0;
	ReadUsbIO->io_Data			= inBuffer;
	ReadUsbIO->io_Length		= LE_WORD( readDescriptor->ed_MaxPacketSize );
	ReadUsbIO->io_Actual		= 0;
	ReadUsbIO->io_Offset		= 0;
	ReadUsbIO->io_EndPoint		= readEndpoint;

	struct USBIOReq *WriteUsbIO = IUSBSys->USBAllocRequest( (struct IORequest *)UsbIO,TAG_END );
	if( WriteUsbIO == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate USB IO Write request\n",LIBNAME,__func__,__LINE__ );

		goto exit_writerequest_failed;
	}

	// TODO: Construct USB gadget specifc Amiga OS device name, to include the serial of the gadget, 
	// or the device address. Thus multiple gadgets at the same time are supported
	struct USBBusDscHead *serial = IUSBSys->USBEPGetDescriptor( (struct IORequest *)UsbIO,IUSBSys->USBGetEndPoint( UsbFunction,NULL,0 ),USBSDT_TYP_STANDARD | USBSDT_REC_DEVICE,USBDESC_STRING,deviceDescriptor->dd_SerialStr,TAG_END );
	if( serial != NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Serial (Index %d, Length %d) \n",LIBNAME,__func__,__LINE__,deviceDescriptor->dd_SerialStr,serial->dh_Length - 2  );

		IUSBSys->USBFreeDescriptors( serial );
	}

	struct UsbCDCLineCoding *lineCoding = (struct UsbCDCLineCoding *)IExec->AllocVecTags( sizeof(struct UsbCDCLineCoding),
		AVT_Type,			MEMF_SHARED,
		TAG_END );
	if( lineCoding == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate public memory!!\n",LIBNAME,__func__ ,__LINE__);

		goto exit_allocate_linecoding_failed;
	}

	ULONG deviceAddress;
	IUSBSys->USBGetRawFunctionAttrs( rawFunction,
		USBA_DeviceAddress,		&deviceAddress,
		TAG_END );
	IExec->DebugPrintF( "[%s] (%s@%ld) Function device at address = 0x%08x \n",LIBNAME,__func__,__LINE__,deviceAddress  );

	APTR mutex = IExec->AllocSysObjectTags( ASOT_MUTEX,
		ASOMUTEX_Recursive,	FALSE,  // The default is to create a recursive mutex.
		TAG_END );
	if( mutex == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to allcate mutex!!\n",LIBNAME,__func__ ,__LINE__);

		goto exit_allocate_mutex_failed;
	}
	
	ULONG initTab[] = {
		IDATA_CMOVE	( OFFSET(PL2303DeviceBase,pl2303_USBFDTask)	),
		IDATA_COPY	( sizeof(struct Task *) ),							(LONG)IExec->FindTask( NULL ),
		IDATA_COPY	( sizeof(struct USBSysIFace *) ),					(LONG)IUSBSys,
		IDATA_COPY	( sizeof(struct USBIOReq *)  ),						(LONG)UsbIO,
		IDATA_COPY	( sizeof(struct USBIOReq *)  ),						(LONG)ReadUsbIO,
		IDATA_COPY	( sizeof(struct USBIOReq *)  ),						(LONG)WriteUsbIO,
		IDATA_COPY	( sizeof(struct UsbFunction *) ),					(LONG)UsbFunction,
		IDATA_COPY	( sizeof(APTR) ),									(LONG)mutex,
		IDATA_QUIT
	};

	struct PL2303DeviceBase *devBase = (struct PL2303DeviceBase *)IExec->CreateLibraryTags(
		CLT_DataSize,     	sizeof(struct PL2303DeviceBase),
		CLT_InitFunc,     	_pl2303_Init,
		CLT_Interfaces,   	pl2303Interfaces,
		CLT_NoLegacyIFace,	TRUE,
		CLT_InitData,		initTab,
		TAG_DONE
	);
	if( devBase == NULL ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to prepared device\n",LIBNAME,__func__,__LINE__ );

		goto exit_create_device_failed;
	}

	IExec->NewMinList( &devBase->pl2303_readRequests );
	IExec->NewMinList( &devBase->pl2303_writeRequests );

	devBase->io_RBufLen 	= ReadUsbIO->io_Length;
	
	devBase->pl2303_Type	= pl2303_detect_type( devBase,deviceDescriptor );
	devBase->pl2303_Quirks	= pl2303_type_data[ devBase->pl2303_Type ].quirks;
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Detected PL2303 as type = %d\n",LIBNAME,__func__,__LINE__,devBase->pl2303_Type );
	#endif

	if( ! pl2303_initialze_chip( devBase ) ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to initialize chip!!\n",LIBNAME,__func__ ,__LINE__);

		goto exit_initialze_chip_failed;
	}
	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Chip initialzed\n",LIBNAME,__func__ ,__LINE__);
	#endif

	#if DebugLevel > 0
	if( pl2303_get_line_request( devBase,lineCoding ) == USBERR_NOERROR ) {
		pl2303_debug_print_line_request( devBase,lineCoding );
	}
	#endif
	
	lineCoding->uclc_dwDTERate		= LE_LONG( 115200 );
	lineCoding->uclc_bDataBits		= CDC_DATA_BITS_8;
	lineCoding->uclc_bParityType	= CDC_PARITY_NONE;
	lineCoding->uclc_bCharFormat	= CDC_STOP_BITS_1;
	if( pl2303_set_line_request( devBase,lineCoding ) != USBERR_NOERROR ) {
		IExec->DebugPrintF( "[%s] (%s@%ld) Failed to set line coding\n",LIBNAME,__func__,__LINE__ );

		goto exit_linecoding_failed;
	}

	#if DebugLevel > 0
	if( pl2303_get_line_request( devBase,lineCoding ) == USBERR_NOERROR ) {
		pl2303_debug_print_line_request( devBase,lineCoding );
	}
	#endif

	IExec->AddDevice( (struct Device *)devBase );

	// Clear it because we wait on that to tell use that another party demandd an expunge
	IExec->SetSignal( 0,SIGF_SINGLE );

	IExec->SendIO( (struct IORequest *)InterruptUsbIO );

	for( BOOL isDettached = FALSE;! isDettached; ) {

		for( struct IOStdReq *io = (struct IOStdReq *)IExec->GetMsg( &devBase->pl2303_Unit.unit_MsgPort );io != NULL;io = (struct IOStdReq *)IExec->GetMsg( &devBase->pl2303_Unit.unit_MsgPort ) ) {
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) IORequest = 0x%lx Received message\n",LIBNAME,__func__,__LINE__,io );

				if( io->io_Error == IOERR_ABORTED ) {
					IExec->DebugPrintF( "[%s] (%s@%ld) Received message has been aborted!!!\n",LIBNAME,__func__,__LINE__,io );
				}
			#endif		
		
			io->io_Actual = 0;

			switch( io->io_Command ) {
				case CMD_READ:

					IExec->MutexObtain( devBase->pl2303_Mutex );

					BOOL isReadRequestsEmpty = IsMinListEmpty( &devBase->pl2303_readRequests );

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Command = CMD_READ, Data = 0x%lx, Offset = 0x%lx, Length = %ld, Write Requests is empty = %d\n",LIBNAME,__func__,__LINE__,io->io_Data,io->io_Offset,io->io_Length,isReadRequestsEmpty );
					#endif

					IExec->AddTail( (struct List *)&devBase->pl2303_readRequests,(struct Node *)io );

					if( isReadRequestsEmpty && ReadUsbIO->io_Offset < ReadUsbIO->io_Actual ) {
						for( uint32 processed = ReadUsbIO->io_Offset;processed < ReadUsbIO->io_Actual; ) {

							struct IOStdReq *io = (struct IOStdReq *)IExec->GetHead( (struct List *)&devBase->pl2303_readRequests );
							if( io ) {
								IExec->DebugPrintF( "[%s] (%s@%ld) Processed = %d, Left = %d, Offset = %d\n",LIBNAME,__func__,__LINE__,processed,ReadUsbIO->io_Actual,ReadUsbIO->io_Offset );
								IExec->DebugPrintF( "[%s] (%s@%ld) io_Length = %d, io_Actual = %d\n",LIBNAME,__func__,__LINE__,io->io_Length,io->io_Actual );
	
								if( io->io_Length - io->io_Actual <= ReadUsbIO->io_Actual - processed ) {
									IExec->CopyMem( ReadUsbIO->io_Data + processed,io->io_Data + io->io_Actual,io->io_Length );
	
									io->io_Actual			 = io->io_Length;
									processed				+= io->io_Actual;						
									ReadUsbIO->io_Offset	+= io->io_Actual;

									io->io_Error	= IOERR_SUCCESS;
	
									IExec->Remove( (struct Node *)io );
									// Processed answer sender
									IExec->ReplyMsg( (struct Message *)io );
									IExec->DebugPrintF( "[%s] (%s@%ld) Replayed to CMD_READ\n",LIBNAME,__func__,__LINE__ );

								}
								else { // io->io_Length > ReadUsbIO->io_Actual
									IExec->CopyMem( ReadUsbIO->io_Data + processed,io->io_Data + io->io_Actual,ReadUsbIO->io_Actual - processed );
	
									io->io_Actual			 = ReadUsbIO->io_Actual - processed;
									processed				+= io->io_Actual;	
									ReadUsbIO->io_Offset	+= io->io_Actual;
								}

								IExec->DebugPrintF( "[%s] (%s@%ld) Processed = %d, Left = %d, Offset = %d, Read Requests is empty = %d\n",LIBNAME,__func__,__LINE__,processed,ReadUsbIO->io_Actual,ReadUsbIO->io_Offset,IsMinListEmpty( &devBase->pl2303_readRequests ) );

								if( ReadUsbIO->io_Offset == ReadUsbIO->io_Actual && ! IsMinListEmpty( &devBase->pl2303_readRequests ) ) {

									IExec->DebugPrintF( "[%s] (%s@%ld) Requests new data\n",LIBNAME,__func__,__LINE__ );

									ReadUsbIO->io_Actual = 0;
									ReadUsbIO->io_Offset = 0;
			
									IExec->SendIO( (struct IORequest *)ReadUsbIO );

									break;
								}
							}
							else {
								IExec->DebugPrintF( "[%s] (%s@%ld) No pending CMD_READ\n",LIBNAME,__func__,__LINE__ );
								ReadUsbIO->io_Offset = processed;
	
								break;
							}
						}
					}
					else {

						IExec->DebugPrintF( "[%s] (%s@%ld) Requests new data\n",LIBNAME,__func__,__LINE__ );

						ReadUsbIO->io_Actual = 0;
						ReadUsbIO->io_Offset = 0;

						IExec->SendIO( (struct IORequest *)ReadUsbIO );
					}

					IExec->MutexRelease( devBase->pl2303_Mutex );

					break;
		
				case CMD_WRITE:		
				
					IExec->MutexObtain( devBase->pl2303_Mutex );

					BOOL isWriteRequestsEmpty = IsMinListEmpty( &devBase->pl2303_writeRequests );

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Command = CMD_WRITE, Data = 0x%lx, Offset = 0x%lx, Length = %ld, Pending write request = %d\n",LIBNAME,__func__,__LINE__,io->io_Data,io->io_Offset,io->io_Length,isWriteRequestsEmpty );
					#endif

					// Check if data shall be written until \0 is found in "stream"
					if( io->io_Length == -1 ) {
						unsigned char *data = (unsigned char *)io->io_Data;
						for( ;*data != '\0';data++ );
						io->io_Actual = data - (unsigned char *)io->io_Data;

						#if DebugLevel > 0
							IExec->DebugPrintF( "[%s] (%s@%ld) Command = CMD_WRITE, Found Length = %ld, Data = '%s'\n",LIBNAME,__func__,__LINE__,io->io_Actual,(unsigned char *)io->io_Data );
						#endif
					}
					else {
						io->io_Actual = io->io_Length;
					}

					IExec->AddTail( (struct List *)&devBase->pl2303_writeRequests,(struct Node *)io );

					if( isWriteRequestsEmpty ) {
						WriteUsbIO->io_Command		= CMD_WRITE;
						WriteUsbIO->io_SetupData	= NULL;
						WriteUsbIO->io_SetupLength	= 0;
						WriteUsbIO->io_Data			= io->io_Data;;
						WriteUsbIO->io_Length		= io->io_Actual;
						WriteUsbIO->io_EndPoint		= writeEndpoint;
						IExec->SendIO( (struct IORequest *)WriteUsbIO );
					}

					IExec->MutexRelease( devBase->pl2303_Mutex );

					break;

				case SDCMD_SETPARAMS:

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Command = SDCMD_SETPARAMS\n",LIBNAME,__func__,__LINE__ );
					#endif

					// All parameters have already been checked that they are supported in _pl2303_serial_setparams, so just apply 
					struct IOExtSer *sio = (struct IOExtSer *)io;

					_pl2303_encode_baud_rate( IExec,devBase,devBase->pl2303_Type,sio->io_Baud,lineCoding );
					lineCoding->uclc_bDataBits		= sio->io_ReadLen;
					lineCoding->uclc_bParityType	= CDC_PARITY_NONE;
					if( ( sio->io_SerFlags &= SERF_PARTY_ODD ) ==  SERF_PARTY_ODD ) {
						lineCoding->uclc_bParityType	= CDC_PARITY_ODD;
					} 
					else if( ( sio->io_SerFlags &= SERF_PARTY_ON ) ==  SERF_PARTY_ON ) {
						lineCoding->uclc_bParityType	= CDC_PARITY_EVEN;
					}
					else if( ( sio->io_ExtFlags &= SEXTF_MARK ) ==  SEXTF_MARK ) {
						lineCoding->uclc_bParityType	= CDC_PARITY_MARK;
					}
					else if( ( sio->io_ExtFlags &= SEXTF_MSPON ) ==  SEXTF_MSPON ) {
						lineCoding->uclc_bParityType	= CDC_PARITY_SPACE;
					}
					lineCoding->uclc_bCharFormat	= CDC_STOP_BITS_1;
					if( sio->io_StopBits == 2 ) {
						lineCoding->uclc_bCharFormat	= CDC_STOP_BITS_2;
					} 
					pl2303_set_line_request( devBase,lineCoding );

					io->io_Error	= IOERR_SUCCESS;
	
					// Processed answer sender
					IExec->ReplyMsg( (struct Message *)io );	
					break;

				default:

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Command = 0x%lx  Don't know how to handle\n",LIBNAME,__func__,__LINE__,io->io_Command );
					#endif
		
					io->io_Error = IOERR_NOCMD;

					// Processed answer sender
					IExec->ReplyMsg( (struct Message *)io );
		
					break;
			}
		}

		// This is the now main Task to handle all communciation
		#if DebugLevel > 0
			IExec->DebugPrintF( "[%s] (%s@%ld) Waiting for work to be processed...\n",LIBNAME,__func__,__LINE__ );
		#endif

		uint32 signal = IExec->Wait( 1UL << UsbStackMsgPort->mp_SigBit | 1UL << UsbIO->io_Message.mn_ReplyPort->mp_SigBit | 1UL << devBase->pl2303_Unit.unit_MsgPort.mp_SigBit | SIGF_SINGLE );

		if( signal & SIGF_SINGLE ) {
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Got notification that pl2303.device got expunge demanded by other party\n",LIBNAME,__func__,__LINE__);
			#endif

			goto exit_expunge_from_device;
		}
		if( signal & ( 1UL << UsbIO->io_Message.mn_ReplyPort->mp_SigBit ) ) {
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Got notification from pl2303 gadget that requests has been processed\n",LIBNAME,__func__ ,__LINE__);
			#endif

			for( struct USBIOReq *usbIO = (struct USBIOReq *)IExec->GetMsg( UsbIO->io_Message.mn_ReplyPort );usbIO != NULL;usbIO = (struct USBIOReq *)IExec->GetMsg( UsbIO->io_Message.mn_ReplyPort ) ) {
				#if DebugLevel > 0
					IExec->DebugPrintF( "[%s] (%s@%ld) Received message, USBIOReq = %p\n",LIBNAME,__func__,__LINE__,usbIO );
				#endif		

				if( usbIO == ReadUsbIO ) {

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Read request processed, io_Length = %d, io_Offset = %d, io_Actual = %d,io_Error = %d\n",LIBNAME,__func__,__LINE__,ReadUsbIO->io_Length,ReadUsbIO->io_Offset,ReadUsbIO->io_Actual,ReadUsbIO->io_Error );
					#endif		

					if(  ReadUsbIO->io_Actual == 0 ) {
						#if DebugLevel > 0
							IExec->DebugPrintF( "[%s] (%s@%ld) Read request failed no data returned, reissuing\n",LIBNAME,__func__,__LINE__ );
						#endif		
						
						ReadUsbIO->io_Actual = 0;
						ReadUsbIO->io_Offset = 0;

						IExec->SendIO( (struct IORequest *)ReadUsbIO );
					}
					else {
						for( uint32 processed = ReadUsbIO->io_Offset;processed < ReadUsbIO->io_Actual; ) {

							struct IOStdReq *io = (struct IOStdReq *)IExec->GetHead( (struct List *)&devBase->pl2303_readRequests );
							if( io ) {
								IExec->DebugPrintF( "[%s] (%s@%ld) Processed = %d, Left = %d, Offset = %d\n",LIBNAME,__func__,__LINE__,processed,ReadUsbIO->io_Actual,ReadUsbIO->io_Offset );
								IExec->DebugPrintF( "[%s] (%s@%ld) io_Length = %d, io_Actual = %d\n",LIBNAME,__func__,__LINE__,io->io_Length,io->io_Actual );

								if( io->io_Length - io->io_Actual <= ReadUsbIO->io_Actual - processed ) {
									IExec->CopyMem( ReadUsbIO->io_Data + processed,io->io_Data + io->io_Actual,io->io_Length );

									io->io_Actual			 = io->io_Length;
									processed				+= io->io_Actual;						
									ReadUsbIO->io_Offset	+= io->io_Actual;

									io->io_Error	= IOERR_SUCCESS;

									IExec->Remove( (struct Node *)io );
									// Processed answer sender
									IExec->ReplyMsg( (struct Message *)io );	
									IExec->DebugPrintF( "[%s] (%s@%ld) Replayed to CMD_READ\n",LIBNAME,__func__,__LINE__ );
								}
								else { // io->io_Length > ReadUsbIO->io_Actual
									IExec->CopyMem( ReadUsbIO->io_Data + processed,io->io_Data + io->io_Actual,ReadUsbIO->io_Actual - processed );

									io->io_Actual			 = ReadUsbIO->io_Actual - processed;
									processed				+= io->io_Actual;	
									ReadUsbIO->io_Offset	+= io->io_Actual;
								}

								IExec->DebugPrintF( "[%s] (%s@%ld) Processed = %d, Left = %d, Offset = %d, Read Requests is empty = %d\n",LIBNAME,__func__,__LINE__,processed,ReadUsbIO->io_Actual,ReadUsbIO->io_Offset,IsMinListEmpty( &devBase->pl2303_readRequests ) );

								if( ReadUsbIO->io_Offset == ReadUsbIO->io_Actual && ! IsMinListEmpty( &devBase->pl2303_readRequests ) ) {

									IExec->DebugPrintF( "[%s] (%s@%ld) Requests new data\n",LIBNAME,__func__,__LINE__ );

									ReadUsbIO->io_Actual = 0;
									ReadUsbIO->io_Offset = 0;
			
									IExec->SendIO( (struct IORequest *)ReadUsbIO );

									break;
								}							
							}
							else {
								IExec->DebugPrintF( "[%s] (%s@%ld) No pending CMD_READ\n",LIBNAME,__func__,__LINE__ );

								ReadUsbIO->io_Offset = processed;

								break;
							}
						}
					}
				}
				else if( usbIO == WriteUsbIO ) {

					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Write request processed, io_Length = %d, io_Actual = %d,io_Error = %d\n",LIBNAME,__func__,__LINE__,WriteUsbIO->io_Length,WriteUsbIO->io_Actual,WriteUsbIO->io_Error );
					#endif		

					struct IOStdReq *io = (struct IOStdReq *)IExec->RemHead( (struct List *)&devBase->pl2303_writeRequests );
					io->io_Error = WriteUsbIO->io_Error;

					// Processed answer sender
					IExec->ReplyMsg( (struct Message *)io );

					io = (struct IOStdReq *)IExec->GetHead( (struct List *)&devBase->pl2303_writeRequests );
					if( io ) {
						WriteUsbIO->io_Data			= io->io_Data;;
						WriteUsbIO->io_Length		= io->io_Actual;
	
						IExec->SendIO( (struct IORequest *)WriteUsbIO );
					}
				}
				else if( usbIO == InterruptUsbIO ) {
					#if DebugLevel > 0
						IExec->DebugPrintF( "[%s] (%s@%ld) Interrupt request processed, io_Length = %d, io_Actual = %d, io_Error = %d\n",LIBNAME,__func__,__LINE__,InterruptUsbIO->io_Length,InterruptUsbIO->io_Actual,InterruptUsbIO->io_Error );
						IExec->DebugPrintF( "*** Received Hex Data: " );
						for( int i = 0; i < InterruptUsbIO->io_Actual; i++ ) {
							IExec->DebugPrintF(  "0x%02x ",((char *)InterruptUsbIO->io_Data)[ i ] );
						}
						IExec->DebugPrintF( "\n" );
					#endif		

					if( InterruptUsbIO->io_Error == USBERR_NOERROR ) {
						IExec->SendIO( (struct IORequest *)InterruptUsbIO );
					}
					#if DebugLevel > 0
					else {
						IExec->DebugPrintF( "[%s] (%s@%ld) Interrupt request processed with an IO error = %d, no further submitting of interrupt requests\n",LIBNAME,__func__,__LINE__,InterruptUsbIO->io_Error );
					}
					#endif		
				}
				#if DebugLevel > 0
				else {
					IExec->DebugPrintF( "[%s] (%s@%ld) Unknown ???? request processed\n",LIBNAME,__func__,__LINE__ );
				}
				#endif		
			}
		}
		if( signal & ( 1UL << devBase->pl2303_Unit.unit_MsgPort.mp_SigBit ) ) {
			#if DebugLevel > 0
				IExec->DebugPrintF( "[%s] (%s@%ld) Got notifcation that pl2303.device I/O work is requested\n",LIBNAME,__func__ ,__LINE__);
			#endif
		}
		if( signal & ( 1UL << UsbStackMsgPort->mp_SigBit ) ) {
			for( struct USBNotifyMsg *usbNotifyMessage = (struct USBNotifyMsg *)IExec->GetMsg( UsbStackMsgPort );usbNotifyMessage != NULL;usbNotifyMessage = (struct USBNotifyMsg *)IExec->GetMsg( UsbStackMsgPort ) ) {

				switch( usbNotifyMessage->Type ) {
					case USBNM_TYPE_FUNCTIONDETACH:
					case USBNM_TYPE_INTERFACEDETACH:
						#if DebugLevel > 0
							IExec->DebugPrintF( "[%s] (%s@%ld) Got Dettach-Notification from USB Stack Type := %d \n",LIBNAME,__func__,__LINE__,usbNotifyMessage->Type );
						#endif

						isDettached = TRUE;
						break;
					default:
						IExec->DebugPrintF( "[%s] (%s@%ld) Got Notification from USB Stack Type := %d which is unhandled\n",LIBNAME,__func__,__LINE__,usbNotifyMessage->Type );
						break;
				}

				IExec->ReplyMsg( (struct Message *)usbNotifyMessage );
			}
		}
	}


	// TODO: Need to clean any pending Io rquest from USbstack!
	
exit_linecoding_failed:
exit_initialze_chip_failed:
	
	// Clear it because we wait on that until the devic expunge has signal us that it has really expunged
	IExec->SetSignal( 0,SIGF_SINGLE );

	// We onle Remove the device, that will trigger a call to expunge of the device
	// And the device Expunge method is responsible to call DeleteLibrary
	IExec->RemDevice( (struct Device *)devBase );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Wait for expunge call to signal us to cleanup\n",LIBNAME,__func__,__LINE__ );
	#endif

	// Wait for signal from expunge call before freeing resources
	IExec->Wait( SIGF_SINGLE );

	#if DebugLevel > 0
		IExec->DebugPrintF( "[%s] (%s@%ld) Got signal to cleanup\n",LIBNAME,__func__,__LINE__ );
	#endif

exit_expunge_from_device:
exit_create_device_failed:

	IExec->FreeSysObject( ASOT_MUTEX,mutex );

exit_allocate_mutex_failed:

	IExec->FreeVec( lineCoding );

exit_allocate_linecoding_failed:

	IUSBSys->USBFreeRequest( WriteUsbIO );

exit_writerequest_failed:

	IExec->FreeVec( inBuffer );

exit_allocinbuffer_failed:

	IUSBSys->USBFreeRequest( ReadUsbIO );

exit_readrequest_failed:

	IExec->FreeVec( lineStatus );

exit_alloclinestatus_failed:

	IUSBSys->USBFreeRequest( InterruptUsbIO );

exit_interruptrequest_failed:
exit_interruptendpoint_failed:
exit_writeendpoint_failed:
exit_readendpoint_failed:

IUSBSys->USBDeclaimInterface( UsbInterface );

exit_getinterface_failed:
exit_getendpointdescriptors_failed:
exit_setconfiguration_failed:

	IUSBSys->USBFreeDescriptors( (struct USBBusDscHead *)configurationDescriptors );

exit_getconfigurationdescriptor_failed:

	IUSBSys->USBFreeDescriptors( (struct USBBusDscHead *)deviceDescriptor );

exit_getdevicedescriptor_failed:

	IUSBSys->USBDeclaimFunction( UsbFunction );

exit_claim_failed:

	IExec->FreeSysObject( ASOT_PORT,UsbStackMsgPort );

exit_alloc_msg_failed:

	IUSBSys->USBFreeRequest( UsbIO );
	
exit_usbio_failed:

	IExec->DropInterface( (struct Interface*)IUSBSys );
	
exit_opendevice_failed:

	// We do not ned to close / free for us opned device and setup IORequest & MsgPort

	return USBERR_NOERROR;
}