// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Texas Instruments' K3 SD Host Controller Interface
 */

#include <clk.h>
#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <power-domain.h>
#include <regmap.h>
#include <sdhci.h>
#include <soc.h>
#include "mmc_private.h"

/* CTL_CFG Registers */
#define CTL_CFG_2		0x14

#define SLOTTYPE_MASK		GENMASK(31, 30)
#define SLOTTYPE_EMBEDDED	BIT(30)

/* PHY Registers */
#define PHY_CTRL1	0x100
#define PHY_CTRL2	0x104
#define PHY_CTRL3	0x108
#define PHY_CTRL4	0x10C
#define PHY_CTRL5	0x110
#define PHY_CTRL6	0x114
#define PHY_STAT1	0x130
#define PHY_STAT2	0x134

#define IOMUX_ENABLE_SHIFT	31
#define IOMUX_ENABLE_MASK	BIT(IOMUX_ENABLE_SHIFT)
#define OTAPDLYENA_SHIFT	20
#define OTAPDLYENA_MASK		BIT(OTAPDLYENA_SHIFT)
#define OTAPDLYSEL_SHIFT	12
#define OTAPDLYSEL_MASK		GENMASK(15, 12)
#define STRBSEL_SHIFT		24
#define STRBSEL_4BIT_MASK	GENMASK(27, 24)
#define STRBSEL_8BIT_MASK	GENMASK(31, 24)
#define SEL50_SHIFT		8
#define SEL50_MASK		BIT(SEL50_SHIFT)
#define SEL100_SHIFT		9
#define SEL100_MASK		BIT(SEL100_SHIFT)
#define FREQSEL_SHIFT		8
#define FREQSEL_MASK		GENMASK(10, 8)
#define CLKBUFSEL_SHIFT		0
#define CLKBUFSEL_MASK		GENMASK(2, 0)
#define DLL_TRIM_ICP_SHIFT	4
#define DLL_TRIM_ICP_MASK	GENMASK(7, 4)
#define DR_TY_SHIFT		20
#define DR_TY_MASK		GENMASK(22, 20)
#define ENDLL_SHIFT		1
#define ENDLL_MASK		BIT(ENDLL_SHIFT)
#define DLLRDY_SHIFT		0
#define DLLRDY_MASK		BIT(DLLRDY_SHIFT)
#define PDB_SHIFT		0
#define PDB_MASK		BIT(PDB_SHIFT)
#define CALDONE_SHIFT		1
#define CALDONE_MASK		BIT(CALDONE_SHIFT)
#define RETRIM_SHIFT		17
#define RETRIM_MASK		BIT(RETRIM_SHIFT)
#define SELDLYTXCLK_SHIFT	17
#define SELDLYTXCLK_MASK	BIT(SELDLYTXCLK_SHIFT)

#define DRIVER_STRENGTH_50_OHM	0x0
#define DRIVER_STRENGTH_33_OHM	0x1
#define DRIVER_STRENGTH_66_OHM	0x2
#define DRIVER_STRENGTH_100_OHM	0x3
#define DRIVER_STRENGTH_40_OHM	0x4

#define AM654_SDHCI_MIN_FREQ	400000

#define SDHCI_TUNING_LOOP_COUNT	40

struct am654_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
	struct regmap *base;
	bool non_removable;
	u32 otap_del_sel[MMC_MODES_END];
	u32 trm_icp;
	u32 drv_strength;
	u32 strb_sel;
	u32 clkbuf_sel;
	u32 flags;
#define DLL_PRESENT	(1 << 0)
#define IOMUX_PRESENT	(1 << 1)
#define FREQSEL_2_BIT	(1 << 2)
#define STRBSEL_4_BIT	(1 << 3)
#define DLL_CALIB	(1 << 4)
	bool dll_on;
};

struct timing_data {
	const char *binding;
	u32 capability;
};

