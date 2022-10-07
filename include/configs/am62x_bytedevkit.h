/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 bytes at work AG - https://www.bytesatwork.io
 *
 * Based on am62x_evm.c
 * Copyright (C) 2020-2022 Texas Instruments Incorporated - https://www.ti.com/
 *	Suman Anna <s-anna@ti.com>
 */

#ifndef __CONFIG_AM62X_BYTEDEVKIT_H
#define __CONFIG_AM62X_BYTEDEVKIT_H

#include <linux/sizes.h>

/* DDR Configuration */
#define CONFIG_SYS_SDRAM_BASE1		0x880000000
#define CONFIG_SYS_BOOTM_LEN		SZ_64M

#ifdef CONFIG_SYS_K3_SPL_ATF
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"tispl.bin"
#endif

#define CONFIG_SYS_MALLOC_LEN		SZ_128M

/* baw config built in defaults */
#define BAW_CONFIG_BUILTIN_PCB 10
#define BAW_CONFIG_BUILTIN_RAM 14
#define BAW_CONFIG_BUILTIN_FLASH 3

#if defined(CONFIG_TARGET_AM62X_BYTEDEVKIT_A53)
#define CONFIG_SPL_MAX_SIZE		SZ_1M
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SPL_TEXT_BASE + SZ_4M)
#else
#define CONFIG_SPL_MAX_SIZE		CONFIG_SYS_K3_MAX_DOWNLODABLE_IMAGE_SIZE
/*
 * Maximum size in memory allocated to the SPL BSS. Keep it as tight as
 * possible (to allow the build to go through), as this directly affects
 * our memory footprint. The less we use for BSS the more we have available
 * for everything else.
 */
#define CONFIG_SPL_BSS_MAX_SIZE		0x5000
/*
 * Link BSS to be within SPL in a dedicated region located near the top of
 * the MCU SRAM, this way making it available also before relocation. Note
 * that we are not using the actual top of the MCU SRAM as there is a memory
 * location allocated for Device Manager data and some memory filled in by
 * the boot ROM that we want to read out without any interference from the
 * C context.
 */
#define CONFIG_SPL_BSS_START_ADDR	(0x43c3c800 -\
					 CONFIG_SPL_BSS_MAX_SIZE)
/* Set the stack right below the SPL BSS section */
#define CONFIG_SYS_INIT_SP_ADDR		0x7000ffff
/* Configure R5 SPL post-relocation malloc pool in DDR */
#define CONFIG_SYS_SPL_MALLOC_START	0x84000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_16M
#endif

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
	"mmc_boot=echo Booting from SD/MMC ...; run args_mmc; run generic_boot\0" \
	"emmc_boot=echo Booting from eMMC ...; run args_emmc; run generic_boot\0" \
	"generic_boot=" \
		"run args_generic; " \
		"load mmc ${mmc_dev}:${mmc_part} ${loadaddr} ${bootdir}/${kernelfile} || exit; " \
		"load mmc ${mmc_dev}:${mmc_part} ${dtbaddr} ${bootdir}/${dtbfile} || exit; " \
		"booti ${loadaddr} - ${dtbaddr}; " \
	"\0" \
	"update_spi=sf probe; " \
		"fatload mmc 1 ${loadaddr} tiboot3.bin; " \
		"sf update $loadaddr 0x0 $filesize; " \
		"fatload mmc 1 ${loadaddr} tispl.bin; " \
		"sf update $loadaddr 0x80000 $filesize; " \
		"fatload mmc 1 ${loadaddr} u-boot.img; " \
		"sf update $loadaddr 0x280000 $filesize; " \
	"\0"

/* The remaining common defines, source <configs/ti_armv7_common.h> */

/*
 * Our DDR memory always starts at 0x80000000 and U-Boot shall have
 * relocated itself to higher in memory by the time this value is used.
 * However, set this to a 32MB offset to allow for easier Linux kernel
 * booting as the default is often used as the kernel load address.
 */
#define CONFIG_SYS_LOAD_ADDR		0x82000000

/*
 * DDR information. If the CONFIG_NR_DRAM_BANKS is not defined,
 * we say (for simplicity) that we have 1 bank, always, even when
 * we have more. We always start at 0x80000000, and we place the
 * initial stack pointer in our SRAM. Otherwise, we can define
 * CONFIG_NR_DRAM_BANKS before including this file.
 */
