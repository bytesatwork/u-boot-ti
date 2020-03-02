// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 bytes at work AG
 *
 * Based on board/ti/am335x/mux.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 */

#include <common.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mux.h>
#include <asm/io.h>
#include <i2c.h>
#include "board.h"

static struct module_pin_mux uart0_pin_mux[] = {
	{OFFSET(uart0_rxd), (MODE(0) | PULLUP_EN | RXACTIVE)},	/* UART0_RXD */
	{OFFSET(uart0_txd), (MODE(0) | PULLUDEN)},		/* UART0_TXD */
	{-1},
};

static struct module_pin_mux mmc0_pin_mux[] = {
	{OFFSET(mmc0_dat3), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT3 */
	{OFFSET(mmc0_dat2), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT2 */
	{OFFSET(mmc0_dat1), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT1 */
	{OFFSET(mmc0_dat0), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_DAT0 */
	{OFFSET(mmc0_clk), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | RXACTIVE | PULLUP_EN)},	/* MMC0_CMD */
	{-1},
};

#ifdef CONFIG_SPI_FLASH
static struct module_pin_mux mmc1_pin_mux[] = {
	{OFFSET(gpmc_ad7), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT7 */
	{OFFSET(gpmc_ad6), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT6 */
	{OFFSET(gpmc_ad5), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT5 */
	{OFFSET(gpmc_ad4), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT4 */
	{OFFSET(gpmc_ad3), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT3 */
	{OFFSET(gpmc_ad2), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT2 */
	{OFFSET(gpmc_ad1), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT1 */
	{OFFSET(gpmc_ad0), (MODE(1) | RXACTIVE | PULLUP_EN)},	/* MMC1_DAT0 */
	{OFFSET(gpmc_csn1), (MODE(2) | RXACTIVE | PULLUP_EN)},	/* MMC1_CLK */
	{OFFSET(gpmc_csn2), (MODE(2) | RXACTIVE | PULLUP_EN)},	/* MMC1_CMD */
	{-1},
};
#endif

static struct module_pin_mux i2c0_pin_mux[] = {
	{OFFSET(i2c0_sda), (MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL)},	/* I2C_DATA */
	{OFFSET(i2c0_scl), (MODE(0) | RXACTIVE | PULLUDEN | SLEWCTRL)},	/* I2C_SCLK */
	{-1},
};

static struct module_pin_mux mii1_pin_mux[] = {
	{OFFSET(mii1_txen), MODE(2)},			/* MII1_TXEN */
	{OFFSET(mii1_rxdv), MODE(2) | RXACTIVE},	/* MII1_RXDV */
	{OFFSET(mii1_txd3), MODE(2)},			/* MII1_TXD3 */
	{OFFSET(mii1_txd2), MODE(2)},			/* MII1_TXD2 */
	{OFFSET(mii1_txd1), MODE(2)},			/* MII1_TXD1 */
	{OFFSET(mii1_txd0), MODE(2)},			/* MII1_TXD0 */
	{OFFSET(mii1_txclk), MODE(2)},			/* MII1_TXCLK */
	{OFFSET(mii1_rxclk), MODE(2) | RXACTIVE},	/* MII1_RXCLK */
	{OFFSET(mii1_rxd3), MODE(2) | RXACTIVE},	/* MII1_RXD3 */
	{OFFSET(mii1_rxd2), MODE(2) | RXACTIVE},	/* MII1_RXD2 */
	{OFFSET(mii1_rxd1), MODE(2) | RXACTIVE},	/* MII1_RXD1 */
	{OFFSET(mii1_rxd0), MODE(2) | RXACTIVE},	/* MII1_RXD0 */
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN},	/* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},		/* MDIO_CLK */
	{OFFSET(uart1_rtsn), (MODE(7) | RXACTIVE | PULLUP_EN)},	/* PHY Reset */
	{OFFSET(gpmc_ad11), (MODE(7) | RXACTIVE | PULLUP_EN)},	/* PHY INT */
	{-1},
};

#ifdef CONFIG_SPI_FLASH
static struct module_pin_mux spi0_pin_mux[] = {
	{OFFSET(spi0_sclk), (MODE(0) | RXACTIVE | PULLUDEN)},			/* SPI0_SCLK */
	{OFFSET(spi0_d0), (MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN)},		/* SPI0_D0 */
	{OFFSET(spi0_d1), (MODE(0) | RXACTIVE | PULLUDEN)},			/* SPI0_D1 */
	{OFFSET(spi0_cs0), (MODE(0) | RXACTIVE | PULLUDEN | PULLUP_EN)},	/* SPI0_CS0 */
	{-1},
};
#endif

/* GPIO Muxing for Production Test */
static struct module_pin_mux gpio_pin_mux[] = {
	{OFFSET(gpmc_ad14), (MODE(7) | RXACTIVE)},
	{OFFSET(gpmc_ad12), (MODE(7) | RXACTIVE)},
	{OFFSET(gpmc_ad13), (MODE(7) | RXACTIVE)},
	{OFFSET(gpmc_ad10), (MODE(7) | RXACTIVE)},
	{OFFSET(gpmc_ad9), (MODE(7) | RXACTIVE)},
	{OFFSET(gpmc_ad8), (MODE(7) | RXACTIVE)},
	{OFFSET(uart1_ctsn), (MODE(7) | RXACTIVE)},
	{OFFSET(emu0), (MODE(7) | RXACTIVE)},
	{OFFSET(emu1), (MODE(7) | RXACTIVE)},
	{OFFSET(spi0_cs1), (MODE(7) | RXACTIVE)},
	{OFFSET(mcasp0_aclkx), (MODE(7) | RXACTIVE)},
	{OFFSET(uart0_ctsn), (MODE(7) | RXACTIVE)},
	{-1},
};

void enable_uart0_pin_mux(void)
{
	configure_module_pin_mux(uart0_pin_mux);
}

void enable_i2c0_pin_mux(void)
{
	configure_module_pin_mux(i2c0_pin_mux);
}

void enable_board_pin_mux(void)
{
	configure_module_pin_mux(mii1_pin_mux);
	configure_module_pin_mux(mmc0_pin_mux);
	configure_module_pin_mux(gpio_pin_mux);

#ifdef CONFIG_SPI_FLASH
	configure_module_pin_mux(spi0_pin_mux);
	configure_module_pin_mux(mmc1_pin_mux);
#endif
}
