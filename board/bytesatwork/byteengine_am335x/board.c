// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 bytes at work AG
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/omap.h>
#include <asm/arch/mem.h>
#include <asm/arch/clock.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mux.h>
#include <asm-generic/gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/emif.h>
#include <asm/io.h>
#include <cpsw.h>
#include <power/tps65910.h>
#include <environment.h>
#include <errno.h>
#include <miiphy.h>
#include <spl.h>
#include <watchdog.h>
#include "board.h"
#include "m2config/m2config_eeprom.h"

DECLARE_GLOBAL_DATA_PTR;

static __maybe_unused struct ctrl_dev *cdev =
	(struct ctrl_dev *)CTRL_DEVICE_BASE;

void set_uart_mux_conf(void)
{
	enable_uart0_pin_mux();
}

void set_mux_conf_regs(void)
{
	enable_board_pin_mux();
}

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
void am33xx_spl_board_init(void)
{
	int mpu_vdd;
	int sil_rev;

	/* Get the frequency */
	dpll_mpu_opp100.m = am335x_get_efuse_mpu_max_freq(cdev);

	/*
	 * Depending on MPU clock and PG we will need a different
	 * VDD to drive at that speed.
	 */
	sil_rev = readl(&cdev->deviceid) >> 28;
	mpu_vdd = am335x_get_tps65910_mpu_vdd(sil_rev,
					      dpll_mpu_opp100.m);

	/* Tell the TPS65910 to use i2c */
	tps65910_set_i2c_control();

	/* First update MPU voltage. */
	if (tps65910_voltage_update(MPU, mpu_vdd))
		return;

	/* Second, update the CORE voltage. */
	if (tps65910_voltage_update(CORE, TPS65910_OP_REG_SEL_1_1_3))
		return;

	/* Set CORE Frequencies to OPP100 */
	do_setup_dpll(&dpll_core_regs, &dpll_core_opp100);

	/* Set MPU Frequency to what we detected now that voltages are set */
	do_setup_dpll(&dpll_mpu_regs, &dpll_mpu_opp100);
}
#endif

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	m2config_eeprom_init();
#ifdef CONFIG_HW_WATCHDOG
	hw_watchdog_init();
#endif

	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
#ifdef CONFIG_NAND
	gpmc_init();
#endif

	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifndef CONFIG_SPL_BUILD
	u8 mac_addr[6];
	u32 mac_hi, mac_lo;

	/* Reset PHY */
	gpio_request(13, "PHY_RESET");
	gpio_direction_output(13, 0);
	udelay(100 * 1000);
	gpio_set_value(13, 1);

	/* try reading mac address from efuse */
	mac_lo = readl(&cdev->macid0l);
	mac_hi = readl(&cdev->macid0h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

	if (!env_get("ethaddr")) {
		printf("<ethaddr> not set. Validating first E-fuse MAC\n");

		if (is_valid_ethaddr(mac_addr))
			eth_env_set_enetaddr("ethaddr", mac_addr);
	}

	mac_lo = readl(&cdev->macid1l);
	mac_hi = readl(&cdev->macid1h);
	mac_addr[0] = mac_hi & 0xFF;
	mac_addr[1] = (mac_hi & 0xFF00) >> 8;
	mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
	mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
	mac_addr[4] = mac_lo & 0xFF;
	mac_addr[5] = (mac_lo & 0xFF00) >> 8;

	if (!env_get("eth1addr")) {
		if (is_valid_ethaddr(mac_addr))
			eth_env_set_enetaddr("eth1addr", mac_addr);
	}
#endif

	return 0;
}
#endif
