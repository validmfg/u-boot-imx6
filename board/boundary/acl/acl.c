/*
 * Copyright (C) 2016, Boundary Devices <info@boundarydevices.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/fbpanel.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/sata.h>
#include <asm/imx-common/spi.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mxc_hdmi.h>
#include <i2c.h>
#include <input.h>
#include <netdev.h>
#include <splash.h>
#include <usb/ehci-fsl.h>

DECLARE_GLOBAL_DATA_PTR;

#define BUTTON_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define ENET_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define I2C_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define OUTPUT_40OHM	(PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm)

#define RGB_PAD_CTRL	PAD_CTL_DSE_120ohm

#define SPI_PAD_CTRL	(PAD_CTL_HYS | PAD_CTL_SPEED_MED |	\
	PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

#define UART_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_FAST)

#define USDHC_PAD_CTRL	(PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_FAST)

#define WEAK_PULLDN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define WEAK_PULLDN_OUTPUT (PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define WEAK_PULLUP_OUTPUT (PAD_CTL_PUS_100K_UP |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_SLOW)

static const iomux_v3_cfg_t init_pads[] = {
	/* Buzzer */
#define GP_GPIO3_CLKO2	IMX_GPIO_NR(1, 6)
	IOMUX_PAD_CTRL(GPIO_3__GPIO1_IO03, WEAK_PULLDN),

	/* ECSPI1 */
	IOMUX_PAD_CTRL(EIM_D17__ECSPI1_MISO, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D18__ECSPI1_MOSI, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D16__ECSPI1_SCLK, SPI_PAD_CTRL),
