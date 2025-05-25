// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2025 Max Larsson
 *
 * This file is part of pl2303.device.
 */

#pragma once

#include <exec/types.h>

#include "usbfd_function_obtain.h"
#include "usbfd_function_release.h"
#include "usbfd_function_get_attrs.h"
#include "usbfd_function_run_function.h"
#include "usbfd_function_run_interface.h"

static APTR FunctionDriver_Vectors[] = {
    _usbfd_function_Obtain,
    _usbfd_function_Release,
    NULL,
    NULL,
    _usbfd_function_GetAttrsA,
    _usbfd_function_GetAttrs,
    _usbfd_function_RunFunction,
    _usbfd_function_RunInterface,
    (APTR)-1
};

static struct TagItem FunctionDriver_Tags[] = {
    { MIT_Name,         (uint32)"main"					},
    { MIT_VectorTable,  (uint32)FunctionDriver_Vectors	},
    { MIT_Version,      (uint32)1						},
    { TAG_DONE,         (uint32)0						}
};
