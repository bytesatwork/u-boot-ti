// SPDX-License-Identifier: GPL-2.0+
/*
 * RAM initialization for byteENGINE AM335x
 *
 * Copyright (C) 2015 bytes at work AG
 */

#include <asm/arch/ddr_defs.h>
#include <asm/arch-am33xx/clock.h>
#include <i2c.h>
#include <config.h>
#include "board.h"
#include "ddr_defs_baw.h"
#include "m2config/m2config.h"
#include "m2config/m2config_get.h"
#include "m2config/m2config_eeprom.h"

DECLARE_GLOBAL_DATA_PTR;

#define OSC	(V_OSCK / 1000000)

/* 256 MB DDR2 ------------------------------------------------------------- */

static const struct ctrl_ioregs MT47H128M16RT25E_ctrl_ioregs = {
	.cm0ioctl		= MT47H128M16RT25E_IOCTRL_VALUE,
	.cm1ioctl		= MT47H128M16RT25E_IOCTRL_VALUE,
	.cm2ioctl		= MT47H128M16RT25E_IOCTRL_VALUE,
	.dt0ioctl		= MT47H128M16RT25E_IOCTRL_VALUE,
	.dt1ioctl		= MT47H128M16RT25E_IOCTRL_VALUE,
};

static const struct ddr_data MT47H128M16RT25E_ddr_data = {
	.datardsratio0		= MT47H128M16RT25E_RD_DQS,
	.datafwsratio0		= MT47H128M16RT25E_PHY_FIFO_WE,
	.datawrsratio0		= MT47H128M16RT25E_PHY_WR_DATA,
};

static const struct cmd_control MT47H128M16RT25E_cmd_control = {
	.cmd0csratio		= MT47H128M16RT25E_RATIO,
	.cmd1csratio		= MT47H128M16RT25E_RATIO,
	.cmd2csratio		= MT47H128M16RT25E_RATIO,
};

static const struct emif_regs MT47H128M16RT25E_emif_regs = {
	.sdram_config		= MT47H128M16RT25E_EMIF_SDCFG,
	.ref_ctrl		= MT47H128M16RT25E_EMIF_SDREF,
	.sdram_tim1		= MT47H128M16RT25E_EMIF_TIM1,
	.sdram_tim2		= MT47H128M16RT25E_EMIF_TIM2,
	.sdram_tim3		= MT47H128M16RT25E_EMIF_TIM3,
	.emif_ddr_phy_ctlr_1	= MT47H128M16RT25E_EMIF_READ_LATENCY,
};

#define MT47H128M16RT25E_CLOCK_MHZ	266

static void init_MT47H128M16RT25E(void)
{
	config_ddr(MT47H128M16RT25E_CLOCK_MHZ,
		   &MT47H128M16RT25E_ctrl_ioregs,
		   &MT47H128M16RT25E_ddr_data,
		   &MT47H128M16RT25E_cmd_control,
		   &MT47H128M16RT25E_emif_regs,
		   0);
}

static const struct dpll_params MT47H128M16RT25E_dpll = {
	MT47H128M16RT25E_CLOCK_MHZ, OSC - 1,
	1, -1, -1, -1, -1
};

/* DDR3 256 and 512 MB Legacy setting due to wrong programmed modules config 2 */

static const struct ctrl_ioregs K4B2G1646EBIH9_ctrl_ioregs = {
	.cm0ioctl               = K4B2G1646EBIH9_IOCTRL_VALUE,
	.cm1ioctl               = K4B2G1646EBIH9_IOCTRL_VALUE,
	.cm2ioctl               = K4B2G1646EBIH9_IOCTRL_VALUE,
	.dt0ioctl               = K4B2G1646EBIH9_IOCTRL_VALUE,
	.dt1ioctl               = K4B2G1646EBIH9_IOCTRL_VALUE,
};

static const struct ddr_data K4B2G1646EBIH9_ddr_data = {
	.datardsratio0		= K4B2G1646EBIH9_RD_DQS,
	.datawdsratio0		= K4B2G1646EBIH9_WR_DQS,
	.datafwsratio0		= K4B2G1646EBIH9_PHY_FIFO_WE,
	.datawrsratio0		= K4B2G1646EBIH9_PHY_WR_DATA,
};

