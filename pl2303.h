// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once


#define BREAK_REQUEST_TYPE			0x21
#define BREAK_REQUEST				0x23
#define BREAK_ON					0xffff
#define BREAK_OFF					0x0000

#define UART_STATE_INDEX			0x08
#define UART_STATE_MSR_MASK			0x8b
#define UART_STATE_TRANSIENT_MASK	0x74
#define UART_DCD					0x01
#define UART_DSR					0x02
#define UART_BREAK_ERROR			0x04
#define UART_RING					0x08
#define UART_FRAME_ERROR			0x10
#define UART_PARITY_ERROR			0x20
#define UART_OVERRUN_ERROR			0x40
#define UART_CTS					0x80

#define PL2303_FLOWCTRL_MASK		0xf0


#define PL2303_HXN_RESET_REG		0x07
#define PL2303_HXN_RESET_UPSTREAM_PIPE	0x02
#define PL2303_HXN_RESET_DOWNSTREAM_PIPE	0x01

#define PL2303_HXN_FLOWCTRL_REG		0x0a
#define PL2303_HXN_FLOWCTRL_MASK	0x1c
#define PL2303_HXN_FLOWCTRL_NONE	0x1c
#define PL2303_HXN_FLOWCTRL_RTS_CTS	0x18
#define PL2303_HXN_FLOWCTRL_XON_XOFF	0x0c


 #define BENQ_VENDOR_ID				0x04a5
 #define BENQ_PRODUCT_ID_S81		0x4027
 
 #define PL2303_VENDOR_ID			0x067b
 #define PL2303_PRODUCT_ID			0x2303
 #define PL2303_PRODUCT_ID_TB		0x2304
 #define PL2303_PRODUCT_ID_GC		0x23a3
 #define PL2303_PRODUCT_ID_GB		0x23b3
 #define PL2303_PRODUCT_ID_GT		0x23c3
 #define PL2303_PRODUCT_ID_GL		0x23d3
 #define PL2303_PRODUCT_ID_GE		0x23e3
 #define PL2303_PRODUCT_ID_GS		0x23f3
 #define PL2303_PRODUCT_ID_RSAQ2	0x04bb
 #define PL2303_PRODUCT_ID_DCU11	0x1234
 #define PL2303_PRODUCT_ID_PHAROS	0xaaa0
 #define PL2303_PRODUCT_ID_RSAQ3	0xaaa2
 #define PL2303_PRODUCT_ID_CHILITAG	0xaaa8
 #define PL2303_PRODUCT_ID_ALDIGA	0x0611
 #define PL2303_PRODUCT_ID_MMX		0x0612
 #define PL2303_PRODUCT_ID_GPRS		0x0609
 #define PL2303_PRODUCT_ID_HCR331	0x331a
 #define PL2303_PRODUCT_ID_MOTOROLA	0x0307
 #define PL2303_PRODUCT_ID_ZTEK		0xe1f1
 
 
 #define ATEN_VENDOR_ID				0x0557
 #define ATEN_VENDOR_ID2			0x0547
 #define ATEN_PRODUCT_ID			0x2008
 #define ATEN_PRODUCT_UC485			0x2021
 #define ATEN_PRODUCT_UC232B		0x2022
 #define ATEN_PRODUCT_ID2			0x2118
 
 #define IBM_VENDOR_ID				0x04b3
 #define IBM_PRODUCT_ID				0x4016
 
 #define IODATA_VENDOR_ID			0x04bb
 #define IODATA_PRODUCT_ID			0x0a03
 #define IODATA_PRODUCT_ID_RSAQ5	0x0a0e
 
 #define ELCOM_VENDOR_ID			0x056e
 #define ELCOM_PRODUCT_ID			0x5003
 #define ELCOM_PRODUCT_ID_UCSGT		0x5004
 
 #define ITEGNO_VENDOR_ID			0x0eba
 #define ITEGNO_PRODUCT_ID			0x1080
 #define ITEGNO_PRODUCT_ID_2080		0x2080
 
 #define MA620_VENDOR_ID			0x0df7
 #define MA620_PRODUCT_ID			0x0620
 
 #define RATOC_VENDOR_ID			0x0584
 #define RATOC_PRODUCT_ID			0xb000
 
 #define TRIPP_VENDOR_ID			0x2478
 #define TRIPP_PRODUCT_ID			0x2008
 
 #define RADIOSHACK_VENDOR_ID		0x1453
 #define RADIOSHACK_PRODUCT_ID		0x4026
 
 #define DCU10_VENDOR_ID			0x0731
 #define DCU10_PRODUCT_ID			0x0528
 
 #define SITECOM_VENDOR_ID			0x6189
 #define SITECOM_PRODUCT_ID			0x2068
 
 /* Alcatel OT535/735 USB cable */
 #define ALCATEL_VENDOR_ID			0x11f7
 #define ALCATEL_PRODUCT_ID			0x02df
 
 #define SIEMENS_VENDOR_ID			0x11f5
 #define SIEMENS_PRODUCT_ID_SX1		0x0001
 #define SIEMENS_PRODUCT_ID_X65		0x0003
 #define SIEMENS_PRODUCT_ID_X75		0x0004
 #define SIEMENS_PRODUCT_ID_EF81	0x0005
 
 #define SYNTECH_VENDOR_ID			0x0745
 #define SYNTECH_PRODUCT_ID			0x0001
 
 /* Nokia CA-42 Cable */
 #define NOKIA_CA42_VENDOR_ID		0x078b
 #define NOKIA_CA42_PRODUCT_ID		0x1234
 
 /* CA-42 CLONE Cable www.ca-42.com chipset: Prolific Technology Inc */
 #define CA_42_CA42_VENDOR_ID		0x10b5
 #define CA_42_CA42_PRODUCT_ID		0xac70
 
 #define SAGEM_VENDOR_ID			0x079b
 #define SAGEM_PRODUCT_ID			0x0027
 
 /* Leadtek GPS 9531 (ID 0413:2101) */
 #define LEADTEK_VENDOR_ID			0x0413
 #define LEADTEK_9531_PRODUCT_ID	0x2101
 
 /* USB GSM cable from Speed Dragon Multimedia, Ltd */
 #define SPEEDDRAGON_VENDOR_ID		0x0e55
 #define SPEEDDRAGON_PRODUCT_ID		0x110b
 
 /* DATAPILOT Universal-2 Phone Cable */
 #define DATAPILOT_U2_VENDOR_ID		0x0731
 #define DATAPILOT_U2_PRODUCT_ID	0x2003
 
 /* Belkin "F5U257" Serial Adapter */
 #define BELKIN_VENDOR_ID			0x050d
 #define BELKIN_PRODUCT_ID			0x0257
 
 /* Alcor Micro Corp. USB 2.0 TO RS-232 */
 #define ALCOR_VENDOR_ID			0x058F
 #define ALCOR_PRODUCT_ID			0x9720
 
 /* Willcom WS002IN Data Driver (by NetIndex Inc.) */
 #define WS002IN_VENDOR_ID			0x11f6
 #define WS002IN_PRODUCT_ID			0x2001
 
 /* Corega CG-USBRS232R Serial Adapter */
 #define COREGA_VENDOR_ID			0x07aa
 #define COREGA_PRODUCT_ID			0x002a
 
 /* Y.C. Cable U.S.A., Inc - USB to RS-232 */
 #define YCCABLE_VENDOR_ID			0x05ad
 #define YCCABLE_PRODUCT_ID			0x0fba
 
 /* "Superial" USB - Serial */
 #define SUPERIAL_VENDOR_ID			0x5372
 #define SUPERIAL_PRODUCT_ID		0x2303
 
 /* Hewlett-Packard POS Pole Displays */
 #define HP_VENDOR_ID				0x03f0
 #define HP_LD381GC_PRODUCT_ID		0x0183
 #define HP_LM920_PRODUCT_ID		0x026b
 #define HP_TD620_PRODUCT_ID		0x0956
 #define HP_LD960_PRODUCT_ID		0x0b39
 #define HP_LD381_PRODUCT_ID		0x0f7f
 #define HP_LM930_PRODUCT_ID		0x0f9b
 #define HP_LCM220_PRODUCT_ID		0x3139
 #define HP_LCM960_PRODUCT_ID		0x3239
 #define HP_LD220_PRODUCT_ID		0x3524
 #define HP_LD220TA_PRODUCT_ID		0x4349
 #define HP_LD960TA_PRODUCT_ID		0x4439
 #define HP_LM940_PRODUCT_ID		0x5039
 
 /* Cressi Edy (diving computer) PC interface */
 #define CRESSI_VENDOR_ID			0x04b8
 #define CRESSI_EDY_PRODUCT_ID		0x0521
 
 /* Zeagle dive computer interface */
 #define ZEAGLE_VENDOR_ID			0x04b8
 #define ZEAGLE_N2ITION3_PRODUCT_ID	0x0522
 
 /* Sony, USB data cable for CMD-Jxx mobile phones */
 #define SONY_VENDOR_ID				0x054c
 #define SONY_QN3USB_PRODUCT_ID		0x0437
 
 /* Sanwa KB-USB2 multimeter cable (ID: 11ad:0001) */
 #define SANWA_VENDOR_ID			0x11ad
 #define SANWA_PRODUCT_ID			0x0001
 
 /* ADLINK ND-6530 RS232,RS485 and RS422 adapter */
 #define ADLINK_VENDOR_ID			0x0b63
 #define ADLINK_ND6530_PRODUCT_ID	0x6530
 #define ADLINK_ND6530GC_PRODUCT_ID	0x653a
 
 /* SMART USB Serial Adapter */
 #define SMART_VENDOR_ID			0x0b8c
 #define SMART_PRODUCT_ID			0x2303
 
 /* Allied Telesis VT-Kit3 */
 #define AT_VENDOR_ID				0x0caa
 #define AT_VTKIT3_PRODUCT_ID		0x3001
 
 /* Macrosilicon MS3020 */
 #define MACROSILICON_VENDOR_ID		0x345f
 #define MACROSILICON_MS3020_PRODUCT_ID	0x3020
 