# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (C) 2022 bytes at work AG - https://www.bytesatwork.io
#
# Based on ti/am62x/Makefile
# Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
#	Suman Anna <s-anna@ti.com>

obj-y	+= am62x_bytedevkit.o

obj-y += baw_config/baw_config.o
obj-y += baw_config/baw_config_get.o
obj-$(CONFIG_CMD_BAW_CONFIG) += baw_config/cmd_baw_config.o
obj-$(CONFIG_BAW_CONFIG_EEPROM) += baw_config/baw_config_eeprom.o
obj-$(CONFIG_BAW_CONFIG_BUILTIN) += baw_config/baw_config_builtin.o
