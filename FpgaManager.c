/*
 * Copyright (C) 2012 Altera Corporation <www.altera.com>
 * All rights reserved.
 *
 * This file contains only support functions used also by the SoCFPGA
 * platform code, the real meat is located in drivers/fpga/socfpga.c .
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */
/*
#include <common.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <asm/arch/fpga_manager.h>
#include <asm/arch/reset_manager.h>
#include <asm/arch/system_manager.h>
*/

#include "common.h"

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))


//DECLARE_GLOBAL_DATA_PTR;
#define SOCFPGA_FPGAMGRDATA_ADDRESS	0xffb90000
#define SOCFPGA_FPGAMGRREGS_ADDRESS	0xff706000

/* Timeout count */
#define FPGA_TIMEOUT_CNT		0x1000000

struct socfpga_system_manager {
	/* System Manager Module */
	u32	siliconid1;			/* 0x00 */
	u32	siliconid2;
	u32	_pad_0x8_0xf[2];
	u32	wddbg;				/* 0x10 */
	u32	bootinfo;
	u32	hpsinfo;
	u32	parityinj;
	/* FPGA Interface Group */
	u32	fpgaintfgrp_gbl;		/* 0x20 */
	u32	fpgaintfgrp_indiv;
	u32	fpgaintfgrp_module;
	u32	_pad_0x2c_0x2f;
	/* Scan Manager Group */
	u32	scanmgrgrp_ctrl;		/* 0x30 */
	u32	_pad_0x34_0x3f[3];
	/* Freeze Control Group */
	u32	frzctrl_vioctrl;		/* 0x40 */
	u32	_pad_0x44_0x4f[3];
	u32	frzctrl_hioctrl;		/* 0x50 */
	u32	frzctrl_src;
	u32	frzctrl_hwctrl;
	u32	_pad_0x5c_0x5f;
	/* EMAC Group */
	u32	emacgrp_ctrl;			/* 0x60 */
	u32	emacgrp_l3master;
	u32	_pad_0x68_0x6f[2];
	/* DMA Controller Group */
	u32	dmagrp_ctrl;			/* 0x70 */
	u32	dmagrp_persecurity;
	u32	_pad_0x78_0x7f[2];
	/* Preloader (initial software) Group */
	u32	iswgrp_handoff[8];		/* 0x80 */
	u32	_pad_0xa0_0xbf[8];		/* 0xa0 */
	/* Boot ROM Code Register Group */
	u32	romcodegrp_ctrl;		/* 0xc0 */
	u32	romcodegrp_cpu1startaddr;
	u32	romcodegrp_initswstate;
	u32	romcodegrp_initswlastld;
	u32	romcodegrp_bootromswstate;	/* 0xd0 */
	u32	__pad_0xd4_0xdf[3];
	/* Warm Boot from On-Chip RAM Group */
	u32	romcodegrp_warmramgrp_enable;	/* 0xe0 */
	u32	romcodegrp_warmramgrp_datastart;
	u32	romcodegrp_warmramgrp_length;
	u32	romcodegrp_warmramgrp_execution;
	u32	romcodegrp_warmramgrp_crc;	/* 0xf0 */
	u32	__pad_0xf4_0xff[3];
	/* Boot ROM Hardware Register Group */
	u32	romhwgrp_ctrl;			/* 0x100 */
	u32	_pad_0x104_0x107;
	/* SDMMC Controller Group */
	u32	sdmmcgrp_ctrl;
	u32	sdmmcgrp_l3master;
	/* NAND Flash Controller Register Group */
	u32	nandgrp_bootstrap;		/* 0x110 */
	u32	nandgrp_l3master;
	/* USB Controller Group */
	u32	usbgrp_l3master;
	u32	_pad_0x11c_0x13f[9];
	/* ECC Management Register Group */
	u32	eccgrp_l2;			/* 0x140 */
	u32	eccgrp_ocram;
	u32	eccgrp_usb0;
	u32	eccgrp_usb1;
	u32	eccgrp_emac0;			/* 0x150 */
	u32	eccgrp_emac1;
	u32	eccgrp_dma;
	u32	eccgrp_can0;
	u32	eccgrp_can1;			/* 0x160 */
	u32	eccgrp_nand;
	u32	eccgrp_qspi;
	u32	eccgrp_sdmmc;
	u32	_pad_0x170_0x3ff[164];
	/* Pin Mux Control Group */
	u32	emacio[20];			/* 0x400 */
	u32	flashio[12];			/* 0x450 */
	u32	generalio[28];			/* 0x480 */
	u32	_pad_0x4f0_0x4ff[4];
	u32	mixed1io[22];			/* 0x500 */
	u32	mixed2io[8];			/* 0x558 */
	u32	gplinmux[23];			/* 0x578 */
	u32	gplmux[71];			/* 0x5d4 */
	u32	nandusefpga;			/* 0x6f0 */
	u32	_pad_0x6f4;
	u32	rgmii1usefpga;			/* 0x6f8 */
	u32	_pad_0x6fc_0x700[2];
	u32	i2c0usefpga;			/* 0x704 */
	u32	sdmmcusefpga;			/* 0x708 */
	u32	_pad_0x70c_0x710[2];
	u32	rgmii0usefpga;			/* 0x714 */
	u32	_pad_0x718_0x720[3];
	u32	i2c3usefpga;			/* 0x724 */
	u32	i2c2usefpga;			/* 0x728 */
	u32	i2c1usefpga;			/* 0x72c */
	u32	spim1usefpga;			/* 0x730 */
	u32	_pad_0x734;
	u32	spim0usefpga;			/* 0x738 */
};

