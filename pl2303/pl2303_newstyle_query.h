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

BOOL _pl2303_newstyle_query( struct ExecIFace *IExec,struct IOStdReq *io );