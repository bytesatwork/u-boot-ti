/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 bytes at work AG - https://www.bytesatwork.io
 *
 * Based on am62x_evm.h
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Suman Anna <s-anna@ti.com>
 */

#ifndef __CONFIG_AM62X_BYTEDEVKIT_H
#define __CONFIG_AM62X_BYTEDEVKIT_H

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1		0x880000000
#define CFG_SYS_SDRAM_BASE		0x80000000

/* baw config built in defaults */
#define BAW_CONFIG_BUILTIN_PCB 10
#define BAW_CONFIG_BUILTIN_RAM 14
#define BAW_CONFIG_BUILTIN_FLASH 3

#define CONFIG_BOOTCOMMAND \
	"if mmc dev 1; then " \
		"run mmc_boot; " \
	"else; " \
		"run emmc_boot; " \
	"fi; "

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=ttyS2,115200n8\0" \
	"dtbfile=" CONFIG_DEFAULT_DEVICE_TREE ".dtb\0" \
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
		"rootfstype=ext4 rootwait ${bootargs_append}; " \
	"\0" \
	"dtb_fixups=fdt addr ${dtbaddr}; fdt resize 0x10000; " \
		"setexpr second_mac gsub ':' ' ' $eth1addr; " \
		"fdt set /bus@f0000/ethernet@8000000/ethernet-ports/port@2 "\
			"mac-address [${second_mac}]" \
	"\0" \
	"mmc_boot=echo Booting from SD/MMC ...; run args_mmc; run generic_boot\0" \
	"emmc_boot=echo Booting from eMMC ...; run args_emmc; run generic_boot\0" \
	"generic_boot=" \
		"run args_generic; " \
		"load mmc ${mmc_dev}:${mmc_part} ${loadaddr} ${bootdir}/${kernelfile} || exit; " \
		"load mmc ${mmc_dev}:${mmc_part} ${dtbaddr} ${bootdir}/${dtbfile} || exit; " \
		"run dtb_fixups; " \
		"run enable_oldi; " \
		"booti ${loadaddr} - ${dtbaddr}; " \
	"\0" \
	"update_emmc=echo Writing bootloader to eMMC; " \
		"mmc dev 0 1; " \
		"load mmc 1 ${loadaddr} tiboot3.bin; " \
		"mmc write ${loadaddr} 0x0 0x400; " \
		"load mmc 1 ${loadaddr} tispl.bin; " \
		"mmc write ${loadaddr} 0x400 0xC00; " \
		"load mmc 1 ${loadaddr} u-boot.img; " \
		"mmc write ${loadaddr} 0x1000 0x1000; " \
		"mmc dev 0 0; " \
	"\0" \
	"enable_oldi=mw.l 0x00109008 0x68EF3490; mw.l 0x0010900C 0xD172BC5A; " \
		"mw.l 0x00108700 0x00000000; " \
	"\0" \
	"dfu_alt_info_ram=tispl.bin ram 0x80080000 0x200000;" \
		"u-boot.img ram 0x81000000 0x400000\0" \
	"\0"

#endif /* __CONFIG_AM62X_BYTEDEVKIT_H */