struct socfpga_fpga_manager {
	/* FPGA Manager Module */
	u32	stat;			/* 0x00 */
	u32	ctrl;
	u32	dclkcnt;
	u32	dclkstat;
	u32	gpo;			/* 0x10 */
	u32	gpi;
	u32	misci;			/* 0x18 */
	u32	_pad_0x1c_0x82c[517];

	/* Configuration Monitor (MON) Registers */
	u32	gpio_inten;		/* 0x830 */
	u32	gpio_intmask;
	u32	gpio_inttype_level;
	u32	gpio_int_polarity;
	u32	gpio_intstatus;		/* 0x840 */
	u32	gpio_raw_intstatus;
	u32	_pad_0x848;
	u32	gpio_porta_eoi;
	u32	gpio_ext_porta;		/* 0x850 */
	u32	_pad_0x854_0x85c[3];
	u32	gpio_1s_sync;		/* 0x860 */
	u32	_pad_0x864_0x868[2];
	u32	gpio_ver_id_code;
	u32	gpio_config_reg2;	/* 0x870 */
	u32	gpio_config_reg1;
};

#define FPGAMGRREGS_STAT_MODE_MASK		0x7
#define FPGAMGRREGS_STAT_MSEL_MASK		0xf8
#define FPGAMGRREGS_STAT_MSEL_LSB		3

#define FPGAMGRREGS_CTRL_CFGWDTH_MASK		0x200
#define FPGAMGRREGS_CTRL_AXICFGEN_MASK		0x100
#define FPGAMGRREGS_CTRL_NCONFIGPULL_MASK	0x4
#define FPGAMGRREGS_CTRL_NCE_MASK		0x2
#define FPGAMGRREGS_CTRL_EN_MASK		0x1
#define FPGAMGRREGS_CTRL_CDRATIO_LSB		6

#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CRC_MASK	0x8
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_ID_MASK	0x4
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_CD_MASK	0x2
#define FPGAMGRREGS_MON_GPIO_EXT_PORTA_NS_MASK	0x1

/* FPGA Mode */
#define FPGAMGRREGS_MODE_FPGAOFF		0x0
#define FPGAMGRREGS_MODE_RESETPHASE		0x1
#define FPGAMGRREGS_MODE_CFGPHASE		0x2
#define FPGAMGRREGS_MODE_INITPHASE		0x3
#define FPGAMGRREGS_MODE_USERMODE		0x4
#define FPGAMGRREGS_MODE_UNKNOWN		0x5