static const struct timing_data td[] = {
	[MMC_LEGACY] = {"ti,otap-del-sel-legacy", 0},
	[MMC_HS] = {"ti,otap-del-sel-mmc-hs", MMC_CAP(MMC_HS)},
	[SD_HS]  = {"ti,otap-del-sel-sd-hs", MMC_CAP(SD_HS)},
	[UHS_SDR12] = {"ti,otap-del-sel-sdr12", MMC_CAP(UHS_SDR12)},
	[UHS_SDR25] = {"ti,otap-del-sel-sdr25", MMC_CAP(UHS_SDR25)},
	[UHS_SDR50] = {"ti,otap-del-sel-sdr50", MMC_CAP(UHS_SDR50)},
	[UHS_SDR104] = {"ti,otap-del-sel-sdr104", MMC_CAP(UHS_SDR104)},
	[UHS_DDR50] = {"ti,otap-del-sel-ddr50", MMC_CAP(UHS_DDR50)},
	[MMC_DDR_52] = {"ti,otap-del-sel-ddr52", MMC_CAP(MMC_DDR_52)},
	[MMC_HS_200] = {"ti,otap-del-sel-hs200", MMC_CAP(MMC_HS_200)},
	[MMC_HS_400] = {"ti,otap-del-sel-hs400", MMC_CAP(MMC_HS_400)},
};

struct am654_driver_data {
	const struct sdhci_ops *ops;
	u32 flags;
};

static int am654_sdhci_execute_tuning(struct mmc *mmc, u8 opcode)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	u32 ctrl;
	struct sdhci_host *host;
	char tuning_loop_counter = SDHCI_TUNING_LOOP_COUNT;

	debug("%s\n", __func__);

	host = mmc->priv;

	ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctrl |= SDHCI_CTRL_EXEC_TUNING;
	sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);

	sdhci_writel(host, SDHCI_INT_DATA_AVAIL, SDHCI_INT_ENABLE);
	sdhci_writel(host, SDHCI_INT_DATA_AVAIL, SDHCI_SIGNAL_ENABLE);

	do {
		cmd.cmdidx = opcode;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;

		data.blocksize = 64;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;

		if (tuning_loop_counter-- == 0)
			break;

		if (cmd.cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200 &&
		    mmc->bus_width == 8)
			data.blocksize = 128;

		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
						    data.blocksize),
			     SDHCI_BLOCK_SIZE);
		sdhci_writew(host, data.blocks, SDHCI_BLOCK_COUNT);
		sdhci_writew(host, SDHCI_TRNS_READ, SDHCI_TRANSFER_MODE);

		mmc_send_cmd(mmc, &cmd, NULL);

		ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);

		if (cmd.cmdidx == MMC_CMD_SEND_TUNING_BLOCK)
			udelay(1);

	} while (ctrl & SDHCI_CTRL_EXEC_TUNING);

	if (tuning_loop_counter < 0) {
		ctrl &= ~SDHCI_CTRL_TUNED_CLK;
		sdhci_writel(host, ctrl, SDHCI_HOST_CONTROL2);
	}

	if (!(ctrl & SDHCI_CTRL_TUNED_CLK)) {
		printf("%s:Tuning failed\n", __func__);
		return -1;
	}

	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK,
		     SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, 0x0, SDHCI_SIGNAL_ENABLE);

	return 0;
}

static void am654_sdhci_set_control_reg(struct sdhci_host *host)
{
	struct mmc *mmc = (struct mmc *)host->mmc;
	u32 reg;

	if (IS_SD(host->mmc) &&
	    mmc->signal_voltage == MMC_SIGNAL_VOLTAGE_180) {
		reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		reg |= SDHCI_CTRL_VDD_180;
		sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
	}

	sdhci_set_uhs_timing(host);
}

static void sdhci_am654_setup_dll(struct sdhci_host *host, unsigned int speed)
{
	struct udevice *dev = host->mmc->dev;
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	int sel50, sel100, freqsel;
	u32 mask, val;
	int ret;

	if (plat->flags & FREQSEL_2_BIT) {
		switch (speed) {
		case 200000000:
			sel50 = 0;
			sel100 = 0;
			break;
		case 100000000:
			sel50 = 0;
			sel100 = 1;
			break;
		default:
			sel50 = 1;
			sel100 = 0;
		}

		/* Configure PHY DLL frequency */
		mask = SEL50_MASK | SEL100_MASK;
		val = (sel50 << SEL50_SHIFT) | (sel100 << SEL100_SHIFT);
		regmap_update_bits(plat->base, PHY_CTRL5, mask, val);
	} else {
		switch (speed) {
		case 200000000:
			freqsel = 0x0;
			break;
		default:
			freqsel = 0x4;
		}
		regmap_update_bits(plat->base, PHY_CTRL5, FREQSEL_MASK,
				   freqsel << FREQSEL_SHIFT);
	}

	/* Configure DLL TRIM */
	mask = DLL_TRIM_ICP_MASK;
	val = plat->trm_icp << DLL_TRIM_ICP_SHIFT;

	/* Configure DLL driver strength */
	mask |= DR_TY_MASK;
	val |= plat->drv_strength << DR_TY_SHIFT;
	regmap_update_bits(plat->base, PHY_CTRL1, mask, val);

	/* Enable DLL */
	regmap_update_bits(plat->base, PHY_CTRL1, ENDLL_MASK,
			   0x1 << ENDLL_SHIFT);
	/*
	 * Poll for DLL ready. Use a one second timeout.
	 * Works in all experiments done so far
	 */
	ret = regmap_read_poll_timeout(plat->base, PHY_STAT1, val,
				       val & DLLRDY_MASK, 1000, 1000000);
	if (ret) {
		dev_err(dev, "DLL failed to relock\n");
		return;
	}

	plat->dll_on = true;
}

