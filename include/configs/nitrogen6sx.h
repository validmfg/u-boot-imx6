/*
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Boundary Devices Nitrogen6SX
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

#define CONFIG_MACH_TYPE	3769

#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE

/* M4 specific */
#define CONFIG_SYS_AUXCORE_BOOTDATA_DDR		0x9ff00000
#define CONFIG_SYS_AUXCORE_BOOTDATA_OCRAM	0x00910000
#define CONFIG_SYS_AUXCORE_BOOTDATA_TCM		0x007F8000
#define CONFIG_EXTRA_ENV_M4 \
	"m4image=m4_fw.bin\0" \
	"m4offset=0x1e0000\0" \
	"m4size=0x8000\0" \
	"loadm4image=load ${dtype} ${disk}:1 ${loadaddr} ${m4image}\0" \
	"m4update=for dtype in ${bootdevs}; do " \
		"for disk in 0 1 ; do ${dtype} dev ${disk} ;" \
			"if run loadm4image; then " \
				"sf probe; " \
				"sf erase ${m4offset} ${m4size}; " \
				"sf write ${loadaddr} ${m4offset} ${filesize}; " \
				"exit; " \
			"fi; " \
		"done; " \
		"done\0" \
	"m4loadaddr="__stringify(CONFIG_SYS_AUXCORE_BOOTDATA_TCM)"\0" \
	"m4boot=run m4boot_nor\0" \
	"m4boot_ext=load ${dtype} ${disk}:1 ${m4loadaddr} ${m4image}; " \
		"dcache flush; bootaux ${m4loadaddr}\0" \
	"m4boot_nor=sf probe; sf read ${m4loadaddr} ${m4offset} ${m4size}; " \
		"dcache flush; bootaux ${m4loadaddr}\0"

#define CONFIG_CMD_FBPANEL
#define CONFIG_PCIE_IMX_PERST_GPIO	IMX_GPIO_NR(4, 10)
#define CONFIG_RGMII1
#define CONFIG_RGMII2
#define CONFIG_SYS_FSL_ESDH_GPIO_WP
#define ENET_MDIO_BASE			ENET_BASE_ADDR

#define CONFIG_CI_UDC
#define CONFIG_CMD_SF
#define CONFIG_MXC_UART_BASE		UART1_BASE
#define CONFIG_PHY_ATHEROS
#define CONFIG_SYS_FSL_USDHC_NUM	2
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define BD_CONSOLE	"ttymxc0"
#define BD_I2C_MASK	7

#include "boundary.h"
#define CONFIG_EXTRA_ENV_SETTINGS BD_BOUNDARY_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_M4

#endif	       /* __CONFIG_H */