/* FPGA CD Ratio Value */
#define CDRATIO_x1				0x0
#define CDRATIO_x2				0x1
#define CDRATIO_x4				0x2
#define CDRATIO_x8				0x3


static struct socfpga_fpga_manager *fpgamgr_regs =
	(struct socfpga_fpga_manager *)SOCFPGA_FPGAMGRREGS_ADDRESS;
static struct socfpga_system_manager *sysmgr_regs =
	(struct socfpga_system_manager *)SOCFPGA_SYSMGR_ADDRESS;

/* Set CD ratio */
static void fpgamgr_set_cd_ratio(unsigned long ratio)
{
	clrsetbits_le32(&fpgamgr_regs->ctrl,
			0x3 << FPGAMGRREGS_CTRL_CDRATIO_LSB,
			(ratio & 0x3) << FPGAMGRREGS_CTRL_CDRATIO_LSB);
}

static int fpgamgr_dclkcnt_set(unsigned long cnt)
{
	unsigned long i;

	/* Clear any existing done status */
	if (readl(&fpgamgr_regs->dclkstat))
		writel(0x1, &fpgamgr_regs->dclkstat);

	/* Write the dclkcnt */
	writel(cnt, &fpgamgr_regs->dclkcnt);

	/* Wait till the dclkcnt done */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		if (!readl(&fpgamgr_regs->dclkstat))
			continue;

		writel(0x1, &fpgamgr_regs->dclkstat);
		return 0;
	}

	return -110 /*ETIMEDOUT*/;
}

/* Check whether FPGA Init_Done signal is high */
static int is_fpgamgr_initdone_high(void)
{
	unsigned long val;

	val = readl(&fpgamgr_regs->gpio_ext_porta);
	return val & FPGAMGRREGS_MON_GPIO_EXT_PORTA_ID_MASK;
}

/* Get the FPGA mode */
int fpgamgr_get_mode(void)
{
	unsigned long val;

	val = readl(&fpgamgr_regs->stat);
	return val & FPGAMGRREGS_STAT_MODE_MASK;
}

/* Check whether FPGA is ready to be accessed */
int fpgamgr_test_fpga_ready(void)
{
	/* Check for init done signal */
	if (!is_fpgamgr_initdone_high())
		return 0;

	/* Check again to avoid false glitches */
	if (!is_fpgamgr_initdone_high())
		return 0;

	if (fpgamgr_get_mode() != FPGAMGRREGS_MODE_USERMODE)
		return 0;

	return 1;
}

/* Poll until FPGA is ready to be accessed or timeout occurred */
int fpgamgr_poll_fpga_ready(void)
{
	unsigned long i;

	/* If FPGA is blank, wait till WD invoke warm reset */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		/* check for init done signal */
		if (!is_fpgamgr_initdone_high())
			continue;
		/* check again to avoid false glitches */
		if (!is_fpgamgr_initdone_high())
			continue;
		return 1;
	}

	return 0;
}

/* Ensure the FPGA entering user mode */
static int fpgamgr_program_poll_usermode(void)
{
	unsigned long i;

	/* Additional clocks for the CB to exit initialization phase */
	if (fpgamgr_dclkcnt_set(0x5000))
		return -7;

	/* (5) wait until FPGA enter user mode */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		if (fpgamgr_get_mode() == FPGAMGRREGS_MODE_USERMODE)
			break;
	}
	/* If not in configuration state, return error */
	if (i == FPGA_TIMEOUT_CNT)
		return -8;

	/* To release FPGA Manager drive over configuration line */
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_EN_MASK);

	return 0;
}