static const struct cmd_control K4B2G1646EBIH9_cmd_control = {
	.cmd0csratio		= K4B2G1646EBIH9_RATIO,
	.cmd0iclkout		= K4B2G1646EBIH9_INVERT_CLKOUT,

	.cmd1csratio		= K4B2G1646EBIH9_RATIO,
	.cmd1iclkout		= K4B2G1646EBIH9_INVERT_CLKOUT,

	.cmd2csratio		= K4B2G1646EBIH9_RATIO,
	.cmd2iclkout		= K4B2G1646EBIH9_INVERT_CLKOUT,
};

static const struct emif_regs K4B2G1646EBIH9_emif_regs = {
	.sdram_config		= K4B2G1646EBIH9_EMIF_SDCFG,
	.ref_ctrl		= K4B2G1646EBIH9_EMIF_SDREF,
	.sdram_tim1		= K4B2G1646EBIH9_EMIF_TIM1,
	.sdram_tim2		= K4B2G1646EBIH9_EMIF_TIM2,
	.sdram_tim3		= K4B2G1646EBIH9_EMIF_TIM3,
	.zq_config		= K4B2G1646EBIH9_ZQ_CFG,
	.emif_ddr_phy_ctlr_1	= K4B2G1646EBIH9_EMIF_READ_LATENCY,
};

#define K4B2G1646EBIH9_CLOCK_MHZ	303

static void init_K4B2G1646EBIH9(void)
{
	config_ddr(K4B2G1646EBIH9_CLOCK_MHZ,
		   &K4B2G1646EBIH9_ctrl_ioregs,
		   &K4B2G1646EBIH9_ddr_data,
		   &K4B2G1646EBIH9_cmd_control,
		   &K4B2G1646EBIH9_emif_regs,
		   0);

	mdelay(2);
}

static const struct dpll_params K4B2G1646EBIH9_dpll = {
	K4B2G1646EBIH9_CLOCK_MHZ, OSC - 1,
	1, -1, -1, -1, -1
};

/* DDR3 256 MB K4B2G1646Q-BCK0 --------------------------------------------- */

static const struct ctrl_ioregs K4B2G1646QBCK0_ctrl_ioregs = {
	.cm0ioctl               = K4B2G1646QBCK0_IOCTRL_VALUE,
	.cm1ioctl               = K4B2G1646QBCK0_IOCTRL_VALUE,
	.cm2ioctl               = K4B2G1646QBCK0_IOCTRL_VALUE,
	.dt0ioctl               = K4B2G1646QBCK0_IOCTRL_VALUE,
	.dt1ioctl               = K4B2G1646QBCK0_IOCTRL_VALUE,
};

static const struct ddr_data K4B2G1646QBCK0_ddr_data = {
	.datardsratio0		= K4B2G1646QBCK0_RD_DQS,
	.datawdsratio0		= K4B2G1646QBCK0_WR_DQS,
	.datafwsratio0		= K4B2G1646QBCK0_PHY_FIFO_WE,
	.datawrsratio0		= K4B2G1646QBCK0_PHY_WR_DATA,
};

static const struct cmd_control K4B2G1646QBCK0_cmd_control = {
	.cmd0csratio		= K4B2G1646QBCK0_RATIO,
	.cmd0iclkout		= K4B2G1646QBCK0_INVERT_CLKOUT,

	.cmd1csratio		= K4B2G1646QBCK0_RATIO,
	.cmd1iclkout		= K4B2G1646QBCK0_INVERT_CLKOUT,

	.cmd2csratio		= K4B2G1646QBCK0_RATIO,
	.cmd2iclkout		= K4B2G1646QBCK0_INVERT_CLKOUT,
};

static const struct emif_regs K4B2G1646QBCK0_emif_regs = {
	.sdram_config		= K4B2G1646QBCK0_EMIF_SDCFG,
	.ref_ctrl		= K4B2G1646QBCK0_EMIF_SDREF,
	.sdram_tim1		= K4B2G1646QBCK0_EMIF_TIM1,
	.sdram_tim2		= K4B2G1646QBCK0_EMIF_TIM2,
	.sdram_tim3		= K4B2G1646QBCK0_EMIF_TIM3,
	.zq_config		= K4B2G1646QBCK0_ZQ_CFG,
	.emif_ddr_phy_ctlr_1	= K4B2G1646QBCK0_EMIF_READ_LATENCY,
};

#define K4B2G1646QBCK0_CLOCK_MHZ	400

