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

#ifdef CONFIG_NAND
#define BOOT_ENV_SETTINGS	NAND_BOOT_ENV_SETTINGS
#define CONFIG_BOOTCOMMAND \
	"if mmc rescan; then " \
		"echo SD/MMC found on device ${mmc_dev}; " \
		"run nand_update; " \
		"run mmc_boot; " \
	"else " \
		"run nand_boot; " \
	"fi; "
#else
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

#ifdef CONFIG_NAND

/* NAND: device related configs */
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_PAGE_COUNT	(CONFIG_SYS_NAND_BLOCK_SIZE / \
					 CONFIG_SYS_NAND_PAGE_SIZE)
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	SZ_128K
/* NAND: driver related configs */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	NAND_LARGE_BADBLOCK_POS
#define CONFIG_SYS_NAND_ECCPOS		{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}

#define CONFIG_SYS_NAND_ECCSIZE		512
#define CONFIG_SYS_NAND_ECCBYTES	3
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_NAND_OMAP_ECCSCHEME	OMAP_ECC_HAM1_CODE_HW
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x00080000
/* NAND: SPL related configs */

#define MTDPARTS_DEFAULT		"mtdparts=omap2-nand.0:128k(SPL)," \
					"128k(SPL.backup1)," \
					"128k(SPL.backup2)," \
					"128k(SPL.backup3),1920k(u-boot)," \
					"128k(u-boot-env),512k(devtree),5m(kernel),-(rootfs)"

#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		0x260000
#define CONFIG_ENV_SIZE			SZ_128K
#define CONFIG_SYS_ENV_SECT_SIZE	CONFIG_SYS_NAND_BLOCK_SIZE
#else
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_OFFSET		SZ_128K
#define CONFIG_ENV_SIZE			SZ_8K
#endif

#define NAND_BOOT_ENV_SETTINGS \
	"mtd_parts=" MTDPARTS_DEFAULT "\0" \
	"nand_root=/dev/mtdblock8\0" \
	"nand_root_fs_type=jffs2\0" \
	"nand_spl_addr=   0x00000000\0" \
	"nand_spl_size=   0x00080000\0" \
	"nand_u-boot_addr=0x00080000\0" \
	"nand_u-boot_size=0x001e0000\0" \
	"nand_param_addr= 0x00260000\0" \
	"nand_param_size= 0x00020000\0" \
	"nand_dtb_addr=   0x00280000\0" \
	"nand_dtb_size=   0x00080000\0" \
	"nand_kernel_addr=0x00300000\0" \
	"nand_kernel_size=0x00500000\0" \
	"nand_rootfs_addr=0x00800000\0" \
	"nand_rootfs_size=0x0f800000\0" \
	"updatepath=/batw-sw\0" \
	"nand_args=setenv bootargs " \
		"console=${console} " \
		"vt.global_cursor_default=0 " \
		"${mtd_parts} " \
		"root=${nand_root} " \
		"rootfstype=${nand_root_fs_type} " \
		"rootwait " \
		"consoleblank=0 " \
		"; " \
	"\0" \
	"nand_boot=echo Booting from nand ...; " \
		"run nand_args; " \
		"nand read ${loadaddr} ${nand_kernel_addr} ${nand_kernel_size} || exit;" \
		"nand read ${dtbaddr} ${nand_dtb_addr} ${nand_dtb_size} || exit; " \
		"bootm ${loadaddr} - ${dtbaddr}; " \
	"\0" \
	"nand_update=echo Updating nand from mmc...; " \
		"if size mmc 0 ${updatepath}; then " \
			"if load mmc 0 ${loadaddr} ${updatepath}/MLO; then " \
				"echo Found new SPL, flash it.; "\
				"nand erase ${nand_spl_addr} ${nand_spl_size}; " \
				"nand write ${loadaddr} ${nand_spl_addr} ${filesize}; " \
			"fi; " \
			"if load mmc 0 ${loadaddr} ${updatepath}/u-boot.img; then " \
				"echo Found new U-Boot, flash it.; "\
				"nand erase ${nand_u-boot_addr} ${nand_u-boot_size}; " \
				"nand write ${loadaddr} ${nand_u-boot_addr} ${filesize}; " \
			"fi; " \
			"if load mmc 0 ${loadaddr} ${updatepath}/devtree.dtb; then " \
				"echo Found new device tree, flash it.; "\
				"nand erase ${nand_dtb_addr} ${nand_dtb_size}; " \
				"nand write ${loadaddr} ${nand_dtb_addr} ${filesize}; " \
			"fi; " \
			"if load mmc 0 ${loadaddr} ${updatepath}/uImage; then " \
				"echo Found new uImage, flash it.; "\
				"nand erase ${nand_kernel_addr} ${nand_kernel_size}; " \
				"nand write ${loadaddr} ${nand_kernel_addr} ${filesize}; " \
			"fi; " \
			"if load mmc 0 ${loadaddr} ${updatepath}/rootfs.jffs2; then " \
				"echo Found new rootfs, flash it.; "\
				"nand erase ${nand_rootfs_addr} ${nand_rootfs_size}; " \
				"nand write ${loadaddr} ${nand_rootfs_addr} ${filesize}; " \
			"fi; " \
			"echo Flashing done. Remove SD card before power cycling.; " \
		"else " \
			"echo Update path ${updatepath} not found.; " \
		"fi; " \
	"\0"

#endif

#ifdef CONFIG_SPI_FLASH

#ifndef CONFIG_SPL_BUILD
# define CONFIG_DM_SPI_FLASH
#endif

/* SPI Nor Flash */
#define CONFIG_SF_DEFAULT_SPEED		24000000
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
#define CONFIG_ENV_SIZE			SZ_64K
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SECT_SIZE		SZ_64K
#define CONFIG_ENV_OFFSET		(2112 * 1024)
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
