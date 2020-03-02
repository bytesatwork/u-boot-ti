// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2015 bytes at work AG
 *
 * Based on am335x_evm.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 */

#ifndef BYTEENGINE_AM335X_H
#define BYTEENGINE_AM335X_H

#include <configs/ti_am335x_common.h>
#include <linux/sizes.h>

#ifndef CONFIG_SPL_BUILD
# define CONFIG_TIMESTAMP
#endif

#define CONFIG_SYS_BOOTM_LEN		SZ_16M

/* Clock Defines */
#define V_OSCK				24000000 /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

#define M2CONFIG_BUILTIN_PCB	M2_PCB_REV_02
#define M2CONFIG_BUILTIN_RAM	M2_RAM_K4B2G1646EBIH9
#define M2CONFIG_BUILTIN_FLASH	M2_NAND_2GBIT

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1		0x44e09000	/* UART0 */
#define CONFIG_SYS_NS16550_COM2		0x48022000	/* UART1 */
#define CONFIG_SYS_NS16550_COM3		0x48024000	/* UART2 */
#define CONFIG_SYS_NS16550_COM4		0x481a6000	/* UART3 */
#define CONFIG_SYS_NS16550_COM5		0x481a8000	/* UART4 */
#define CONFIG_SYS_NS16550_COM6		0x481aa000	/* UART5 */

/* PMIC support */
#define CONFIG_POWER_TPS65910

/* Network */
#define CONFIG_PHY_ADDR			1

/*
 * Disable MMC DM for SPL build, it can be re-enabled after adding
 * DM support in SPL
 */
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_DM_MMC
#undef CONFIG_TIMER
#undef CONFIG_DM_USB
#endif

#ifdef CONFIG_SPI_FLASH
#define BOOT_ENV_SETTINGS	MMC_BOOT_ENV_SETTINGS
#define CONFIG_BOOTCOMMAND \
	"if mmc rescan; then " \
		"echo SD/MMC found on device ${mmc_dev}; " \
		"run mmc_boot; " \
	"else " \
		"run emmc_boot; " \
	"fi; "
#endif

#ifndef CONFIG_SPL_BUILD
#define CONFIG_EXTRA_ENV_SETTINGS \
	BOOT_ENV_SETTINGS \
	"mlofile=MLO\0" \
	"ubootfile=u-boot.img\0" \
	"kernelfile=uImage\0" \
	"dtbfile=devtree.dtb\0" \
	"fdt_high=0xffffffff\0" \
	"loadaddr=0x82000000\0" \
	"dtbaddr=0x83000000\0" \
	"console=ttyO0,115200n8\0" \
	"mmc_root=/dev/mmcblk0p2\0" \
	"mmc_args=setenv bootargs " \
		"console=${console} " \
		"vt.global_cursor_default=0 " \
		"${mtd_parts} " \
		"root=${mmc_root} " \
		"rootwait " \
		"consoleblank=0 " \
		"; " \
	"\0" \
	"mmc_boot=echo Booting from mmc ...; " \
		"run mmc_args; " \
		"load mmc 0 ${loadaddr} ${kernelfile} || exit; " \
		"load mmc 0 ${dtbaddr} ${dtbfile} || exit; " \
		"bootm ${loadaddr} - ${dtbaddr}; " \
	"\0"

#endif

#ifdef CONFIG_SPI_FLASH

#ifndef CONFIG_SPL_BUILD
# define CONFIG_DM_SPI_FLASH
#endif

/* SPI Nor Flash */

/*
 * Default to using SPI for environment for M25PE40.
 * 0x000000 - 0x010000 : SPL (64KiB)
 * 0x010000 - 0x060000 : U-Boot (320kiB)
 * 0x060000 - 0x080000 : U-Boot Environment (128KiB)
 */
#ifdef CONFIG_SPI_BOOT
/* SPL related */
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x10000

/* M25PE40: 64 KiB env size */
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#define MTDIDS_DEFAULT			"nor0=m25pe40-flash.0"
#define MTDPARTS_DEFAULT		"mtdparts=m25pe40-flash.0:64k(SPL)," \
					"2048k(u-boot),64k(u-boot-env1)"
#endif

#define MMC_BOOT_ENV_SETTINGS \
	"mlofilespi=MLO.byteswap\0" \
	"ubootoffset=0x10000\0" \
	"spiflashsize=0x1000000\0" \
	"bootsize=0x210000\0" \
	"emmc_root=/dev/mmcblk1p2\0" \
	"emmc_args=setenv bootargs " \
		"console=${console} " \
		"vt.global_cursor_default=0 " \
		"root=${emmc_root} " \
		"rootwait " \
		"consoleblank=0 " \
		"; " \
	"\0" \
	"emmc_boot=echo Booting from Emmc ...; " \
		"run emmc_args; " \
		"load mmc 1:1 ${loadaddr} ${kernelfile} || exit; " \
		"load mmc 1:1 ${dtbaddr} ${dtbfile} || exit; " \
		"bootm ${loadaddr} - ${dtbaddr}; " \
	"\0" \
	"update_spiflash=echo Updating SPI Flash ...; " \
		"sf probe 0; " \
		"sf erase 0 +${bootsize}; " \
		"mmc rescan; " \
		"load mmc 0 ${loadaddr} ${mlofilespi} || exit; "\
		"sf write ${loadaddr} 0 ${filesize}; " \
		"load mmc 0 ${loadaddr} ${ubootfile} || exit; " \
		"sf write ${loadaddr} ${ubootoffset} ${filesize}; " \
	"\0"

#endif

#endif
