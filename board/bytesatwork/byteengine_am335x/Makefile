# SPDX-License-Identifier: GPL-2.0+
#
# Makefile
#
# Copyright (C) 2018 bytes at work AG
#
# Based on board/ti/am335x/Makefile
#
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/

ifeq ($(CONFIG_SKIP_LOWLEVEL_INIT),)
obj-y	:= mux.o
obj-y	+= ram.o
endif

obj-y	+= board.o
obj-y	+= m2config/m2config.o
obj-y	+= m2config/m2config_get.o
obj-$(CONFIG_M2CONFIG_EEPROM)	+= m2config/m2config_eeprom.o
obj-$(CONFIG_M2CONFIG_BUILTIN)	+= m2config/m2config_builtin.o
obj-$(CONFIG_CMD_M2CONFIG)	+= m2config/cmd_m2config.o
