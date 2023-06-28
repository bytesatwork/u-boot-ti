// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2026 bytesatwork AG - https://www.bytesatwork.io
 *
 * Based on ti/am62x/evm.c
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Suman Anna <s-anna@ti.com>
 *
 */

#include <asm/io.h>
#include <spl.h>
#include <dm/uclass.h>
#include <k3-ddrss.h>
#include <fdt_support.h>
#include <asm/arch/hardware.h>
#include <env.h>
#include <net.h>
#include <cpu_func.h>
#include <mach/k3-ddr.h>

#include "../baw_config/baw_config_eeprom.h"

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

/* Copied from board/ti/common/k3-ddr.c */
int dram_init(void)
{
	s32 ret;

	ret = fdtdec_setup_mem_size_base_lowest();
	if (ret)
		printf("Error setting up mem size and base. %d\n", ret);

	return ret;
}

/* Copied from board/ti/common/k3-ddr.c */
int dram_init_banksize(void)
{
	s32 ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		printf("Error setting up memory banksize. %d\n", ret);

	return ret;
}

#if defined(CONFIG_SPL_BUILD)
void spl_perform_fixups(struct spl_image_info *spl_image)
{
	if (IS_ENABLED(CONFIG_K3_DDRSS)) {
		if (IS_ENABLED(CONFIG_K3_INLINE_ECC))
			fixup_ddr_driver_for_ecc(spl_image);
	} else {
		fixup_memory_node(spl_image);
	}
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	int ret;

	ret = baw_config_eeprom_init();
	if (ret)
		printf("Initialize byteENGINE EEPROM failed! (%d)\n", ret);

	return 0;
}
#endif

int last_stage_init(void)
{
	unsigned char ethaddr[6] = {0};
	int ret;

	/* Manually increase 2nd MAC address by one */
	ret = eth_env_get_enetaddr_by_index("eth", 0, ethaddr);
	if (ret) {
		for (int i = 5; i > 2; i--) {
			ethaddr[i]++;
			if (ethaddr[i])
				break;
		}
	} else {
		printf("Invalid MAC address at index 0!");
	}

	ret = eth_env_set_enetaddr_by_index("eth", 1, ethaddr);
	if (ret) {
		if (ret == -EEXIST)
			printf("Use MAC address at index 1 from env.\n");
		else
			printf("Set env MAC address at index 1 failed! (%d)\n", ret);
	}

	return 0;
}

EVENT_SPY_SIMPLE(EVT_LAST_STAGE_INIT, last_stage_init);

#ifdef CONFIG_SPL_BOARD_INIT
void spl_board_init(void)
{
	u32 val;

	/* We have 32k crystal, so lets enable it */
	val = readl(MCU_CTRL_LFXOSC_CTRL);
	val &= ~(MCU_CTRL_LFXOSC_32K_DISABLE_VAL);
	writel(val, MCU_CTRL_LFXOSC_CTRL);
	/* Add any TRIM needed for the crystal here.. */
	/* Make sure to mux up to take the SoC 32k from the crystal */
	writel(MCU_CTRL_DEVICE_CLKOUT_LFOSC_SELECT_VAL,
	       MCU_CTRL_DEVICE_CLKOUT_32K_CTRL);

	enable_caches();

	baw_config_eeprom_init();
}
#endif
