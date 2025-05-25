// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/io.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "pl2303_devicebase.h"

BOOL _pl2303_io_write( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io );
BOOL _pl2303_io_write_abort( struct ExecIFace *IExec,struct PL2303DeviceBase *devBase,struct IOStdReq *io );