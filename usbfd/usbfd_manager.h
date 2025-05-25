// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

#include "usbfd_devicebase.h"
#include "usbfd_manager_close.h"
#include "usbfd_manager_expunge.h"
#include "usbfd_manager_obtain.h"
#include "usbfd_manager_open.h"
#include "usbfd_manager_release.h"


static APTR UsbFDManager_Vectors[] = {
    _usbfd_manager_Obtain,
    _usbfd_manager_Release,
    NULL,
    NULL,
    _usbfd_manager_Open,
    _usbfd_manager_Close,
    _usbfd_manager_Expunge,
    NULL,
    (APTR)-1,
};

static struct TagItem UsbFDManager_Tags[] = {
    { MIT_Name,         (uint32)"__library"				},
    { MIT_VectorTable,  (uint32)UsbFDManager_Vectors	},
    { MIT_Version,      (uint32)1						},
    { TAG_DONE,         (uint32)0						}
};
