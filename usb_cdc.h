// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

enum CDCStopBits {
	CDC_STOP_BITS_1		= 0,
	CDC_STOP_BITS_1_5	= 1,
	CDC_STOP_BITS_2		= 2
};

enum CDCParityType {
	CDC_PARITY_NONE		= 0,
	CDC_PARITY_ODD		= 1,
	CDC_PARITY_EVEN		= 2,
	CDC_PARITY_MARK		= 3,
	CDC_PARITY_SPACE	= 4
};

enum CDCDataBits {
	CDC_DATA_BITS_5		= 5,
	CDC_DATA_BITS_6		= 6,
	CDC_DATA_BITS_7		= 7,
	CDC_DATA_BITS_8		= 8,
	CDC_DATA_BITS_16	= 16
};

// MEMF_SHARED
struct UsbCDCLineCoding {
    ULONG uclc_dwDTERate;							/* Baud rate */
    UBYTE uclc_bCharFormat;							/* Stop bits 0->1, 1->1.5, 2->2 */
    UBYTE uclc_bParityType;							/* Party (0=None, 1=Odd, 2=Even, 3=Mark, 4=Space) */
    UBYTE uclc_bDataBits;							/* Databits (5,6,7,8 or 16) */
};