/* Start the FPGA programming by initialize the FPGA Manager */
static int fpgamgr_program_init(void)
{
	unsigned long msel, i;

	/* Get the MSEL value */
	msel = readl(&fpgamgr_regs->stat);
	msel &= FPGAMGRREGS_STAT_MSEL_MASK;
	msel >>= FPGAMGRREGS_STAT_MSEL_LSB;

	/*
	 * Set the cfg width
	 * If MSEL[3] = 1, cfg width = 32 bit
	 */
	if (msel & 0x8) {
		setbits_le32(&fpgamgr_regs->ctrl,
			     FPGAMGRREGS_CTRL_CFGWDTH_MASK);

		/* To determine the CD ratio */
		/* MSEL[1:0] = 0, CD Ratio = 1 */
		if ((msel & 0x3) == 0x0)
			fpgamgr_set_cd_ratio(CDRATIO_x1);
		/* MSEL[1:0] = 1, CD Ratio = 4 */
		else if ((msel & 0x3) == 0x1)
			fpgamgr_set_cd_ratio(CDRATIO_x4);
		/* MSEL[1:0] = 2, CD Ratio = 8 */
		else if ((msel & 0x3) == 0x2)
			fpgamgr_set_cd_ratio(CDRATIO_x8);

	} else {	/* MSEL[3] = 0 */
		clrbits_le32(&fpgamgr_regs->ctrl,
			     FPGAMGRREGS_CTRL_CFGWDTH_MASK);

		/* To determine the CD ratio */
		/* MSEL[1:0] = 0, CD Ratio = 1 */
		if ((msel & 0x3) == 0x0)
			fpgamgr_set_cd_ratio(CDRATIO_x1);
		/* MSEL[1:0] = 1, CD Ratio = 2 */
		else if ((msel & 0x3) == 0x1)
			fpgamgr_set_cd_ratio(CDRATIO_x2);
		/* MSEL[1:0] = 2, CD Ratio = 4 */
		else if ((msel & 0x3) == 0x2)
			fpgamgr_set_cd_ratio(CDRATIO_x4);
	}

	/* To enable FPGA Manager configuration */
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCE_MASK);

	/* To enable FPGA Manager drive over configuration line */
	setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_EN_MASK);

	/* Put FPGA into reset phase */
	setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCONFIGPULL_MASK);

	/* (1) wait until FPGA enter reset phase */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		if (fpgamgr_get_mode() == FPGAMGRREGS_MODE_RESETPHASE)
			break;
	}

	/* If not in reset state, return error */
	if (fpgamgr_get_mode() != FPGAMGRREGS_MODE_RESETPHASE) {
		FDy_puts("FPGA: Could not reset\n\r");
		return -1;
	}

	/* Release FPGA from reset phase */
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCONFIGPULL_MASK);

	/* (2) wait until FPGA enter configuration phase */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		if (fpgamgr_get_mode() == FPGAMGRREGS_MODE_CFGPHASE)
			break;
	}

	/* If not in configuration state, return error */
	if (fpgamgr_get_mode() != FPGAMGRREGS_MODE_CFGPHASE) {
		FDy_puts("FPGA: Could not configure\n\r");
		return -2;
	}

	/* Clear all interrupts in CB Monitor */
	writel(0xFFF, &fpgamgr_regs->gpio_porta_eoi);

	/* Enable AXI configuration */
	setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_AXICFGEN_MASK);

	return 0;
}

/* Write the RBF data to FPGA Manager */
static void fpgamgr_program_write(const void *rbf_data, unsigned long rbf_size)
{
	uint32_t src = (uint32_t)rbf_data;
	uint32_t dst = SOCFPGA_FPGAMGRDATA_ADDRESS;

	/* Number of loops for 32-byte long copying. */
	uint32_t loops32 = rbf_size / 32;
	/* Number of loops for 4-byte long copying + trailing bytes */
	uint32_t loops4 = DIV_ROUND_UP(rbf_size % 32, 4);

	asm volatile(
		"1:	ldmia	%0!,	{r0-r7}\n"
		"	stmia	%1!,	{r0-r7}\n"
		"	sub	%1,	#32\n"
		"	subs	%2,	#1\n"
		"	bne	1b\n"
		"	cmp	%3,	#0\n"
		"	beq	3f\n"
		"2:	ldr	%2,	[%0],	#4\n"
		"	str	%2,	[%1]\n"
		"	subs	%3,	#1\n"
		"	bne	2b\n"
		"3:	nop\n"
		: "+r"(src), "+r"(dst), "+r"(loops32), "+r"(loops4) :
		: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "cc");
}

