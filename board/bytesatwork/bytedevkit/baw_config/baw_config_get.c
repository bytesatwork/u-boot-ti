// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015 bytes at work AG
 */

#include <linux/types.h>
#include <i2c.h>
#include <stdio.h>
#include <dm/uclass.h>
#include "baw_config_get.h"

#include "baw_config_builtin.h"
#include "baw_config_eeprom.h"

#define PMIC_ADDRESS 0x2D

void baw_config_get(struct baw_config *config)
{
	u8 __attribute__((unused)) reg = 0;

	if (baw_config_eeprom_read(config) == 0) {
		if (IS_ENABLED(CONFIG_SPL_BUILD)) {
			printf("Use EEPROM RAM config: %u (%s)\n", config->ram,
			       baw_config_get_ram_name(config->ram));
		}

		return;
	}

	if (IS_ENABLED(CONFIG_SPL_BUILD) && IS_ENABLED(CONFIG_TARGET_BYTEENGINE_AM335X) &&
	    IS_ENABLED(CONFIG_DM_I2C)) {
		struct udevice *pmic;
		struct udevice *bus;
		int ret;

		ret = uclass_get_device_by_seq(UCLASS_I2C, CONFIG_BAW_CONFIG_EEPROM_BUS, &bus);
		if (ret) {
			printf("%s(): uclass_get_device_by_seq(): %d\n", __func__, ret);
			return;
		}

		ret = dm_i2c_probe(bus, CONFIG_BAW_CONFIG_EEPROM_ADDRESS, 0, &pmic);
		if (ret) {
			printf("%s(): dm_i2c_probe(): %d\n", __func__, ret);
			return;
		}

		ret = i2c_set_chip_offset_len(pmic, 1);
		if (ret) {
			printf("%s(): i2c_set_chip_offset_len(): %d\n", __func__, ret);
			return;
		}

		ret = dm_i2c_read(pmic, 0x20, &reg, 1);
		if (ret) {
			printf("Error: PMIC read failed\n");
			goto default_config;
		} else {
			if (reg == 0x05) {
				config->ram = M2_RAM_MT47H128M16RT25E;	/* set DDR2 */
				printf("DDR2 detected\n");
			} else if (reg == 0x01) {
				config->ram = M2_RAM_K4B2G1646EBIH9;	/* set legacy DDR3 */
				printf("DDR3 detected\n");
			} else {
				goto default_config;
			}
		}

		return;
	}

default_config:
	if (IS_ENABLED(CONFIG_TARGET_BYTEENGINE_AM335X)) {
		config->ram = M2_RAM_K4B2G1646EBIH9;	/* set default to legacy DDR3 */
	} else if (IS_ENABLED(CONFIG_TARGET_IMX8MM_BYTEDEVKIT)) {
		if (IS_ENABLED(CONFIG_BAW_CONFIG_BUILTIN)) {
			config->ram = BAW_CONFIG_BUILTIN_RAM;
			if (IS_ENABLED(CONFIG_SPL_BUILD)) {
				printf("Use built in RAM config: %u (%s)\n", config->ram,
				       baw_config_get_ram_name(config->ram));

				return;
			}
		} else {
			/* use smallest RAM as fallback */
			config->ram = M6_RAM_MT53E128M32D2DS_053;
		}
	}

	if (IS_ENABLED(CONFIG_SPL_BUILD)) {
		printf("Error: no RAM configuration found, trying fallback config ");
		printf("%u (%s)\n", config->ram, baw_config_get_ram_name(config->ram));
	}
}
