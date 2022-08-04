/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015 bytes at work AG
 */

#ifndef BAW_CONFIG_H
#define BAW_CONFIG_H

#include <config.h>
#include <linux/types.h>

enum baw_config_pcb {
	M2_PCB_REV_00 = 0,
	M2_PCB_REV_01 = 1,
	M2_PCB_REV_02 = 2,
	M2_PCB_REV_03 = 3,
	M2_PCB_REV_04 = 4,
	M5_PCB_REV_1_0,
	M5_PCB_REV_1_2,
	M6_PCB_REV_1_0,
	M6_PCB_REV_1_1,
	M8_PCB_REV_0_1,
	M8_PCB_REV_1_0,
};

enum baw_config_ram {
	M2_RAM_MT47H128M16RT25E = 1,
	M2_RAM_K4B2G1646EBIH9   = 2,
	M2_RAM_K4B2G1646QBCK0   = 3,
	M2_RAM_K4B4G1646DBIK0   = 4,
	M2_RAM_MT41K512M16HA107 = 5,
	M6_RAM_MT53E128M32D2DS_053,
	M6_RAM_MT53E256M32D2DS_053,
	M6_RAM_MT53E384M32D2DS_053,
	M6_RAM_MT53D512M32D2DS_053,
	M6_RAM_MT53E768M32D4DT_053,
	M6_RAM_MT53D1024M32D4DT_053,
	M8_RAM_MT53E256M16D1DS_046,
	M8_RAM_MT53D512M16D1DS_046,
	M8_RAM_MT53E1G16D1FW_046,
	M8_RAM_MT53E4G16D4NQ_046,
};

enum baw_config_flash {
	M2_NAND_2GBIT  = 0,
	M2_NAND_4GBIT  = 1,
	EMMC_4GB  = 2,
	EMMC_8GB  = 3,
	EMMC_16GB = 4,
	EMMC_32GB = 5,
};

struct baw_config {
	enum baw_config_pcb pcb;
	enum baw_config_ram ram;
	enum baw_config_flash flash;
	bool ext_avail;
	u32 artno;
	u32 lot;
	u8 lotseq;
	char proddate[12];
	char flashdate[6];
	char flashuser[6];
	bool track_avail;
	char macaddr[18];
	char uid[16];
};

struct baw_config_name_pair {
	u32 config;
	const char *name;
};

extern const struct baw_config_name_pair baw_config_pcb_name[];
extern const struct baw_config_name_pair baw_config_ram_name[];
extern const struct baw_config_name_pair baw_config_flash_name[];

const char *baw_config_get_name(u32 config, const struct baw_config_name_pair *map);

static inline const char *baw_config_get_pcb_name(enum baw_config_pcb config)
{
	return baw_config_get_name(config, baw_config_pcb_name);
}

static inline const char *baw_config_get_ram_name(enum baw_config_ram config)
{
	return baw_config_get_name(config, baw_config_ram_name);
}

static inline const char *baw_config_get_flash_name(enum baw_config_flash config)
{
	return baw_config_get_name(config, baw_config_flash_name);
}

#endif