#define GP_ECSPI1_NOR_CS	IMX_GPIO_NR(3, 19)
	IOMUX_PAD_CTRL(EIM_D19__GPIO3_IO19, WEAK_PULLUP),

	/* ECSPI2 */
	IOMUX_PAD_CTRL(CSI0_DAT8__ECSPI2_SCLK, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(CSI0_DAT9__ECSPI2_MOSI, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(CSI0_DAT10__ECSPI2_MISO, SPI_PAD_CTRL),
#define GP_ECSPI2_NOR_CS0	IMX_GPIO_NR(5, 29)
	IOMUX_PAD_CTRL(CSI0_DAT11__GPIO5_IO29, WEAK_PULLUP),
#define GP_ECSPI2_NOR_CS1	IMX_GPIO_NR(2, 27)
	IOMUX_PAD_CTRL(EIM_LBA__GPIO2_IO27, WEAK_PULLUP),

	/* ECSPI4 */
	IOMUX_PAD_CTRL(EIM_D21__ECSPI4_SCLK, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D28__ECSPI4_MOSI, SPI_PAD_CTRL),

#ifndef CONFIG_MX6S
	/* ECSPI5 */
	IOMUX_PAD_CTRL(SD2_CLK__ECSPI5_SCLK, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(SD2_CMD__ECSPI5_MOSI, SPI_PAD_CTRL),
	IOMUX_PAD_CTRL(SD2_DAT0__ECSPI5_MISO, SPI_PAD_CTRL),
#define GP_ECSPI5_NOR_CS	IMX_GPIO_NR(1, 14)
	IOMUX_PAD_CTRL(SD2_DAT1__GPIO1_IO14, WEAK_PULLUP),
#endif

	/* ENET pads that don't change for PHY reset */
	IOMUX_PAD_CTRL(ENET_MDIO__ENET_MDIO, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(ENET_MDC__ENET_MDC, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TXC__RGMII_TXC, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TD0__RGMII_TD0, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TD1__RGMII_TD1, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TD2__RGMII_TD2, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TD3__RGMII_TD3, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_TX_CTL__RGMII_TX_CTL, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(ENET_REF_CLK__ENET_TX_CLK, ENET_PAD_CTRL),
#define GP_ENET_PHY_RESET_ACL	IMX_GPIO_NR(1, 27)
	IOMUX_PAD_CTRL(ENET_RXD0__GPIO1_IO27, WEAK_PULLUP),
#define GPIRQ_ENET_PHY		IMX_GPIO_NR(1, 28)
	IOMUX_PAD_CTRL(ENET_TX_EN__GPIO1_IO28, WEAK_PULLUP),

	/* Fan */
#define GPIRQ_FAN		IMX_GPIO_NR(7, 12)
	IOMUX_PAD_CTRL(GPIO_8__GPIO1_IO08, WEAK_PULLDN),
#define GP_FAN_ON		IMX_GPIO_NR(1, 8)
	IOMUX_PAD_CTRL(GPIO_17__GPIO7_IO12, WEAK_PULLDN),

	/* I2C2 RV4172 RTC */
#define GPIRQ_RTC_RV4162	IMX_GPIO_NR(4, 6)
	IOMUX_PAD_CTRL(KEY_COL0__GPIO4_IO06, WEAK_PULLUP),

	/* I2C3 ADS7924 ADC */
#define GPIRQ_ADC_INTR		IMX_GPIO_NR(3, 14)
	IOMUX_PAD_CTRL(EIM_DA14__GPIO3_IO14, WEAK_PULLUP),
#define GP_ADC_RESET		IMX_GPIO_NR(3, 15)
	IOMUX_PAD_CTRL(EIM_DA15__GPIO3_IO15, WEAK_PULLDN),

	/* I2C3 ADT75 Temperature */
#define GPIRQ_TEMP_ALERT	IMX_GPIO_NR(1, 4)
	IOMUX_PAD_CTRL(GPIO_4__GPIO1_IO04, WEAK_PULLUP),

	/* Motors */
#define GP_DOOR1		IMX_GPIO_NR(2, 24)
	IOMUX_PAD_CTRL(EIM_CS1__GPIO2_IO24, WEAK_PULLDN),
#define GP_MOTOR_HOME1A		IMX_GPIO_NR(2, 23)
	IOMUX_PAD_CTRL(EIM_CS0__GPIO2_IO23, WEAK_PULLDN),
#define GP_MOTOR_HOME1B		IMX_GPIO_NR(2, 25)
	IOMUX_PAD_CTRL(EIM_OE__GPIO2_IO25, WEAK_PULLDN),
#define GP_MOTOR_HOME2		IMX_GPIO_NR(2, 26)
	IOMUX_PAD_CTRL(EIM_RW__GPIO2_IO26, WEAK_PULLDN),
#define GP_STEP1_RESET		IMX_GPIO_NR(3, 8)
	IOMUX_PAD_CTRL(EIM_DA8__GPIO3_IO08, WEAK_PULLDN),
#define GP_STEP1_BUSY		IMX_GPIO_NR(3, 9)
	IOMUX_PAD_CTRL(EIM_DA9__GPIO3_IO09, WEAK_PULLDN),
#define GP_STEP1_FLAG		IMX_GPIO_NR(3, 10)
	IOMUX_PAD_CTRL(EIM_DA10__GPIO3_IO10, WEAK_PULLDN),
#define GP_STEP1_SW		IMX_GPIO_NR(3, 11)
	IOMUX_PAD_CTRL(EIM_DA11__GPIO3_IO11, WEAK_PULLDN),
#define GP_STEP2_RESET		IMX_GPIO_NR(2, 31)
	IOMUX_PAD_CTRL(EIM_EB3__GPIO2_IO31, WEAK_PULLDN),
#define GP_STEP2_BUSY		IMX_GPIO_NR(5, 0)
	IOMUX_PAD_CTRL(EIM_WAIT__GPIO5_IO00, WEAK_PULLDN),
#define GP_STEP2_FLAG		IMX_GPIO_NR(6, 31)
	IOMUX_PAD_CTRL(EIM_BCLK__GPIO6_IO31, WEAK_PULLDN),
#define GP_STEP2_SW		IMX_GPIO_NR(2, 28)
	IOMUX_PAD_CTRL(EIM_EB0__GPIO2_IO28, WEAK_PULLDN),

	/* PWM1 - Step motor 1 */
	IOMUX_PAD_CTRL(SD1_DAT3__GPIO1_IO21, WEAK_PULLDN),

	/* PWM2 - Step motor 2 */
	IOMUX_PAD_CTRL(SD1_DAT2__GPIO1_IO19, WEAK_PULLDN),

	/* PWM3 - Backlight on RGB connector: J15 */
#define GP_BACKLIGHT_RGB	IMX_GPIO_NR(1, 17)
	IOMUX_PAD_CTRL(SD1_DAT1__GPIO1_IO17, WEAK_PULLDN),

	/* PWM4 - Cartridge optics */
	IOMUX_PAD_CTRL(SD1_CMD__GPIO1_IO18, WEAK_PULLDN),

	/* reg_usbotg_vbus */
#define GP_REG_USBOTG		IMX_GPIO_NR(3, 22)
	IOMUX_PAD_CTRL(EIM_D22__GPIO3_IO22, WEAK_PULLDN),

	/* Printer */
#define GP_PRINT_BDCAY		IMX_GPIO_NR(2, 22)
	IOMUX_PAD_CTRL(EIM_A16__GPIO2_IO22, WEAK_PULLUP),
#define GP_PRINT_ADCAY		IMX_GPIO_NR(2, 21)
	IOMUX_PAD_CTRL(EIM_A17__GPIO2_IO21, WEAK_PULLUP),
#define GP_PRINT_TOFF		IMX_GPIO_NR(2, 20)
	IOMUX_PAD_CTRL(EIM_A18__GPIO2_IO20, WEAK_PULLUP),
#define GP_PRINT_ATE		IMX_GPIO_NR(2, 19)
	IOMUX_PAD_CTRL(EIM_A19__GPIO2_IO19, WEAK_PULLUP),
#define GP_PRINT_SLEEP_N	IMX_GPIO_NR(2, 18)
	IOMUX_PAD_CTRL(EIM_A20__GPIO2_IO18, WEAK_PULLUP),
#define GP_PRINT_BEN		IMX_GPIO_NR(2, 17)
	IOMUX_PAD_CTRL(EIM_A21__GPIO2_IO17, WEAK_PULLUP),
#define GP_PRINT_BPH		IMX_GPIO_NR(2, 16)
	IOMUX_PAD_CTRL(EIM_A22__GPIO2_IO16, WEAK_PULLUP),
#define GP_PRINT_AEN		IMX_GPIO_NR(6, 6)
	IOMUX_PAD_CTRL(EIM_A23__GPIO6_IO06, WEAK_PULLUP),
#define GP_PRINT_APH		IMX_GPIO_NR(5, 4)
	IOMUX_PAD_CTRL(EIM_A24__GPIO5_IO04, WEAK_PULLUP),
#define GP_PRINT_STROBE1	IMX_GPIO_NR(3, 0)
	IOMUX_PAD_CTRL(EIM_DA0__GPIO3_IO00, WEAK_PULLUP),
#define GP_PRINT_STROBE2	IMX_GPIO_NR(3, 1)
	IOMUX_PAD_CTRL(EIM_DA1__GPIO3_IO01, WEAK_PULLUP),
#define GP_PRINT_STROBE3	IMX_GPIO_NR(3, 2)
	IOMUX_PAD_CTRL(EIM_DA2__GPIO3_IO02, WEAK_PULLUP),
#define GP_PRINT_STROBE4	IMX_GPIO_NR(3, 3)
	IOMUX_PAD_CTRL(EIM_DA3__GPIO3_IO03, WEAK_PULLUP),
#define GP_PRINT_STROBE5	IMX_GPIO_NR(3, 4)
	IOMUX_PAD_CTRL(EIM_DA4__GPIO3_IO04, WEAK_PULLUP),
#define GP_PRINT_STROBE6	IMX_GPIO_NR(3, 5)
	IOMUX_PAD_CTRL(EIM_DA5__GPIO3_IO05, WEAK_PULLUP),
#define GP_PRINT_LATCH_B	IMX_GPIO_NR(3, 7)
	IOMUX_PAD_CTRL(EIM_DA7__GPIO3_IO07, WEAK_PULLUP),
#define GP_PRINT_ALERT		IMX_GPIO_NR(5, 20)
	IOMUX_PAD_CTRL(CSI0_DATA_EN__GPIO5_IO20, WEAK_PULLUP),
#define GP_PRINT_TRQ0		IMX_GPIO_NR(5, 22)
	IOMUX_PAD_CTRL(CSI0_DAT4__GPIO5_IO22, WEAK_PULLUP),
#define GP_PRINT_TRQ1		IMX_GPIO_NR(5, 23)
	IOMUX_PAD_CTRL(CSI0_DAT5__GPIO5_IO23, WEAK_PULLUP),
#define GP_PRINT_FAULT		IMX_GPIO_NR(5, 24)
	IOMUX_PAD_CTRL(CSI0_DAT6__GPIO5_IO24, WEAK_PULLUP),
#define GP_PRINT_DO		IMX_GPIO_NR(1, 13)
	IOMUX_PAD_CTRL(SD2_DAT2__GPIO1_IO13, WEAK_PULLUP),
#define GP_PAPER_OUT		IMX_GPIO_NR(1, 12)
	IOMUX_PAD_CTRL(SD2_DAT3__GPIO1_IO12, WEAK_PULLUP),
#define GP_STAT_LED1		IMX_GPIO_NR(3, 29)
	IOMUX_PAD_CTRL(EIM_D29__GPIO3_IO29, WEAK_PULLDN),
#define GP_STAT_LED2		IMX_GPIO_NR(2, 29)
	IOMUX_PAD_CTRL(EIM_EB1__GPIO2_IO29, WEAK_PULLDN),
#define GP_DRDY			IMX_GPIO_NR(7, 1)
	IOMUX_PAD_CTRL(SD3_DAT4__GPIO7_IO01, WEAK_PULLDN),

	/* Scanner */
#define GP_SCAN_AIM		IMX_GPIO_NR(5, 19)
	IOMUX_PAD_CTRL(CSI0_MCLK__GPIO5_IO19, WEAK_PULLDN),
#define GP_SCAN_TRIG		IMX_GPIO_NR(5, 18)
	IOMUX_PAD_CTRL(CSI0_PIXCLK__GPIO5_IO18, WEAK_PULLDN),
#define GP_SCAN_STA		IMX_GPIO_NR(6, 2)
	IOMUX_PAD_CTRL(CSI0_DAT16__GPIO6_IO02, WEAK_PULLDN),
#define GP_SCAN_DNLOAD		IMX_GPIO_NR(6, 3)
	IOMUX_PAD_CTRL(CSI0_DAT17__GPIO6_IO03, WEAK_PULLDN),

	/* Solenoids */
#define GP_SOLENOID1_ON		IMX_GPIO_NR(4, 5)
	IOMUX_PAD_CTRL(GPIO_19__GPIO4_IO05, WEAK_PULLUP),
#define GP_SOLENOID2_ON		IMX_GPIO_NR(1, 7)
	IOMUX_PAD_CTRL(GPIO_7__GPIO1_IO07, WEAK_PULLUP),
#define GP_SOLENOID3_ON		IMX_GPIO_NR(1, 2)
	IOMUX_PAD_CTRL(GPIO_2__GPIO1_IO02, WEAK_PULLUP),
#define GP_SPARE		IMX_GPIO_NR(1, 16)
	IOMUX_PAD_CTRL(SD1_DAT0__GPIO1_IO16, WEAK_PULLUP),
#define GPIRQ_SOLENOID_FAULT	IMX_GPIO_NR(7, 13)
	IOMUX_PAD_CTRL(GPIO_18__GPIO7_IO13, WEAK_PULLUP),

	/* Touch */
#define GP_TOUCH_WAKE		IMX_GPIO_NR(4, 11)
	IOMUX_PAD_CTRL(KEY_ROW2__GPIO4_IO11, WEAK_PULLUP),
#define GPIRQ_TOUCH		IMX_GPIO_NR(4, 10)
	IOMUX_PAD_CTRL(KEY_COL2__GPIO4_IO10, WEAK_PULLDN),

	/* UART1 */
	IOMUX_PAD_CTRL(SD3_DAT6__UART1_RX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_DAT7__UART1_TX_DATA, UART_PAD_CTRL),

	/* UART2 */
#ifndef CONFIG_SILENT_UART
	IOMUX_PAD_CTRL(EIM_D26__UART2_TX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D27__UART2_RX_DATA, UART_PAD_CTRL),
#else
	IOMUX_PAD_CTRL(EIM_D26__GPIO3_IO26, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D27__GPIO3_IO27, UART_PAD_CTRL),
#endif

	/* UART3 */
	IOMUX_PAD_CTRL(EIM_D24__UART3_TX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D25__UART3_RX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D23__UART3_CTS_B, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(EIM_D31__UART3_RTS_B, UART_PAD_CTRL),

	/* UART5 */
	IOMUX_PAD_CTRL(KEY_COL1__UART5_TX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(KEY_ROW1__UART5_RX_DATA, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(KEY_ROW4__UART5_CTS_B, UART_PAD_CTRL),
	IOMUX_PAD_CTRL(CSI0_DAT18__UART5_RTS_B, UART_PAD_CTRL),

	/* USBH1 */
	IOMUX_PAD_CTRL(EIM_D30__USB_H1_OC, WEAK_PULLUP),

	/* USBOTG */
	IOMUX_PAD_CTRL(GPIO_1__USB_OTG_ID, WEAK_PULLUP),
	IOMUX_PAD_CTRL(KEY_COL4__USB_OTG_OC, WEAK_PULLUP),

	/* USDHC3 - sdcard */
	IOMUX_PAD_CTRL(SD3_CLK__SD3_CLK, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_CMD__SD3_CMD, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_DAT0__SD3_DATA0, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_DAT1__SD3_DATA1, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_DAT2__SD3_DATA2, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD3_DAT3__SD3_DATA3, USDHC_PAD_CTRL),
#define GP_USDHC3_CD		IMX_GPIO_NR(7, 0)
	IOMUX_PAD_CTRL(SD3_DAT5__GPIO7_IO00, WEAK_PULLUP),

	/* USDHC4 - sdcard */
	IOMUX_PAD_CTRL(SD4_CLK__SD4_CLK, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_CMD__SD4_CMD, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT0__SD4_DATA0, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT1__SD4_DATA1, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT2__SD4_DATA2, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT3__SD4_DATA3, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT4__SD4_DATA4, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT5__SD4_DATA5, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT6__SD4_DATA6, USDHC_PAD_CTRL),
	IOMUX_PAD_CTRL(SD4_DAT7__SD4_DATA7, USDHC_PAD_CTRL),
#define GP_EMMC_RESET		IMX_GPIO_NR(2, 6)
	IOMUX_PAD_CTRL(NANDF_D6__GPIO2_IO06, WEAK_PULLUP),
};

static const iomux_v3_cfg_t enet_ar8035_gpio_pads[] = {
	/* pin 31 - 1 (1P8_SEL) on reset */
#define GP_AR8035_1P8_SEL	IMX_GPIO_NR(6, 30)
	IOMUX_PAD_CTRL(RGMII_RXC__GPIO6_IO30, WEAK_PULLUP_OUTPUT),
	/* pin 29 - 0 - AD0 */
#define GP_AR8035_AD0		IMX_GPIO_NR(6, 25)
	IOMUX_PAD_CTRL(RGMII_RD0__GPIO6_IO25, WEAK_PULLDN_OUTPUT),
	/* pin 28 - 1 - AD1 */
#define GP_AR8035_AD1		IMX_GPIO_NR(6, 27)
	IOMUX_PAD_CTRL(RGMII_RD1__GPIO6_IO27, WEAK_PULLUP_OUTPUT),

	/* mode = 1100 - plloff mode */
	/* pin 26 - 0 - MODE1 */
#define GP_AR8035_MODE1		IMX_GPIO_NR(6, 28)
	IOMUX_PAD_CTRL(RGMII_RD2__GPIO6_IO28, WEAK_PULLDN_OUTPUT),
	/* pin 25 - 1 - MODE3 */
#define GP_AR8035_MODE3		IMX_GPIO_NR(6, 29)
	IOMUX_PAD_CTRL(RGMII_RD3__GPIO6_IO29, WEAK_PULLUP_OUTPUT),
	/* pin 30 - 0 - MODE0 */
#define GP_AR8035_MODE0		IMX_GPIO_NR(6, 24)
	IOMUX_PAD_CTRL(RGMII_RX_CTL__GPIO6_IO24, WEAK_PULLDN_OUTPUT),
};

static const iomux_v3_cfg_t enet_pads[] = {
	IOMUX_PAD_CTRL(RGMII_RXC__RGMII_RXC, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_RD0__RGMII_RD0, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_RD1__RGMII_RD1, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_RD2__RGMII_RD2, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_RD3__RGMII_RD3, ENET_PAD_CTRL),
	IOMUX_PAD_CTRL(RGMII_RX_CTL__RGMII_RX_CTL, ENET_PAD_CTRL),
};

#ifdef CONFIG_CMD_FBPANEL
static const iomux_v3_cfg_t rgb666_pads[] = {
	IOMUX_PAD_CTRL(DI0_DISP_CLK__IPU1_DI0_DISP_CLK, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DI0_PIN15__IPU1_DI0_PIN15, RGB_PAD_CTRL),	/* DRDY */
	IOMUX_PAD_CTRL(DI0_PIN2__IPU1_DI0_PIN02, RGB_PAD_CTRL),		/* HSYNC */
	IOMUX_PAD_CTRL(DI0_PIN3__IPU1_DI0_PIN03, RGB_PAD_CTRL),		/* VSYNC */
	IOMUX_PAD_CTRL(DISP0_DAT0__IPU1_DISP0_DATA00, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT1__IPU1_DISP0_DATA01, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT2__IPU1_DISP0_DATA02, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT3__IPU1_DISP0_DATA03, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT4__IPU1_DISP0_DATA04, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT5__IPU1_DISP0_DATA05, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT6__IPU1_DISP0_DATA06, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT7__IPU1_DISP0_DATA07, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT8__IPU1_DISP0_DATA08, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT9__IPU1_DISP0_DATA09, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT10__IPU1_DISP0_DATA10, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT11__IPU1_DISP0_DATA11, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT12__IPU1_DISP0_DATA12, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT13__IPU1_DISP0_DATA13, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT14__IPU1_DISP0_DATA14, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT15__IPU1_DISP0_DATA15, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT16__IPU1_DISP0_DATA16, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT17__IPU1_DISP0_DATA17, RGB_PAD_CTRL),
};

static const iomux_v3_cfg_t rgb24_pads[] = {
	IOMUX_PAD_CTRL(DISP0_DAT18__IPU1_DISP0_DATA18, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT19__IPU1_DISP0_DATA19, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT20__IPU1_DISP0_DATA20, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT21__IPU1_DISP0_DATA21, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT22__IPU1_DISP0_DATA22, RGB_PAD_CTRL),
	IOMUX_PAD_CTRL(DISP0_DAT23__IPU1_DISP0_DATA23, RGB_PAD_CTRL),
};
#endif

static const iomux_v3_cfg_t rgb_gpio_pads[] = {
	IOMUX_PAD_CTRL(DI0_DISP_CLK__GPIO4_IO16, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DI0_PIN15__GPIO4_IO17, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DI0_PIN2__GPIO4_IO18, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DI0_PIN3__GPIO4_IO19, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT0__GPIO4_IO21, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT1__GPIO4_IO22, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT2__GPIO4_IO23, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT3__GPIO4_IO24, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT4__GPIO4_IO25, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT5__GPIO4_IO26, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT6__GPIO4_IO27, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT7__GPIO4_IO28, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT8__GPIO4_IO29, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT9__GPIO4_IO30, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT10__GPIO4_IO31, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT11__GPIO5_IO05, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT12__GPIO5_IO06, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT13__GPIO5_IO07, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT14__GPIO5_IO08, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT15__GPIO5_IO09, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT16__GPIO5_IO10, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT17__GPIO5_IO11, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT18__GPIO5_IO12, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT19__GPIO5_IO13, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT20__GPIO5_IO14, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT21__GPIO5_IO15, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT22__GPIO5_IO16, WEAK_PULLUP),
	IOMUX_PAD_CTRL(DISP0_DAT23__GPIO5_IO17, WEAK_PULLUP),
};

static struct i2c_pads_info i2c_pads[] = {
	/* I2C2 - touch / RTC / ADC */
	I2C_PADS_INFO_ENTRY(I2C2, KEY_COL3, 4, 12, KEY_ROW3, 4, 13, I2C_PAD_CTRL),
	/* I2C3 - ADC / temperature */
	I2C_PADS_INFO_ENTRY(I2C3, GPIO_5, 1, 05, GPIO_16, 7, 11, I2C_PAD_CTRL),
};

int dram_init(void)
{
	gd->ram_size = ((ulong)CONFIG_DDR_MB * 1024 * 1024);
	return 0;
}

#ifdef CONFIG_USB_EHCI_MX6
int board_ehci_power(int port, int on)
{
	if (port)
		return 0;
	gpio_set_value(GP_REG_USBOTG, on);
	return 0;
}
#endif

#ifdef CONFIG_FSL_ESDHC
static struct fsl_esdhc_cfg usdhc_cfg[] = {
	{.esdhc_base = USDHC3_BASE_ADDR, .max_bus_width = 4},
	{.esdhc_base = USDHC4_BASE_ADDR, .max_bus_width = 8},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;

	if (cfg->esdhc_base == USDHC4_BASE_ADDR)
		return 1;	/* eMMC always present */
	return !gpio_get_value(GP_USDHC3_CD);
}

int board_mmc_init(bd_t *bis)
{
	int ret;
	u32 index = 0;

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);

	for (index = 0; index < CONFIG_SYS_FSL_USDHC_NUM; ++index) {
		switch (index) {
		case 0:
			break;
		case 1:
			gpio_set_value(GP_EMMC_RESET, 1); /* release reset */
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) then supported by the board (%d)\n",
				index + 1, CONFIG_SYS_FSL_USDHC_NUM);
			return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[index]);
		if (ret)
			return ret;
	}
	return 0;
}
#endif

#ifdef CONFIG_MXC_SPI
int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == 0 && cs == 0) ? GP_ECSPI1_NOR_CS : -1;
}
#endif

static void setup_iomux_enet(int ksz)
{
	gpio_direction_output(GP_ENET_PHY_RESET_ACL, 0);
	gpio_direction_output(GP_AR8035_1P8_SEL, 1);
	gpio_direction_output(GP_AR8035_AD0, 0);
	gpio_direction_output(GP_AR8035_AD1, 1);
	gpio_direction_output(GP_AR8035_MODE1, 0);
	gpio_direction_output(GP_AR8035_MODE3, 1);
	gpio_direction_output(GP_AR8035_MODE0, 0);
	SETUP_IOMUX_PADS(enet_ar8035_gpio_pads);

	/* 1 ms minimum reset pulse */
	udelay(1000);
	gpio_set_value(GP_ENET_PHY_RESET_ACL, 1);
	/* strap hold time for AR8035, 5 fails, 6 works, so 12 should be safe */
	udelay(12);

	SETUP_IOMUX_PADS(enet_pads);
	udelay(100);	/* Wait 100 us before using mii interface */
}

int board_eth_init(bd_t *bis)
{
	uint32_t base = IMX_FEC_BASE;
	struct mii_dev *bus = NULL;
	struct phy_device *phydev = NULL;
	int ret;

	setup_iomux_enet(0);

#ifdef CONFIG_FEC_MXC
	bus = fec_get_miibus(base, -1);
	if (!bus)
		return -EINVAL;
	/* scan phy 4,5,6,7 */
	phydev = phy_find_by_mask(bus, (0xf << 4), PHY_INTERFACE_MODE_RGMII);
	if (!phydev) {
		ret = -EINVAL;
		goto free_bus;
	}
	printf("%s at %d\n", phydev->drv->name, phydev->addr);
	ret  = fec_probe(bis, -1, base, bus, phydev);
	if (ret)
		goto free_phydev;
#endif

#ifdef CONFIG_CI_UDC
	/* For otg ethernet*/
	if (!getenv("eth1addr"))
		setenv("eth1addr", getenv("usbnet_devaddr"));
	usb_eth_initialize(bis);
#endif
	return 0;

free_phydev:
	free(phydev);
free_bus:
	free(bus);
	return ret;
}


int splash_screen_prepare(void)
{
	char *env_loadsplash;

	if (!getenv("splashimage") || !getenv("splashsize")) {
		return -1;
	}

	env_loadsplash = getenv("loadsplash");
	if (env_loadsplash == NULL) {
		printf("Environment variable loadsplash not found!\n");
		return -1;
	}

	if (run_command_list(env_loadsplash, -1, 0)) {
		printf("failed to run loadsplash %s\n\n", env_loadsplash);
		return -1;
	}

	return 0;
}

#ifdef CONFIG_CMD_FBPANEL
void board_enable_lcd(const struct display_info_t *di, int enable)
{
	if (enable) {
		SETUP_IOMUX_PADS(rgb666_pads);
		if (di->pixfmt == IPU_PIX_FMT_RGB24)
			SETUP_IOMUX_PADS(rgb24_pads);
		mdelay(100); /* let panel sync up before enabling backlight */
		gpio_direction_output(GP_BACKLIGHT_RGB, enable);
	} else {
		gpio_direction_output(GP_BACKLIGHT_RGB, enable);
		SETUP_IOMUX_PADS(rgb_gpio_pads);
	}
}

void board_pre_enable(const struct display_info_t *di)
{
	SETUP_IOMUX_PADS(rgb666_pads);
}

static const struct display_info_t displays[] = {
	VD_ASIT500MA6F5D(LCD, NULL, 1, 0x40),
};

int board_cfb_skip(void)
{
	return NULL != getenv("novideo");
}
#endif

static const unsigned short gpios_out_low[] = {
	GP_ADC_RESET,
	GP_BACKLIGHT_RGB,
	GP_EMMC_RESET,
	GP_FAN_ON,
	GP_GPIO3_CLKO2,
	GP_PRINT_SLEEP_N,
	GP_PRINT_STROBE1,
	GP_PRINT_STROBE2,
	GP_PRINT_STROBE3,
	GP_PRINT_STROBE4,
	GP_PRINT_STROBE5,
	GP_PRINT_STROBE6,
	GP_REG_USBOTG,
	GP_SOLENOID1_ON,
	GP_SOLENOID2_ON,
	GP_SOLENOID3_ON,
	GP_STAT_LED1,
	GP_STAT_LED2,
	GP_STEP1_RESET,
	GP_STEP2_RESET,
	GP_SPARE,
};

static const unsigned short gpios_out_high[] = {
	GP_ECSPI1_NOR_CS,
	GP_ECSPI2_NOR_CS0,
	GP_ECSPI2_NOR_CS1,
#ifndef CONFIG_MX6S
	GP_ECSPI5_NOR_CS,
#endif
	GP_TOUCH_WAKE,
};

static const unsigned short gpios_in[] = {
	GPIRQ_ADC_INTR,
	GPIRQ_ENET_PHY,
	GPIRQ_FAN,
	GPIRQ_RTC_RV4162,
	GPIRQ_SOLENOID_FAULT,
	GPIRQ_TEMP_ALERT,
	GPIRQ_TOUCH,
	GP_DOOR1,
	GP_MOTOR_HOME1A,
	GP_MOTOR_HOME1B,
	GP_MOTOR_HOME2,
	GP_PAPER_OUT,
	GP_PRINT_ALERT,
	GP_PRINT_DO,
	GP_PRINT_FAULT,
	GP_PRINT_LATCH_B,
	GP_STEP1_BUSY,
	GP_STEP1_FLAG,
	GP_STEP1_SW,
	GP_STEP2_BUSY,
	GP_STEP2_FLAG,
	GP_STEP2_SW,
	GP_USDHC3_CD,
};

static void set_gpios_in(const unsigned short *p, int cnt)
{
	int i;

	for (i = 0; i < cnt; i++)
		gpio_direction_input(*p++);
}

static void set_gpios(const unsigned short *p, int cnt, int val)
{
	int i;

	for (i = 0; i < cnt; i++)
		gpio_direction_output(*p++, val);
}

int board_early_init_f(void)
{
	set_gpios_in(gpios_in, ARRAY_SIZE(gpios_in));
	set_gpios(gpios_out_high, ARRAY_SIZE(gpios_out_high), 1);
	set_gpios(gpios_out_low, ARRAY_SIZE(gpios_out_low), 0);
	SETUP_IOMUX_PADS(init_pads);
	SETUP_IOMUX_PADS(rgb_gpio_pads);
	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int board_init(void)
{
	int i;
	struct i2c_pads_info *p = i2c_pads + i2c_get_info_entry_offset();
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUXC_GPR1_OTG_ID_MASK,
			IOMUXC_GPR1_OTG_ID_GPIO1);

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;
	/* skip I2C1 init since unused */
	for (i = 1; i < 3; i++) {
	        setup_i2c(i, CONFIG_SYS_I2C_SPEED, 0x7f, p);
		p += I2C_PADS_INFO_ENTRY_SPACING;
	}
#ifdef CONFIG_CMD_SATA
	setup_sata();
#endif
#ifdef CONFIG_CMD_FBPANEL
	fbp_setup_display(displays, ARRAY_SIZE(displays));
#endif
	return 0;
}

int checkboard(void)
{
	puts("Board: ACL\n");
	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"mmc0",	MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
	/* 8 bit bus width */
	{"mmc1",	MAKE_CFGVAL(0x60, 0x58, 0x00, 0x00)},
	{NULL,		0},
};
#endif

int misc_init_r(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	setenv_hex("reset_cause", get_imx_reset_cause());
	return 0;
}

int board_late_init(void)
{
	int cpurev = get_cpu_rev();

	setenv("cpu", get_imx_type((cpurev & 0xFF000) >> 12));
	if (!getenv("board"))
		setenv("board", "acl");
	setenv("uboot_defconfig", CONFIG_DEFCONFIG);

	return 0;
}
