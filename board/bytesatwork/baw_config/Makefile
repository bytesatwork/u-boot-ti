# SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
# Copyright (C) 2023 bytesatwork AG - https://www.bytesatwork.io

ifdef CONFIG_SPL_BUILD
obj-y += baw_config.o
obj-y += baw_config_get.o
else
ifneq ($(or $(CONFIG_CMD_BAW_CONFIG), $(CONFIG_BAW_CONFIG_BUILTIN), $(CONFIG_BAW_CONFIG_EEPROM)),)
obj-y += baw_config.o
endif
endif

obj-$(CONFIG_CMD_BAW_CONFIG) += cmd_baw_config.o
obj-$(CONFIG_BAW_CONFIG_EEPROM) += baw_config_eeprom.o
obj-$(CONFIG_BAW_CONFIG_BUILTIN) += baw_config_builtin.o
