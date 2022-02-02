// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015 bytesatwork AG - https://www.bytesatwork.io
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
	u8 __maybe_unused reg = 0;

	if (baw_config_eeprom_read(config) == 0) {
		if (IS_ENABLED(CONFIG_SPL_BUILD)) {
			printf("Use EEPROM RAM config: %u (%s)\n", config->ram,
			       baw_config_get_ram_name(config->ram));
		}

		return;
	}

	if (IS_ENABLED(CONFIG_BAW_CONFIG_BUILTIN)) {
		config->ram = BAW_CONFIG_BUILTIN_RAM;
		if (IS_ENABLED(CONFIG_SPL_BUILD)) {
			printf("Use built in RAM config: %u (%s)\n", config->ram,
			       baw_config_get_ram_name(config->ram));

			return;
		}
	}

	if (IS_ENABLED(CONFIG_SPL_BUILD)) {
		printf("Error: no RAM configuration found");
	}
}