static void init_K4B2G1646QBCK0(void)
{
	config_ddr(K4B2G1646QBCK0_CLOCK_MHZ,
		   &K4B2G1646QBCK0_ctrl_ioregs,
		   &K4B2G1646QBCK0_ddr_data,
		   &K4B2G1646QBCK0_cmd_control,
		   &K4B2G1646QBCK0_emif_regs,
		   0);

	mdelay(2);
}

static const struct dpll_params K4B2G1646QBCK0_dpll = {
	K4B2G1646QBCK0_CLOCK_MHZ, OSC - 1,
	1, -1, -1, -1, -1
};

/* DDR3 512MB K4B4G1646D --------------------------------------------------- */
static const struct ctrl_ioregs K4B4G1646DBIK0_ctrl_ioregs = {
	.cm0ioctl               = K4B4G1646DBIK0_IOCTRL_VALUE,
	.cm1ioctl               = K4B4G1646DBIK0_IOCTRL_VALUE,
	.cm2ioctl               = K4B4G1646DBIK0_IOCTRL_VALUE,
	.dt0ioctl               = K4B4G1646DBIK0_IOCTRL_VALUE,
	.dt1ioctl               = K4B4G1646DBIK0_IOCTRL_VALUE,
};

static const struct ddr_data K4B4G1646DBIK0_ddr_data = {
	.datardsratio0		= K4B4G1646DBIK0_RD_DQS,
	.datawdsratio0		= K4B4G1646DBIK0_WR_DQS,
	.datafwsratio0		= K4B4G1646DBIK0_PHY_FIFO_WE,
	.datawrsratio0		= K4B4G1646DBIK0_PHY_WR_DATA,
};

static const struct cmd_control K4B4G1646DBIK0_cmd_control = {
	.cmd0csratio		= K4B4G1646DBIK0_RATIO,
	.cmd0iclkout		= K4B4G1646DBIK0_INVERT_CLKOUT,

	.cmd1csratio		= K4B4G1646DBIK0_RATIO,
	.cmd1iclkout		= K4B4G1646DBIK0_INVERT_CLKOUT,

	.cmd2csratio		= K4B4G1646DBIK0_RATIO,
	.cmd2iclkout		= K4B4G1646DBIK0_INVERT_CLKOUT,
};

static const struct emif_regs K4B4G1646DBIK0_emif_regs = {
	.sdram_config		= K4B4G1646DBIK0_EMIF_SDCFG,
	.ref_ctrl		= K4B4G1646DBIK0_EMIF_SDREF,
	.sdram_tim1		= K4B4G1646DBIK0_EMIF_TIM1,
	.sdram_tim2		= K4B4G1646DBIK0_EMIF_TIM2,
	.sdram_tim3		= K4B4G1646DBIK0_EMIF_TIM3,
	.zq_config		= K4B4G1646DBIK0_ZQ_CFG,
	.emif_ddr_phy_ctlr_1	= K4B4G1646DBIK0_EMIF_READ_LATENCY,
};

#define K4B4G1646DBIK0_CLOCK_MHZ	400

static void init_K4B4G1646DBIK0(void)
{
	config_ddr(K4B4G1646DBIK0_CLOCK_MHZ,
		   &K4B4G1646DBIK0_ctrl_ioregs,
		   &K4B4G1646DBIK0_ddr_data,
		   &K4B4G1646DBIK0_cmd_control,
		   &K4B4G1646DBIK0_emif_regs,
		   0);

	mdelay(2);
}

static const struct dpll_params K4B4G1646DBIK0_dpll = {
	K4B4G1646DBIK0_CLOCK_MHZ, OSC - 1,
	1, -1, -1, -1, -1
};

/* DDR3 1024MB MT41K512M16HA-107 IT:A 	D9SGD -------------------------------*/

static const struct ctrl_ioregs MT41K512M16HA107_ctrl_ioregs = {
	.cm0ioctl               = MT41K512M16HA107_IOCTRL_VALUE,
	.cm1ioctl               = MT41K512M16HA107_IOCTRL_VALUE,
	.cm2ioctl               = MT41K512M16HA107_IOCTRL_VALUE,
	.dt0ioctl               = MT41K512M16HA107_IOCTRL_VALUE,
	.dt1ioctl               = MT41K512M16HA107_IOCTRL_VALUE,
};

