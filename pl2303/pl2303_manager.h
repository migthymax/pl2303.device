// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

#include "pl2303_manager_abort_io.h"
#include "pl2303_manager_begin_io.h"
#include "pl2303_manager_close.h"
#include "pl2303_manager_expunge.h"
#include "pl2303_manager_obtain.h"
#include "pl2303_manager_open.h"
#include "pl2303_manager_release.h"

static APTR pl2303Manager_Vectors[] = {
    _pl2303_manager_Obtain,
    _pl2303_manager_Release,
    NULL,
    NULL,
    _pl2303_manager_Open,
    _pl2303_manager_Close,
    _pl2303_manager_Expunge,
    NULL,
	_pl2303_manager_BeginIO,
	_pl2303_manager_AbortIO,
    (APTR)-1,
};

static struct TagItem pl2303Manager_Tags[] = {
    { MIT_Name,         (uint32)"__device"				},
    { MIT_VectorTable,  (uint32)pl2303Manager_Vectors	},
    { MIT_Version,      (uint32)1						},
    { TAG_DONE,         (uint32)0						}
};

static APTR pl2303Interfaces[] = {
    pl2303Manager_Tags,
    NULL
};


struct Library *_pl2303_Init( struct PL2303DeviceBase *libBase,APTR seglist,struct ExecIFace *IExec );