static int am654_sdhci_set_ios_post(struct sdhci_host *host)
{
	struct udevice *dev = host->mmc->dev;
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	unsigned int speed = host->mmc->clock;
	int mode = host->mmc->selected_mode;
	u32 otap_del_sel;
	u32 mask, val;

	/* Reset SD Clock Enable */
	val = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
	val &= ~SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, val, SDHCI_CLOCK_CONTROL);

	/* power off phy */
	if (plat->dll_on) {
		regmap_update_bits(plat->base, PHY_CTRL1, ENDLL_MASK, 0);

		plat->dll_on = false;
	}

	/* restart clock */
	sdhci_set_clock(host->mmc, speed);

	/* switch phy back on */
	otap_del_sel = plat->otap_del_sel[mode];
	mask = OTAPDLYENA_MASK | OTAPDLYSEL_MASK;
	val = (1 << OTAPDLYENA_SHIFT) | (otap_del_sel << OTAPDLYSEL_SHIFT);

	/* Write to STRBSEL for HS400 speed mode */
	if (mode == MMC_HS_400) {
		if (plat->flags & STRBSEL_4_BIT)
			mask |= STRBSEL_4BIT_MASK;
		else
			mask |= STRBSEL_8BIT_MASK;

		val |= plat->strb_sel << STRBSEL_SHIFT;
	}

	regmap_update_bits(plat->base, PHY_CTRL4, mask, val);

	if (speed > AM654_SDHCI_MIN_FREQ && mode > UHS_SDR25) {
		regmap_update_bits(plat->base, PHY_CTRL5, SELDLYTXCLK_MASK, 0);
		sdhci_am654_setup_dll(host, speed);
	} else {
		regmap_update_bits(plat->base, PHY_CTRL5, SELDLYTXCLK_MASK,
				   1 << SELDLYTXCLK_SHIFT);
	}

	regmap_update_bits(plat->base, PHY_CTRL5, CLKBUFSEL_MASK,
			   plat->clkbuf_sel);

	return 0;
}

const struct sdhci_ops am654_sdhci_ops = {
	.set_ios_post		= &am654_sdhci_set_ios_post,
	.set_control_reg	= &am654_sdhci_set_control_reg,
	.platform_execute_tuning = &am654_sdhci_execute_tuning,
};

const struct am654_driver_data am654_drv_data = {
	.ops = &am654_sdhci_ops,
	.flags = DLL_PRESENT | IOMUX_PRESENT | FREQSEL_2_BIT | STRBSEL_4_BIT,
};

const struct am654_driver_data am654_sr1_drv_data = {
	.ops = &am654_sdhci_ops,
	.flags = DLL_PRESENT | DLL_CALIB | IOMUX_PRESENT | FREQSEL_2_BIT |
		 STRBSEL_4_BIT,
};

const struct am654_driver_data j721e_8bit_drv_data = {
	.ops = &am654_sdhci_ops,
	.flags = DLL_PRESENT | DLL_CALIB,
};

static int j721e_4bit_sdhci_set_ios_post(struct sdhci_host *host)
{
	struct udevice *dev = host->mmc->dev;
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	u32 otap_del_sel, mask, val;

	otap_del_sel = plat->otap_del_sel[host->mmc->selected_mode];
	mask = OTAPDLYENA_MASK | OTAPDLYSEL_MASK;
	val = (1 << OTAPDLYENA_SHIFT) | (otap_del_sel << OTAPDLYSEL_SHIFT);
	regmap_update_bits(plat->base, PHY_CTRL4, mask, val);
	regmap_update_bits(plat->base, PHY_CTRL5, CLKBUFSEL_MASK,
			   plat->clkbuf_sel);

	return 0;
}

