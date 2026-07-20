/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for K3 AM62L SoC family
 *
 * Copyright (C) 2026 bytesatwork AG - https://www.bytesatwork.io
 *
 * Based on include/configs/am62l3_evm.h
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
 *
 */

#ifndef __CONFIG_AM62LX_BYTEDEVKIT_H
#define __CONFIG_AM62LX_BYTEDEVKIT_H

#include <configs/ti_armv7_common.h>

/* NAND Driver config */
#define CFG_SYS_NAND_BASE            0x51000000

#define CFG_SYS_NAND_ECCPOS		{ 2, 3, 4, 5, 6, 7, 8, 9, \
					 10, 11, 12, 13, 14, 15, 16, 17, \
					 18, 19, 20, 21, 22, 23, 24, 25, \
					 26, 27, 28, 29, 30, 31, 32, 33, \
					 34, 35, 36, 37, 38, 39, 40, 41, \
					 42, 43, 44, 45, 46, 47, 48, 49, \
					 50, 51, 52, 53, 54, 55, 56, 57, }

#define CFG_SYS_NAND_ECCSIZE         512

#define CFG_SYS_NAND_ECCBYTES        14
/*-- end NAND config --*/

#define CFG_EXTRA_ENV_SETTINGS \
	"console=ttyS0,115200n8\0" \
	"dtbfile=k3-am62lx-bytedevkit.dtb\0" \
	"kernelfile=Image\0" \
	"loadaddr=0x82000000\0" \
	"dtbaddr=0x88000000\0" \
	"bootdir=/boot\0" \
	"mmc_dev=1\0" \
	"mmc_part=2\0" \
	"mmc_root=/dev/mmcblk1p2\0" \
	"args_mmc=setenv mmc_dev 1; setenv mmc_part 2;\0" \
	"args_emmc=setenv mmc_dev 0; setenv mmc_part 1;\0" \
	"args_generic=setenv bootargs console=${console} " \
		"earlycon=ns16550a,mmio32,0x02800000 ${mtdparts} " \
		"root=/dev/mmcblk${mmc_dev}p${mmc_part} " \
		"rootfstype=ext4 rootwait quiet ${bootargs_append}; " \
	"\0" \
	"mmc_boot=echo Booting from SD/MMC ...; run args_mmc; run generic_boot\0" \
	"emmc_boot=echo Booting from eMMC ...; run args_emmc; run generic_boot\0" \
	"generic_boot=" \
		"run args_generic; " \
		"load mmc ${mmc_dev}:${mmc_part} ${loadaddr} ${bootdir}/${kernelfile} || exit; " \
		"load mmc ${mmc_dev}:${mmc_part} ${dtbaddr} ${bootdir}/${dtbfile} || exit; " \
		"booti ${loadaddr} - ${dtbaddr}; " \
	"\0" \
	"update_emmc=echo Writing bootloader to eMMC; " \
		"mmc dev 0 1; " \
		"load mmc 1:2 ${loadaddr} tiboot3.bin; " \
		"mmc write ${loadaddr} 0x0 0x400; " \
		"load mmc 1:2 ${loadaddr} tispl.bin; " \
		"mmc write ${loadaddr} 0x400 0x1000; " \
		"load mmc 1:2 ${loadaddr} u-boot.img; " \
		"mmc write ${loadaddr} 0x1400 0xC00; " \
		"mmc dev 0 0; " \
	"\0" \
	"dfu_alt_info_ram=tispl.bin ram 0x80080000 0x200000;" \
		"u-boot.img ram 0x81000000 0x400000" \
	"\0" \
	"bsp_bootcmd=echo Running BSP bootcmd ...; " \
		"if mmc dev 1; then " \
			"run mmc_boot; " \
		"else; " \
			"run emmc_boot; " \
		"fi; " \
	"\0"

#endif /* __CONFIG_AM62LX_BYTEDEVKIT_H */
