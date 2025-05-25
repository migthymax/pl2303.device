// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/interfaces.h>

VOID _pl2303_manager_AbortIO( struct DeviceManagerInterface *Self,struct IOStdReq *io );