// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2026 bytesatwork AG - https://www.bytesatwork.io
 *
 * Based on ti/am62lx/evm.c
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <dm/uclass.h>
#include <env.h>
#include <fdt_support.h>
#include <spl.h>

#include "../../ti/common/fdt_ops.h"
#include "../baw_config/baw_config_eeprom.h"

int board_init(void)
{
	return 0;
}

#if IS_ENABLED(CONFIG_BOARD_LATE_INIT)
int board_late_init(void)
{
	int ret;

	ret = baw_config_eeprom_init();
	if (ret)
		printf("Initialize byteENGINE EEPROM failed! (%d)\n", ret);

	ti_set_fdt_env(NULL, NULL);
	return 0;
}
#endif