const struct sdhci_ops j721e_4bit_sdhci_ops = {
	.set_ios_post		= &j721e_4bit_sdhci_set_ios_post,
	.set_control_reg	= &sdhci_set_control_reg,
	.platform_execute_tuning = &am654_sdhci_execute_tuning,
};

const struct am654_driver_data j721e_4bit_drv_data = {
	.ops = &j721e_4bit_sdhci_ops,
	.flags = IOMUX_PRESENT,
};

const struct soc_device_attribute am654_sdhci_soc_devices[] = {
	{ .family = "AM65X", .revision = "SR1.0", .data = &am654_sr1_drv_data},
	{/* sentinel */}
};

int am654_sdhci_init_phy(struct am654_sdhci_plat *plat)
{
	u32 ctl_cfg_2 = 0;
	u32 mask, val;
	int ret;

	/* Reset OTAP to default value */
	mask = OTAPDLYENA_MASK | OTAPDLYSEL_MASK;
	regmap_update_bits(plat->base, PHY_CTRL4, mask, 0x0);

	if (plat->flags & DLL_CALIB) {
		regmap_read(plat->base, PHY_STAT1, &val);
		if (~val & CALDONE_MASK) {
			/* Calibrate IO lines */
			regmap_update_bits(plat->base, PHY_CTRL1, PDB_MASK,
					   PDB_MASK);
			ret = regmap_read_poll_timeout(plat->base, PHY_STAT1,
						       val, val & CALDONE_MASK,
						       1, 20);
			if (ret)
				return ret;
		}
	}

	/* Enable pins by setting IO mux to 0 */
	if (plat->flags & IOMUX_PRESENT)
		regmap_update_bits(plat->base, PHY_CTRL1, IOMUX_ENABLE_MASK, 0);

	/* Set slot type based on SD or eMMC */
	if (plat->non_removable)
		ctl_cfg_2 = SLOTTYPE_EMBEDDED;

	regmap_update_bits(plat->base, CTL_CFG_2, SLOTTYPE_MASK, ctl_cfg_2);

	return 0;
}

static int sdhci_am654_get_otap_delay(struct udevice *dev,
				      struct mmc_config *cfg)
{
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	int ret;
	int i;

	/* ti,otap-del-sel-legacy is mandatory */
	ret = dev_read_u32(dev, "ti,otap-del-sel-legacy",
			   &plat->otap_del_sel[0]);
	if (ret)
		return ret;
	/*
	 * Remove the corresponding capability if an otap-del-sel
	 * value is not found
	 */
	for (i = MMC_HS; i <= MMC_HS_400; i++) {
		ret = dev_read_u32(dev, td[i].binding, &plat->otap_del_sel[i]);
		if (ret) {
			dev_dbg(dev, "Couldn't find %s\n", td[i].binding);
			/*
			 * Remove the corresponding capability
			 * if an otap-del-sel value is not found
			 */
			cfg->host_caps &= ~td[i].capability;
		}
	}

	return 0;
}

#define MAX_SDCD_DEBOUNCE_TIME 2000
int am654_sdhci_init(struct udevice *dev)
{
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	struct mmc *mmc = mmc_get_mmc_dev(dev);
	struct sdhci_host *host = mmc->priv;
	unsigned long start;
	int val;

	/*
	 * The controller takes about 1 second to debounce the card detect line
	 * and doesn't let us power on until that time is up. Instead of waiting
	 * for 1 second at every stage, poll on the CARD_PRESENT bit upto a
	 * maximum of 2 seconds to be safe..
	 */
	start = get_timer(0);
	do {
		if (get_timer(start) > MAX_SDCD_DEBOUNCE_TIME)
			return -ENOMEDIUM;

		val = mmc_getcd(host->mmc);
	} while (!val);

	am654_sdhci_init_phy(plat);

	return sdhci_init(mmc);
}