#define CONFIG_SYS_SDRAM_BASE		0x80000000

#ifndef CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_INIT_SP_ADDR		(NON_SECURE_SRAM_END - \
						GENERATED_GBL_DATA_SIZE)
#endif

/* Timer information. */
#define CONFIG_SYS_PTV			2	/* Divisor: 2^(PTV+1) => 8 */

/* If DM_I2C, enable non-DM I2C support */
#if !defined(CONFIG_DM_I2C)
#define CONFIG_I2C
#define CONFIG_SYS_I2C
#endif

/*
 * The following are general good-enough settings for U-Boot. We set a
 * large malloc pool as we generally have a lot of DDR, and we opt for
 * function over binary size in the main portion of U-Boot as this is
 * generally easily constrained later if needed. We enable the config
 * options that give us information in the environment about what board
 * we are on so we do not need to rely on the command prompt. We set a
 * console baudrate of 115200 and use the default baud rate table.
 */

/* As stated above, the following choices are optional. */

/* We set the max number of command args high to avoid HUSH bugs. */
#define CONFIG_SYS_MAXARGS		64

/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		1024
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/*
 * When we have SPI, NOR or NAND flash we expect to be making use of
 * mtdparts, both for ease of use in U-Boot and for passing information
 * on to the Linux kernel.
 */

/*
 * Our platforms make use of SPL to initialize the hardware (primarily
 * memory) enough for full U-Boot to be loaded. We make use of the general
 * SPL framework found under common/spl/. Given our generally common memory
 * map, we set a number of related defaults and sizes here.
 */
#if !defined(CONFIG_NOR_BOOT) && \
	!(defined(CONFIG_QSPI_BOOT) && defined(CONFIG_AM43XX))

/*
 * We also support Falcon Mode so that the Linux kernel can be booted
 * directly from SPL. This is not currently available on HS devices.
 */

/*
 * Place the image at the start of the ROM defined image space (per
 * CONFIG_SPL_TEXT_BASE and we limit our size to the ROM-defined
 * downloaded image area minus 1KiB for scratch space. We initialize DRAM as
 * soon as we can so that we can place stack, malloc and BSS there. We load
 * U-Boot itself into memory at 0x80800000 for legacy reasons (to not conflict
 * with older SPLs). We have our BSS be placed 2MiB after this, to allow for
 * the default Linux kernel address of 0x80008000 to work with most sized
 * kernels, in the Falcon Mode case. We have the SPL malloc pool at the end
 * of the BSS area. We suggest that the stack be placed at 32MiB after the
 * start of DRAM to allow room for all of the above (handled in Kconfig).
 */
#ifndef CONFIG_SPL_BSS_START_ADDR
#define CONFIG_SPL_BSS_START_ADDR	0x80a00000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000		/* 512 KB */
#endif
#ifndef CONFIG_SYS_SPL_MALLOC_START
#define CONFIG_SYS_SPL_MALLOC_START	(CONFIG_SPL_BSS_START_ADDR + \
					 CONFIG_SPL_BSS_MAX_SIZE)
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_8M
#endif
#ifndef CONFIG_SPL_MAX_SIZE
#define CONFIG_SPL_MAX_SIZE		(SRAM_SCRATCH_SPACE_ADDR - \
					 CONFIG_SPL_TEXT_BASE)
#endif


/* FAT sd card locations. */
#ifndef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
#define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"u-boot.img"
#endif

#ifdef CONFIG_SPL_OS_BOOT
/* FAT */
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME		"uImage"
#define CONFIG_SPL_FS_LOAD_ARGS_NAME		"args"

/* RAW SD card / eMMC */
#define CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR	0x1700	/* address 0x2E0000 */
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	0x1500	/* address 0x2A0000 */
#define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS	0x200	/* 256KiB */
#endif

/* General parts of the framework, required. */

#ifdef CONFIG_MTD_RAW_NAND
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE
#endif
#endif /* !CONFIG_NOR_BOOT */

#endif /* __CONFIG_AM62X_BYTEDEVKIT_H */
