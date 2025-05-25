// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

enum pl2303_type {
	TYPE_H,
	TYPE_HX,
	TYPE_TA,
	TYPE_TB,
	TYPE_HXD,
	TYPE_HXN,
	UNKNOWN,
	TYPE_COUNT
};

struct pl2303_type_data {
	const char *name;
	unsigned int max_baud_rate;
	unsigned long quirks;
	unsigned int no_autoxonxoff:1;
	unsigned int no_divisors:1;
	unsigned int alt_divisors:1;
};

#define PL2303_QUIRK_UART_STATE_IDX0		0x00000001
#define PL2303_QUIRK_LEGACY					0x00000002
#define PL2303_QUIRK_ENDPOINT_HACK			0x00000004
#define PL2303_QUIRK_NO_BREAK_GETLINE		0x00000008

static const struct pl2303_type_data pl2303_type_data[TYPE_COUNT] = {
	[TYPE_H] = {
		.name				= "H",
		.max_baud_rate		= 1228800,
		.quirks				= PL2303_QUIRK_LEGACY,
		.no_autoxonxoff		= TRUE,
	},
	[TYPE_HX] = {
		.name				= "HX",
		.max_baud_rate		= 6000000,
	},
	[TYPE_TA] = {
		.name				= "TA",
		.max_baud_rate		= 6000000,
		.alt_divisors		= TRUE,
	},
	[TYPE_TB] = {
		.name				= "TB",
		.max_baud_rate		= 12000000,
		.alt_divisors		= TRUE,
	},
	[TYPE_HXD] = {
		.name				= "HXD",
		.max_baud_rate		= 12000000,
	},
	[TYPE_HXN] = {
		.name				= "G",
		.max_baud_rate		= 12000000,
		.no_divisors		= TRUE,
	},
	[UNKNOWN] = {
		.name				= "Unknown",
	}
};