static int am654_sdhci_probe(struct udevice *dev)
{
	struct dm_mmc_ops *ops = mmc_get_ops(dev);
	struct am654_driver_data *drv_data =
			(struct am654_driver_data *)dev_get_driver_data(dev);
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	struct mmc_config *cfg = &plat->cfg;
	const struct soc_device_attribute *soc;
	const struct am654_driver_data *soc_drv_data;
	struct clk clk;
	unsigned long clock;
	int ret;

	ret = clk_get_by_name(dev, "clk_xin", &clk);
	if (ret) {
		dev_err(dev, "failed to get clock\n");
		return ret;
	}

	clock = clk_get_rate(&clk);
	if (IS_ERR_VALUE(clock)) {
		dev_err(dev, "failed to get rate\n");
		return clock;
	}

	host->max_clk = clock;
	host->mmc = &plat->mmc;
	host->mmc->dev = dev;
	ret = sdhci_setup_cfg(cfg, host, cfg->f_max,
			      AM654_SDHCI_MIN_FREQ);
	if (ret)
		return ret;

	ret = sdhci_am654_get_otap_delay(dev, cfg);
	if (ret)
		return ret;

	host->ops = drv_data->ops;

	/* Update ops based on SoC revision */
	soc = soc_device_match(am654_sdhci_soc_devices);
	if (soc && soc->data) {
		soc_drv_data = soc->data;
		host->ops = soc_drv_data->ops;
	}

	host->mmc->priv = host;
	upriv->mmc = host->mmc;

	regmap_init_mem_index(dev_ofnode(dev), &plat->base, 1);

	ops->init = am654_sdhci_init;

	return 0;
}

static int am654_sdhci_ofdata_to_platdata(struct udevice *dev)
{
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	struct mmc_config *cfg = &plat->cfg;
	u32 drv_strength;
	int ret;

	host->name = dev->name;
	host->ioaddr = (void *)dev_read_addr(dev);
	plat->non_removable = dev_read_bool(dev, "non-removable");

	if (plat->flags & DLL_PRESENT) {
		ret = dev_read_u32(dev, "ti,trm-icp", &plat->trm_icp);
		if (ret)
			return ret;

		ret = dev_read_u32(dev, "ti,driver-strength-ohm",
				   &drv_strength);
		if (ret)
			return ret;

		switch (drv_strength) {
		case 50:
			plat->drv_strength = DRIVER_STRENGTH_50_OHM;
			break;
		case 33:
			plat->drv_strength = DRIVER_STRENGTH_33_OHM;
			break;
		case 66:
			plat->drv_strength = DRIVER_STRENGTH_66_OHM;
			break;
		case 100:
			plat->drv_strength = DRIVER_STRENGTH_100_OHM;
			break;
		case 40:
			plat->drv_strength = DRIVER_STRENGTH_40_OHM;
			break;
		default:
			dev_err(dev, "Invalid driver strength\n");
			return -EINVAL;
		}
	}

	dev_read_u32(dev, "ti,strobe-sel", &plat->strb_sel);
	dev_read_u32(dev, "ti,clkbuf-sel", &plat->clkbuf_sel);

	ret = mmc_of_parse(dev, cfg);
	if (ret)
		return ret;

	return 0;
}

static int am654_sdhci_bind(struct udevice *dev)
{
	struct am654_driver_data *drv_data =
			(struct am654_driver_data *)dev_get_driver_data(dev);
	struct am654_sdhci_plat *plat = dev_get_platdata(dev);
	const struct soc_device_attribute *soc;
	const struct am654_driver_data *soc_drv_data;

	plat->flags = drv_data->flags;

	/* Update flags based on SoC revision */
	soc = soc_device_match(am654_sdhci_soc_devices);
	if (soc && soc->data) {
		soc_drv_data = soc->data;
		plat->flags = soc_drv_data->flags;
	}

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id am654_sdhci_ids[] = {
	{
		.compatible = "ti,am654-sdhci-5.1",
		.data = (ulong)&am654_drv_data,
	},
	{
		.compatible = "ti,j721e-sdhci-8bit",
		.data = (ulong)&j721e_8bit_drv_data,
	},
	{
		.compatible = "ti,j721e-sdhci-4bit",
		.data = (ulong)&j721e_4bit_drv_data,
	},
	{ }
};

U_BOOT_DRIVER(am654_sdhci_drv) = {
	.name		= "am654_sdhci",
	.id		= UCLASS_MMC,
	.of_match	= am654_sdhci_ids,
	.ofdata_to_platdata = am654_sdhci_ofdata_to_platdata,
	.ops		= &sdhci_ops,
	.bind		= am654_sdhci_bind,
	.probe		= am654_sdhci_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
	.platdata_auto_alloc_size = sizeof(struct am654_sdhci_plat),
};