static const struct ddr_data MT41K512M16HA107_ddr_data = {
	.datardsratio0		= MT41K512M16HA107_RD_DQS,
	.datawdsratio0		= MT41K512M16HA107_WR_DQS,
	.datafwsratio0		= MT41K512M16HA107_PHY_FIFO_WE,
	.datawrsratio0		= MT41K512M16HA107_PHY_WR_DATA,
};

static const struct cmd_control MT41K512M16HA107_cmd_control = {
	.cmd0csratio		= MT41K512M16HA107_RATIO,
	.cmd0iclkout		= MT41K512M16HA107_INVERT_CLKOUT,

	.cmd1csratio		= MT41K512M16HA107_RATIO,
	.cmd1iclkout		= MT41K512M16HA107_INVERT_CLKOUT,

	.cmd2csratio		= MT41K512M16HA107_RATIO,
	.cmd2iclkout		= MT41K512M16HA107_INVERT_CLKOUT,
};

static const struct emif_regs MT41K512M16HA107_emif_regs = {
	.sdram_config		= MT41K512M16HA107_EMIF_SDCFG,
	.ref_ctrl		= MT41K512M16HA107_EMIF_SDREF,
	.sdram_tim1		= MT41K512M16HA107_EMIF_TIM1,
	.sdram_tim2		= MT41K512M16HA107_EMIF_TIM2,
	.sdram_tim3		= MT41K512M16HA107_EMIF_TIM3,
	.zq_config		= MT41K512M16HA107_ZQ_CFG,
	.emif_ddr_phy_ctlr_1	= MT41K512M16HA107_EMIF_READ_LATENCY,
};

#define MT41K512M16HA107_CLOCK_MHZ 303

static void init_MT41K512M16HA107(void)
{
	config_ddr(MT41K512M16HA107_CLOCK_MHZ,
		   &MT41K512M16HA107_ctrl_ioregs,
		   &MT41K512M16HA107_ddr_data,
		   &MT41K512M16HA107_cmd_control,
		   &MT41K512M16HA107_emif_regs,
		   0);

	mdelay(2);
}

static const struct dpll_params MT41K512M16HA107_dpll = {
	MT41K512M16HA107_CLOCK_MHZ, OSC - 1,
	1, -1, -1, -1, -1
};

/* ------------------------------------------------------------------------- */

struct ram_config {
	m2config_ram_t	config;
	void (*init)(void);
	const struct dpll_params *dpll_param;
};

const struct ram_config ram_init_map[] = {
	{ M2_RAM_MT47H128M16RT25E, init_MT47H128M16RT25E, &MT47H128M16RT25E_dpll },
	{ M2_RAM_K4B2G1646EBIH9, init_K4B2G1646EBIH9, &K4B2G1646EBIH9_dpll },
	{ M2_RAM_K4B2G1646QBCK0, init_K4B2G1646QBCK0, &K4B2G1646QBCK0_dpll },
	{ M2_RAM_K4B4G1646DBIK0, init_K4B4G1646DBIK0, &K4B4G1646DBIK0_dpll },
	{ M2_RAM_MT41K512M16HA107, init_MT41K512M16HA107, &MT41K512M16HA107_dpll },
	{ -1, NULL }
};

static const struct ram_config *find_ram(m2config_ram_t config)
{
	const struct ram_config *itr;

	for (itr = ram_init_map; itr->init; itr++)
		if (itr->config == config)
			return itr;

	return NULL;
}

static const struct ram_config *getRam(void)
{
	const struct ram_config *ram = NULL;
	struct m2config config;

	m2config_get(&config);
	ram = find_ram(config.ram);

	return ram;
}

void sdram_init(void)
{
	const struct ram_config *ram = getRam();

	ram->init();

	/* Set OCP_CONFIG (fix LCD pixel shift during operation) */
	writel(EMIF_OCP_CONFIG_BYTEENGINE_AM335X, EMIF_OCP_CONFIG);
}

const struct dpll_params *get_dpll_ddr_params(void)
{
	enable_i2c0_pin_mux();
#ifndef CONFIG_DM_I2C
	i2c_init(CONFIG_SYS_OMAP24_I2C_SPEED, CONFIG_SYS_OMAP24_I2C_SLAVE);
#endif

	const struct ram_config *ram = getRam();

	return ram->dpll_param;
}