/* Ensure the FPGA entering config done */
static int fpgamgr_program_poll_cd(void)
{
	const uint32_t mask = FPGAMGRREGS_MON_GPIO_EXT_PORTA_NS_MASK |
			      FPGAMGRREGS_MON_GPIO_EXT_PORTA_CD_MASK;
	unsigned long reg, i;

	/* (3) wait until full config done */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		reg = readl(&fpgamgr_regs->gpio_ext_porta);

		/* Config error */
		if (!(reg & mask)) {
			FDy_puts("FPGA: Configuration error.\n\r");
			return -3;
		}

		/* Config done without error */
		if (reg & mask)
			break;
	}

	/* Timeout happened, return error */
	if (i == FPGA_TIMEOUT_CNT) {
		FDy_puts("FPGA: Timeout waiting for program.\n\r");
		return -4;
	}

	/* Disable AXI configuration */
	clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_AXICFGEN_MASK);

	return 0;
}

/* Ensure the FPGA entering init phase */
static int fpgamgr_program_poll_initphase(void)
{
	unsigned long i;

	/* Additional clocks for the CB to enter initialization phase */
	if (fpgamgr_dclkcnt_set(0x4))
		return -5;

	/* (4) wait until FPGA enter init phase or user mode */
	for (i = 0; i < FPGA_TIMEOUT_CNT; i++) {
		if (fpgamgr_get_mode() == FPGAMGRREGS_MODE_INITPHASE)
			break;
		if (fpgamgr_get_mode() == FPGAMGRREGS_MODE_USERMODE)
			break;
	}

	/* If not in configuration state, return error */
	if (i == FPGA_TIMEOUT_CNT)
		return -6;

	return 0;
}

/*
 * FPGA Manager to program the FPGA. This is the interface used by FPGA driver.
 * Return 0 for sucess, non-zero for error.
 */
int socfpga_load(const void *rbf_data, size_t rbf_size)
{
	unsigned long status;

	if ((uint32_t)rbf_data & 0x3) 
	{
		FDy_puts("FPGA: Unaligned data, realign to 32bit boundary.\n");
		return -EINVAL;
	}

	/* Prior programming the FPGA, all bridges need to be shut off */

	/* Disable all signals from hps peripheral controller to fpga */
	writel(0, &sysmgr_regs->fpgaintfgrp_module);

	/* Disable all signals from FPGA to HPS SDRAM */
#define SDR_CTRLGRP_FPGAPORTRST_ADDRESS	0x5080
	writel(0, SOCFPGA_SDR_ADDRESS + SDR_CTRLGRP_FPGAPORTRST_ADDRESS);

	/* Disable all axi bridge (hps2fpga, lwhps2fpga & fpga2hps) */
	socfpga_bridges_reset(1);

	/* Unmap the bridges from NIC-301 */
	writel(0x1, SOCFPGA_L3REGS_ADDRESS);

	/* Initialize the FPGA Manager */
	status = fpgamgr_program_init();
	if (status)
		return status;

	/* Write the RBF data to FPGA Manager */
	fpgamgr_program_write(rbf_data, rbf_size);

	/* Ensure the FPGA entering config done */
	status = fpgamgr_program_poll_cd();
	if (status)
		return status;

	/* Ensure the FPGA entering init phase */
	status = fpgamgr_program_poll_initphase();
	if (status)
		return status;

	/* Ensure the FPGA entering user mode */
	return fpgamgr_program_poll_usermode();
}
