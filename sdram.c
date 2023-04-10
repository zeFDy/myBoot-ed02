/*
 * Copyright Altera Corporation (C) 2014-2015
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
#include <common.h>
#include <errno.h>
#include <div64.h>
#include <watchdog.h>
#include <asm/arch/fpga_manager.h>
#include <asm/arch/sdram.h>
#include <asm/arch/system_manager.h>
#include <asm/io.h>
*/
#include "common.h"
#include "sdram.h"
#include "div64.h"
#define  UINT_MAX	(~0U)


/* Sequencer ac_rom_init configuration */
const u32 ac_rom_init[] = {
	0x20700000,
	0x20780000,
	0x10080471,
	0x10080570,
	0x10090006,
	0x100a0218,
	0x100b0000,
	0x10380400,
	0x10080469,
	0x100804e8,
	0x100a0006,
	0x10090218,
	0x100b0000,
	0x30780000,
	0x38780000,
	0x30780000,
	0x10680000,
	0x106b0000,
	0x10280400,
	0x10480000,
	0x1c980000,
	0x1c9b0000,
	0x1c980008,
	0x1c9b0008,
	0x38f80000,
	0x3cf80000,
	0x38780000,
	0x18180000,
	0x18980000,
	0x13580000,
	0x135b0000,
	0x13580008,
	0x135b0008,
	0x33780000,
	0x10580008,
	0x10780000
};


/* Sequencer inst_rom_init configuration */
const u32 inst_rom_init[] = {
	0x80000,
	0x80680,
	0x8180,
	0x8200,
	0x8280,
	0x8300,
	0x8380,
	0x8100,
	0x8480,
	0x8500,
	0x8580,
	0x8600,
	0x8400,
	0x800,
	0x8680,
	0x880,
	0xa680,
	0x80680,
	0x900,
	0x80680,
	0x980,
	0xa680,
	0x8680,
	0x80680,
	0xb68,
	0xcce8,
	0xae8,
	0x8ce8,
	0xb88,
	0xec88,
	0xa08,
	0xac88,
	0x80680,
	0xce00,
	0xcd80,
	0xe700,
	0xc00,
	0x20ce0,
	0x20ce0,
	0x20ce0,
	0x20ce0,
	0xd00,
	0x680,
	0x680,
	0x680,
	0x680,
	0x60e80,
	0x61080,
	0x61080,
	0x61080,
	0xa680,
	0x8680,
	0x80680,
	0xce00,
	0xcd80,
	0xe700,
	0xc00,
	0x30ce0,
	0x30ce0,
	0x30ce0,
	0x30ce0,
	0xd00,
	0x680,
	0x680,
	0x680,
	0x680,
	0x70e80,
	0x71080,
	0x71080,
	0x71080,
	0xa680,
	0x8680,
	0x80680,
	0x1158,
	0x6d8,
	0x80680,
	0x1168,
	0x7e8,
	0x7e8,
	0x87e8,
	0x40fe8,
	0x410e8,
	0x410e8,
	0x410e8,
	0x1168,
	0x7e8,
	0x7e8,
	0xa7e8,
	0x80680,
	0x40e88,
	0x41088,
	0x41088,
	0x41088,
	0x40f68,
	0x410e8,
	0x410e8,
	0x410e8,
	0xa680,
	0x40fe8,
	0x410e8,
	0x410e8,
	0x410e8,
	0x41008,
	0x41088,
	0x41088,
	0x41088,
	0x1100,
	0xc680,
	0x8680,
	0xe680,
	0x80680,
	0x0,
	0x8000,
	0xa000,
	0xc000,
	0x80000,
	0x80,
	0x8080,
	0xa080,
	0xc080,
	0x80080,
	0x9180,
	0x8680,
	0xa680,
	0x80680,
	0x40f08,
	0x80680
};


static const struct socfpga_sdram_config sdram_config = {
	.ctrl_cfg =
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_MEMTYPE <<
			SDR_CTRLGRP_CTRLCFG_MEMTYPE_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_MEMBL <<
			SDR_CTRLGRP_CTRLCFG_MEMBL_LSB)			|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_ADDRORDER <<
			SDR_CTRLGRP_CTRLCFG_ADDRORDER_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_ECCEN <<
			SDR_CTRLGRP_CTRLCFG_ECCEN_LSB)			|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_ECCCORREN <<
			SDR_CTRLGRP_CTRLCFG_ECCCORREN_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_REORDEREN <<
			SDR_CTRLGRP_CTRLCFG_REORDEREN_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_STARVELIMIT <<
			SDR_CTRLGRP_CTRLCFG_STARVELIMIT_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_DQSTRKEN <<
			SDR_CTRLGRP_CTRLCFG_DQSTRKEN_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_CTRLCFG_NODMPINS <<
			SDR_CTRLGRP_CTRLCFG_NODMPINS_LSB),
	.dram_timing1 =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_TCWL <<
			SDR_CTRLGRP_DRAMTIMING1_TCWL_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_AL <<
			SDR_CTRLGRP_DRAMTIMING1_TAL_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_TCL <<
			SDR_CTRLGRP_DRAMTIMING1_TCL_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_TRRD <<
			SDR_CTRLGRP_DRAMTIMING1_TRRD_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_TFAW <<
			SDR_CTRLGRP_DRAMTIMING1_TFAW_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING1_TRFC <<
			SDR_CTRLGRP_DRAMTIMING1_TRFC_LSB),
	.dram_timing2 =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING2_IF_TREFI <<
			SDR_CTRLGRP_DRAMTIMING2_TREFI_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING2_IF_TRCD <<
			SDR_CTRLGRP_DRAMTIMING2_TRCD_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING2_IF_TRP <<
			SDR_CTRLGRP_DRAMTIMING2_TRP_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING2_IF_TWR <<
			SDR_CTRLGRP_DRAMTIMING2_TWR_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING2_IF_TWTR <<
			SDR_CTRLGRP_DRAMTIMING2_TWTR_LSB),
	.dram_timing3 =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING3_TRTP <<
			SDR_CTRLGRP_DRAMTIMING3_TRTP_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING3_TRAS <<
			SDR_CTRLGRP_DRAMTIMING3_TRAS_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING3_TRC <<
			SDR_CTRLGRP_DRAMTIMING3_TRC_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING3_TMRD <<
			SDR_CTRLGRP_DRAMTIMING3_TMRD_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING3_TCCD <<
			SDR_CTRLGRP_DRAMTIMING3_TCCD_LSB),
	.dram_timing4 =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING4_SELFRFSHEXIT <<
			SDR_CTRLGRP_DRAMTIMING4_SELFRFSHEXIT_LSB)	|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMTIMING4_PWRDOWNEXIT <<
			SDR_CTRLGRP_DRAMTIMING4_PWRDOWNEXIT_LSB),
	.lowpwr_timing =
		(CONFIG_HPS_SDR_CTRLCFG_LOWPWRTIMING_AUTOPDCYCLES <<
			SDR_CTRLGRP_LOWPWRTIMING_AUTOPDCYCLES_LSB)	|
		(CONFIG_HPS_SDR_CTRLCFG_LOWPWRTIMING_CLKDISABLECYCLES <<
			SDR_CTRLGRP_LOWPWRTIMING_CLKDISABLECYCLES_LSB),
	.dram_odt =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMODT_READ <<
			SDR_CTRLGRP_DRAMODT_READ_LSB)			|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMODT_WRITE <<
			SDR_CTRLGRP_DRAMODT_WRITE_LSB),
	.extratime1 =
	(CONFIG_HPS_SDR_CTRLCFG_EXTRATIME1_CFG_EXTRA_CTL_CLK_RD_TO_WR <<
			SDR_CTRLGRP_EXTRATIME1_RD_TO_WR_LSB)		|
	(CONFIG_HPS_SDR_CTRLCFG_EXTRATIME1_CFG_EXTRA_CTL_CLK_RD_TO_WR_BC <<
			SDR_CTRLGRP_EXTRATIME1_RD_TO_WR_BC_LSB)		|
(CONFIG_HPS_SDR_CTRLCFG_EXTRATIME1_CFG_EXTRA_CTL_CLK_RD_TO_WR_DIFF_CHIP <<
			SDR_CTRLGRP_EXTRATIME1_RD_TO_WR_DIFF_LSB),
	.dram_addrw =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMADDRW_COLBITS <<
			SDR_CTRLGRP_DRAMADDRW_COLBITS_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMADDRW_ROWBITS <<
			SDR_CTRLGRP_DRAMADDRW_ROWBITS_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_DRAMADDRW_BANKBITS <<
			SDR_CTRLGRP_DRAMADDRW_BANKBITS_LSB)		|
		((CONFIG_HPS_SDR_CTRLCFG_DRAMADDRW_CSBITS - 1) <<
			SDR_CTRLGRP_DRAMADDRW_CSBITS_LSB),
	.dram_if_width =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMIFWIDTH_IFWIDTH <<
			SDR_CTRLGRP_DRAMIFWIDTH_IFWIDTH_LSB),
	.dram_dev_width =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMDEVWIDTH_DEVWIDTH <<
			SDR_CTRLGRP_DRAMDEVWIDTH_DEVWIDTH_LSB),
	.dram_intr =
		(CONFIG_HPS_SDR_CTRLCFG_DRAMINTR_INTREN <<
			SDR_CTRLGRP_DRAMINTR_INTREN_LSB),
	.lowpwr_eq =
		(CONFIG_HPS_SDR_CTRLCFG_LOWPWREQ_SELFRFSHMASK <<
			SDR_CTRLGRP_LOWPWREQ_SELFRFSHMASK_LSB),
	.static_cfg =
		(CONFIG_HPS_SDR_CTRLCFG_STATICCFG_MEMBL <<
			SDR_CTRLGRP_STATICCFG_MEMBL_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_STATICCFG_USEECCASDATA <<
			SDR_CTRLGRP_STATICCFG_USEECCASDATA_LSB),
	.ctrl_width =
		(CONFIG_HPS_SDR_CTRLCFG_CTRLWIDTH_CTRLWIDTH <<
			SDR_CTRLGRP_CTRLWIDTH_CTRLWIDTH_LSB),
	.cport_width =
		(CONFIG_HPS_SDR_CTRLCFG_CPORTWIDTH_CPORTWIDTH <<
			SDR_CTRLGRP_CPORTWIDTH_CMDPORTWIDTH_LSB),
	.cport_wmap =
		(CONFIG_HPS_SDR_CTRLCFG_CPORTWMAP_CPORTWMAP <<
			SDR_CTRLGRP_CPORTWMAP_CPORTWFIFOMAP_LSB),
	.cport_rmap =
		(CONFIG_HPS_SDR_CTRLCFG_CPORTRMAP_CPORTRMAP <<
			SDR_CTRLGRP_CPORTRMAP_CPORTRFIFOMAP_LSB),
	.rfifo_cmap =
		(CONFIG_HPS_SDR_CTRLCFG_RFIFOCMAP_RFIFOCMAP <<
			SDR_CTRLGRP_RFIFOCMAP_RFIFOCPORTMAP_LSB),
	.wfifo_cmap =
		(CONFIG_HPS_SDR_CTRLCFG_WFIFOCMAP_WFIFOCMAP <<
			SDR_CTRLGRP_WFIFOCMAP_WFIFOCPORTMAP_LSB),
	.cport_rdwr =
		(CONFIG_HPS_SDR_CTRLCFG_CPORTRDWR_CPORTRDWR <<
			SDR_CTRLGRP_CPORTRDWR_CPORTRDWR_LSB),
	.port_cfg =
		(CONFIG_HPS_SDR_CTRLCFG_PORTCFG_AUTOPCHEN <<
			SDR_CTRLGRP_PORTCFG_AUTOPCHEN_LSB),
	.fpgaport_rst = CONFIG_HPS_SDR_CTRLCFG_FPGAPORTRST,
	.fifo_cfg =
		(CONFIG_HPS_SDR_CTRLCFG_FIFOCFG_SYNCMODE <<
			SDR_CTRLGRP_FIFOCFG_SYNCMODE_LSB)		|
		(CONFIG_HPS_SDR_CTRLCFG_FIFOCFG_INCSYNC <<
			SDR_CTRLGRP_FIFOCFG_INCSYNC_LSB),
	.mp_priority =
		(CONFIG_HPS_SDR_CTRLCFG_MPPRIORITY_USERPRIORITY <<
			SDR_CTRLGRP_MPPRIORITY_USERPRIORITY_LSB),
	.mp_weight0 =
		(CONFIG_HPS_SDR_CTRLCFG_MPWIEIGHT_0_STATICWEIGHT_31_0 <<
			SDR_CTRLGRP_MPWEIGHT_MPWEIGHT_0_STATICWEIGHT_31_0_LSB),
	.mp_weight1 =
		(CONFIG_HPS_SDR_CTRLCFG_MPWIEIGHT_1_STATICWEIGHT_49_32 <<
			SDR_CTRLGRP_MPWEIGHT_MPWEIGHT_1_STATICWEIGHT_49_32_LSB) |
		(CONFIG_HPS_SDR_CTRLCFG_MPWIEIGHT_1_SUMOFWEIGHT_13_0 <<
			SDR_CTRLGRP_MPWEIGHT_MPWEIGHT_1_SUMOFWEIGHTS_13_0_LSB),
	.mp_weight2 =
		(CONFIG_HPS_SDR_CTRLCFG_MPWIEIGHT_2_SUMOFWEIGHT_45_14 <<
			SDR_CTRLGRP_MPWEIGHT_MPWEIGHT_2_SUMOFWEIGHTS_45_14_LSB),
	.mp_weight3 =
		(CONFIG_HPS_SDR_CTRLCFG_MPWIEIGHT_3_SUMOFWEIGHT_63_46 <<
			SDR_CTRLGRP_MPWEIGHT_MPWEIGHT_3_SUMOFWEIGHTS_63_46_LSB),
	.mp_pacing0 =
		(CONFIG_HPS_SDR_CTRLCFG_MPPACING_0_THRESHOLD1_31_0 <<
			SDR_CTRLGRP_MPPACING_MPPACING_0_THRESHOLD1_31_0_LSB),
	.mp_pacing1 =
		(CONFIG_HPS_SDR_CTRLCFG_MPPACING_1_THRESHOLD1_59_32 <<
			SDR_CTRLGRP_MPPACING_MPPACING_1_THRESHOLD1_59_32_LSB) |
		(CONFIG_HPS_SDR_CTRLCFG_MPPACING_1_THRESHOLD2_3_0 <<
			SDR_CTRLGRP_MPPACING_MPPACING_1_THRESHOLD2_3_0_LSB),
	.mp_pacing2 =
		(CONFIG_HPS_SDR_CTRLCFG_MPPACING_2_THRESHOLD2_35_4 <<
			SDR_CTRLGRP_MPPACING_MPPACING_2_THRESHOLD2_35_4_LSB),
	.mp_pacing3 =
		(CONFIG_HPS_SDR_CTRLCFG_MPPACING_3_THRESHOLD2_59_36 <<
			SDR_CTRLGRP_MPPACING_MPPACING_3_THRESHOLD2_59_36_LSB),
	.mp_threshold0 =
		(CONFIG_HPS_SDR_CTRLCFG_MPTHRESHOLDRST_0_THRESHOLDRSTCYCLES_31_0 <<
			SDR_CTRLGRP_MPTHRESHOLDRST_0_THRESHOLDRSTCYCLES_31_0_LSB),
	.mp_threshold1 =
		(CONFIG_HPS_SDR_CTRLCFG_MPTHRESHOLDRST_1_THRESHOLDRSTCYCLES_63_32 <<
			SDR_CTRLGRP_MPTHRESHOLDRST_1_THRESHOLDRSTCYCLES_63_32_LSB),
	.mp_threshold2 =
		(CONFIG_HPS_SDR_CTRLCFG_MPTHRESHOLDRST_2_THRESHOLDRSTCYCLES_79_64 <<
			SDR_CTRLGRP_MPTHRESHOLDRST_2_THRESHOLDRSTCYCLES_79_64_LSB),
	.phy_ctrl0 = CONFIG_HPS_SDR_CTRLCFG_PHYCTRL_PHYCTRL_0,
};

static const struct socfpga_sdram_rw_mgr_config rw_mgr_config = {
	.activate_0_and_1		= RW_MGR_ACTIVATE_0_AND_1,
	.activate_0_and_1_wait1		= RW_MGR_ACTIVATE_0_AND_1_WAIT1,
	.activate_0_and_1_wait2		= RW_MGR_ACTIVATE_0_AND_1_WAIT2,
	.activate_1			= RW_MGR_ACTIVATE_1,
	.clear_dqs_enable		= RW_MGR_CLEAR_DQS_ENABLE,
	.guaranteed_read		= RW_MGR_GUARANTEED_READ,
	.guaranteed_read_cont		= RW_MGR_GUARANTEED_READ_CONT,
	.guaranteed_write		= RW_MGR_GUARANTEED_WRITE,
	.guaranteed_write_wait0		= RW_MGR_GUARANTEED_WRITE_WAIT0,
	.guaranteed_write_wait1		= RW_MGR_GUARANTEED_WRITE_WAIT1,
	.guaranteed_write_wait2		= RW_MGR_GUARANTEED_WRITE_WAIT2,
	.guaranteed_write_wait3		= RW_MGR_GUARANTEED_WRITE_WAIT3,
	.idle				= RW_MGR_IDLE,
	.idle_loop1			= RW_MGR_IDLE_LOOP1,
	.idle_loop2			= RW_MGR_IDLE_LOOP2,
	.init_reset_0_cke_0		= RW_MGR_INIT_RESET_0_CKE_0,
	.init_reset_1_cke_0		= RW_MGR_INIT_RESET_1_CKE_0,
	.lfsr_wr_rd_bank_0		= RW_MGR_LFSR_WR_RD_BANK_0,
	.lfsr_wr_rd_bank_0_data		= RW_MGR_LFSR_WR_RD_BANK_0_DATA,
	.lfsr_wr_rd_bank_0_dqs		= RW_MGR_LFSR_WR_RD_BANK_0_DQS,
	.lfsr_wr_rd_bank_0_nop		= RW_MGR_LFSR_WR_RD_BANK_0_NOP,
	.lfsr_wr_rd_bank_0_wait		= RW_MGR_LFSR_WR_RD_BANK_0_WAIT,
	.lfsr_wr_rd_bank_0_wl_1		= RW_MGR_LFSR_WR_RD_BANK_0_WL_1,
	.lfsr_wr_rd_dm_bank_0		= RW_MGR_LFSR_WR_RD_DM_BANK_0,
	.lfsr_wr_rd_dm_bank_0_data	= RW_MGR_LFSR_WR_RD_DM_BANK_0_DATA,
	.lfsr_wr_rd_dm_bank_0_dqs	= RW_MGR_LFSR_WR_RD_DM_BANK_0_DQS,
	.lfsr_wr_rd_dm_bank_0_nop	= RW_MGR_LFSR_WR_RD_DM_BANK_0_NOP,
	.lfsr_wr_rd_dm_bank_0_wait	= RW_MGR_LFSR_WR_RD_DM_BANK_0_WAIT,
	.lfsr_wr_rd_dm_bank_0_wl_1	= RW_MGR_LFSR_WR_RD_DM_BANK_0_WL_1,
	.mrs0_dll_reset			= RW_MGR_MRS0_DLL_RESET,
	.mrs0_dll_reset_mirr		= RW_MGR_MRS0_DLL_RESET_MIRR,
	.mrs0_user			= RW_MGR_MRS0_USER,
	.mrs0_user_mirr			= RW_MGR_MRS0_USER_MIRR,
	.mrs1				= RW_MGR_MRS1,
	.mrs1_mirr			= RW_MGR_MRS1_MIRR,
	.mrs2				= RW_MGR_MRS2,
	.mrs2_mirr			= RW_MGR_MRS2_MIRR,
	.mrs3				= RW_MGR_MRS3,
	.mrs3_mirr			= RW_MGR_MRS3_MIRR,
	.precharge_all			= RW_MGR_PRECHARGE_ALL,
	.read_b2b			= RW_MGR_READ_B2B,
	.read_b2b_wait1			= RW_MGR_READ_B2B_WAIT1,
	.read_b2b_wait2			= RW_MGR_READ_B2B_WAIT2,
	.refresh_all			= RW_MGR_REFRESH_ALL,
	.rreturn			= RW_MGR_RETURN,
	.sgle_read			= RW_MGR_SGLE_READ,
	.zqcl				= RW_MGR_ZQCL,

	.true_mem_data_mask_width	= RW_MGR_TRUE_MEM_DATA_MASK_WIDTH,
	.mem_address_mirroring		= RW_MGR_MEM_ADDRESS_MIRRORING,
	.mem_data_mask_width		= RW_MGR_MEM_DATA_MASK_WIDTH,
	.mem_data_width			= RW_MGR_MEM_DATA_WIDTH,
	.mem_dq_per_read_dqs		= RW_MGR_MEM_DQ_PER_READ_DQS,
	.mem_dq_per_write_dqs		= RW_MGR_MEM_DQ_PER_WRITE_DQS,
	.mem_if_read_dqs_width		= RW_MGR_MEM_IF_READ_DQS_WIDTH,
	.mem_if_write_dqs_width		= RW_MGR_MEM_IF_WRITE_DQS_WIDTH,
	.mem_number_of_cs_per_dimm	= RW_MGR_MEM_NUMBER_OF_CS_PER_DIMM,
	.mem_number_of_ranks		= RW_MGR_MEM_NUMBER_OF_RANKS,
	.mem_virtual_groups_per_read_dqs =
		RW_MGR_MEM_VIRTUAL_GROUPS_PER_READ_DQS,
	.mem_virtual_groups_per_write_dqs =
		RW_MGR_MEM_VIRTUAL_GROUPS_PER_WRITE_DQS,
};

struct socfpga_sdram_io_config io_config = {
	.delay_per_dchain_tap		= IO_DELAY_PER_DCHAIN_TAP,
	.delay_per_dqs_en_dchain_tap	= IO_DELAY_PER_DQS_EN_DCHAIN_TAP,
	.delay_per_opa_tap		= IO_DELAY_PER_OPA_TAP,
	.dll_chain_length		= IO_DLL_CHAIN_LENGTH,
	.dqdqs_out_phase_max		= IO_DQDQS_OUT_PHASE_MAX,
	.dqs_en_delay_max		= IO_DQS_EN_DELAY_MAX,
	.dqs_en_delay_offset		= IO_DQS_EN_DELAY_OFFSET,
	.dqs_en_phase_max		= IO_DQS_EN_PHASE_MAX,
	.dqs_in_delay_max		= IO_DQS_IN_DELAY_MAX,
	.dqs_in_reserve			= IO_DQS_IN_RESERVE,
	.dqs_out_reserve		= IO_DQS_OUT_RESERVE,
	.io_in_delay_max		= IO_IO_IN_DELAY_MAX,
	.io_out1_delay_max		= IO_IO_OUT1_DELAY_MAX,
	.io_out2_delay_max		= IO_IO_OUT2_DELAY_MAX,
	.shift_dqs_en_when_shift_dqs	= IO_SHIFT_DQS_EN_WHEN_SHIFT_DQS,
};

struct socfpga_sdram_misc_config misc_config = {
	.afi_rate_ratio			= AFI_RATE_RATIO,
	.calib_lfifo_offset		= CALIB_LFIFO_OFFSET,
	.calib_vfifo_offset		= CALIB_VFIFO_OFFSET,
	.enable_super_quick_calibration	= ENABLE_SUPER_QUICK_CALIBRATION,
	.max_latency_count_width	= MAX_LATENCY_COUNT_WIDTH,
	.read_valid_fifo_size		= READ_VALID_FIFO_SIZE,
	.reg_file_init_seq_signature	= REG_FILE_INIT_SEQ_SIGNATURE,
	.tinit_cntr0_val		= TINIT_CNTR0_VAL,
	.tinit_cntr1_val		= TINIT_CNTR1_VAL,
	.tinit_cntr2_val		= TINIT_CNTR2_VAL,
	.treset_cntr0_val		= TRESET_CNTR0_VAL,
	.treset_cntr1_val		= TRESET_CNTR1_VAL,
	.treset_cntr2_val		= TRESET_CNTR2_VAL,
};


const struct socfpga_sdram_config *socfpga_get_sdram_config(void)
{
	return &sdram_config;
}

void socfpga_get_seq_ac_init(const u32 **init, unsigned int *nelem)
{
	*init = ac_rom_init;
	*nelem = ARRAY_SIZE(ac_rom_init);
}

void socfpga_get_seq_inst_init(const u32 **init, unsigned int *nelem)
{
	*init = inst_rom_init;
	*nelem = ARRAY_SIZE(inst_rom_init);
}

const struct socfpga_sdram_rw_mgr_config *socfpga_get_sdram_rwmgr_config(void)
{
	return &rw_mgr_config;
}

const struct socfpga_sdram_io_config *socfpga_get_sdram_io_config(void)
{
	return &io_config;
}

const struct socfpga_sdram_misc_config *socfpga_get_sdram_misc_config(void)
{
	return &misc_config;
}

/*
 * hweightN: returns the hamming weight (i.e. the number
 * of bits set) of a N-bit word
 */

static inline unsigned int generic_hweight32(unsigned int w)
{
	unsigned int res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
	res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
	res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
	res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
	return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}

/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
static inline int generic_fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline int __ilog2(unsigned int x)
{
	return generic_fls(x) - 1;
}

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


//DECLARE_GLOBAL_DATA_PTR;

struct sdram_prot_rule {
	u32	sdram_start;	/* SDRAM start address */
	u32	sdram_end;	/* SDRAM end address */
	u32	rule;		/* SDRAM protection rule number: 0-19 */
	int	valid;		/* Rule valid or not? 1 - valid, 0 not*/

	u32	security;
	u32	portmask;
	u32	result;
	u32	lo_prot_id;
	u32	hi_prot_id;
};

static struct socfpga_system_manager 	*sysmgr_regs =	(struct socfpga_system_manager *)SOCFPGA_SYSMGR_ADDRESS;
static struct socfpga_sdr_ctrl 			*sdr_ctrl 	=	(struct socfpga_sdr_ctrl *)SDR_CTRLGRP_ADDRESS;


/**
 * sdram_calculate_size() - Calculate SDRAM size
 *
 * Calculate SDRAM device size based on SDRAM controller parameters.
 * Size is specified in bytes.
 */
unsigned long sdram_calculate_size(void)
{
	unsigned long temp;
	unsigned long row, bank, col, cs, width;
	const struct socfpga_sdram_config *cfg = socfpga_get_sdram_config();
	const unsigned int csbits =
		((cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_CSBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_CSBITS_LSB) + 1;
	const unsigned int rowbits =
		(cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_ROWBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_ROWBITS_LSB;

	temp = readl(&sdr_ctrl->dram_addrw);
	col = (temp & SDR_CTRLGRP_DRAMADDRW_COLBITS_MASK) >>
		SDR_CTRLGRP_DRAMADDRW_COLBITS_LSB;

	/*
	 * SDRAM Failure When Accessing Non-Existent Memory
	 * Use ROWBITS from Quartus/QSys to calculate SDRAM size
	 * since the FB specifies we modify ROWBITs to work around SDRAM
	 * controller issue.
	 */
	row = readl(&sysmgr_regs->iswgrp_handoff[4]);
	if (row == 0)
		row = rowbits;
	/*
	 * If the stored handoff value for rows is greater than
	 * the field width in the sdr.dramaddrw register then
	 * something is very wrong. Revert to using the the #define
	 * value handed off by the SOCEDS tool chain instead of
	 * using a broken value.
	 */
	if (row > 31)
		row = rowbits;

	bank = (temp & SDR_CTRLGRP_DRAMADDRW_BANKBITS_MASK) >>
		SDR_CTRLGRP_DRAMADDRW_BANKBITS_LSB;

	/*
	 * SDRAM Failure When Accessing Non-Existent Memory
	 * Use CSBITs from Quartus/QSys to calculate SDRAM size
	 * since the FB specifies we modify CSBITs to work around SDRAM
	 * controller issue.
	 */
	cs = csbits;

	width = readl(&sdr_ctrl->dram_if_width);

	/* ECC would not be calculated as its not addressible */
	if (width == SDRAM_WIDTH_32BIT_WITH_ECC)
		width = 32;
	if (width == SDRAM_WIDTH_16BIT_WITH_ECC)
		width = 16;

	/* calculate the SDRAM size base on this info */
	temp = 1 << (row + bank + col);
	temp = temp * cs * (width  / 8);

	//debug("%s returns %ld\n", __func__, temp);

	return temp;
}

/**
 * get_errata_rows() - Up the number of DRAM rows to cover entire address space
 * @cfg:	SDRAM controller configuration data
 *
 * SDRAM Failure happens when accessing non-existent memory. Artificially
 * increase the number of rows so that the memory controller thinks it has
 * 4GB of RAM. This function returns such amount of rows.
 */
static int get_errata_rows(const struct socfpga_sdram_config *cfg)
{
	/* Define constant for 4G memory - used for SDRAM errata workaround */
#define MEMSIZE_4G	(4ULL * 1024ULL * 1024ULL * 1024ULL)
	const unsigned long long memsize = MEMSIZE_4G;
	const unsigned int cs =
		((cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_CSBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_CSBITS_LSB) + 1;
	const unsigned int rows =
		(cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_ROWBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_ROWBITS_LSB;
	const unsigned int banks =
		(cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_BANKBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_BANKBITS_LSB;
	const unsigned int cols =
		(cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_COLBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_COLBITS_LSB;
	const unsigned int width = 8;

	unsigned long long newrows;
	int bits, inewrowslog2;

	// debug("workaround rows - memsize %lld\n", memsize);
	// debug("workaround rows - cs        %d\n", cs);
	// debug("workaround rows - width     %d\n", width);
	// debug("workaround rows - rows      %d\n", rows);
	// debug("workaround rows - banks     %d\n", banks);
	// debug("workaround rows - cols      %d\n", cols);

	newrows = lldiv(memsize, cs * (width / 8));
	// debug("rows workaround - term1 %lld\n", newrows);

	newrows = lldiv(newrows, (1 << banks) * (1 << cols));
	// debug("rows workaround - term2 %lld\n", newrows);

	/*
	 * Compute the hamming weight - same as number of bits set.
	 * Need to see if result is ordinal power of 2 before
	 * attempting log2 of result.
	 */
	bits = generic_hweight32(newrows);

	// debug("rows workaround - bits %d\n", bits);

	if (bits != 1) 
	{
		//printf("SDRAM workaround failed, bits set %d\n", bits);
		return rows;
	}

	if (newrows > UINT_MAX) 
	{
		//printf("SDRAM workaround rangecheck failed, %lld\n", newrows);
		return rows;
	}

	inewrowslog2 = __ilog2(newrows);

	// debug("rows workaround - ilog2 %d, %lld\n", inewrowslog2, newrows);

	if (inewrowslog2 == -1) 
	{
		//printf("SDRAM workaround failed, newrows %lld\n", newrows);
		return rows;
	}

	return inewrowslog2;
}

/* SDRAM protection rules vary from 0-19, a total of 20 rules. */
static void sdram_set_rule(struct sdram_prot_rule *prule)
{
	u32 lo_addr_bits;
	u32 hi_addr_bits;
	int ruleno = prule->rule;

	/* Select the rule */
	writel(ruleno, &sdr_ctrl->prot_rule_rdwr);

	/* Obtain the address bits */
	lo_addr_bits = prule->sdram_start >> 20ULL;
	hi_addr_bits = (prule->sdram_end - 1) >> 20ULL;

	// debug("sdram set rule start %x, %d\n", lo_addr_bits, prule->sdram_start);
	// debug("sdram set rule end   %x, %d\n", hi_addr_bits, prule->sdram_end);

	/* Set rule addresses */
	writel(lo_addr_bits | (hi_addr_bits << 12), &sdr_ctrl->prot_rule_addr);

	/* Set rule protection ids */
	writel(prule->lo_prot_id | (prule->hi_prot_id << 12),
	       &sdr_ctrl->prot_rule_id);

	/* Set the rule data */
	writel(prule->security | (prule->valid << 2) |
	       (prule->portmask << 3) | (prule->result << 13),
	       &sdr_ctrl->prot_rule_data);

	/* write the rule */
	writel(ruleno | (1 << 5), &sdr_ctrl->prot_rule_rdwr);

	/* Set rule number to 0 by default */
	writel(0, &sdr_ctrl->prot_rule_rdwr);
}

static void sdram_get_rule(struct sdram_prot_rule *prule)
{
	u32 addr;
	u32 id;
	u32 data;
	int ruleno = prule->rule;

	/* Read the rule */
	writel(ruleno, &sdr_ctrl->prot_rule_rdwr);
	writel(ruleno | (1 << 6), &sdr_ctrl->prot_rule_rdwr);

	/* Get the addresses */
	addr = readl(&sdr_ctrl->prot_rule_addr);
	prule->sdram_start = (addr & 0xFFF) << 20;
	prule->sdram_end = ((addr >> 12) & 0xFFF) << 20;

	/* Get the configured protection IDs */
	id = readl(&sdr_ctrl->prot_rule_id);
	prule->lo_prot_id = id & 0xFFF;
	prule->hi_prot_id = (id >> 12) & 0xFFF;

	/* Get protection data */
	data = readl(&sdr_ctrl->prot_rule_data);

	prule->security = data & 0x3;
	prule->valid = (data >> 2) & 0x1;
	prule->portmask = (data >> 3) & 0x3FF;
	prule->result = (data >> 13) & 0x1;
}

static void
sdram_set_protection_config(const u32 sdram_start, const u32 sdram_end)
{
	struct sdram_prot_rule rule;
	int rules;

	/* Start with accepting all SDRAM transaction */
	writel(0x0, &sdr_ctrl->protport_default);

	/* Clear all protection rules for warm boot case */
	memset(&rule, 0, sizeof(rule));

	for (rules = 0; rules < 20; rules++) {
		rule.rule = rules;
		sdram_set_rule(&rule);
	}

	/* new rule: accept SDRAM */
	rule.sdram_start = sdram_start;
	rule.sdram_end = sdram_end;
	rule.lo_prot_id = 0x0;
	rule.hi_prot_id = 0xFFF;
	rule.portmask = 0x3FF;
	rule.security = 0x3;
	rule.result = 0;
	rule.valid = 1;
	rule.rule = 0;

	/* set new rule */
	sdram_set_rule(&rule);

	/* default rule: reject everything */
	writel(0x3ff, &sdr_ctrl->protport_default);
}

static void sdram_dump_protection_config(void)
{
	struct sdram_prot_rule rule;
	int rules;

	// debug("SDRAM Prot rule, default %x\n", readl(&sdr_ctrl->protport_default));

	for (rules = 0; rules < 20; rules++) {
		rule.rule = rules;
		sdram_get_rule(&rule);
		// debug("Rule %d, rules ...\n", rules);
		// debug("    sdram start %x\n", rule.sdram_start);
		// debug("    sdram end   %x\n", rule.sdram_end);
		// debug("    low prot id %d, hi prot id %d\n", rule.lo_prot_id, rule.hi_prot_id);
		// debug("    portmask %x\n", rule.portmask);
		// debug("    security %d\n", rule.security);
		// debug("    result %d\n", rule.result);
		// debug("    valid %d\n", rule.valid);
	}
}

/**
 * sdram_write_verify() - write to register and verify the write.
 * @addr:	Register address
 * @val:	Value to be written and verified
 *
 * This function writes to a register, reads back the value and compares
 * the result with the written value to check if the data match.
 */
static unsigned sdram_write_verify(const u32 *addr, const u32 val)
{
	u32 rval;

	//debug("   Write - Address 0x%p Data 0x%08x\n", addr, val);
	writel(val, addr);

	//debug("   Read and verify...");
	rval = readl(addr);
	if (rval != val) {
		//debug("FAIL - Address 0x%p Expected 0x%08x Data 0x%08x\n", addr, val, rval);
		return -EINVAL;
	}

	//debug("correct!\n");
	return 0;
}

/**
 * sdr_get_ctrlcfg() - Get the value of DRAM CTRLCFG register
 * @cfg:	SDRAM controller configuration data
 *
 * Return the value of DRAM CTRLCFG register.
 */
static u32 sdr_get_ctrlcfg(const struct socfpga_sdram_config *cfg)
{
	const u32 csbits =
		((cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_CSBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_CSBITS_LSB) + 1;
	u32 addrorder =
		(cfg->ctrl_cfg & SDR_CTRLGRP_CTRLCFG_ADDRORDER_MASK) >>
			SDR_CTRLGRP_CTRLCFG_ADDRORDER_LSB;

	u32 ctrl_cfg = cfg->ctrl_cfg;

	/*
	 * SDRAM Failure When Accessing Non-Existent Memory
	 * Set the addrorder field of the SDRAM control register
	 * based on the CSBITs setting.
	 */
	if (csbits == 1) {
		//if (addrorder != 0)
		//	debug("INFO: Changing address order to 0 (chip, row, bank, column)\n");
		addrorder = 0;
	} else if (csbits == 2) {
		//if (addrorder != 2)
		//	debug("INFO: Changing address order to 2 (row, chip, bank, column)\n");
		addrorder = 2;
	}

	ctrl_cfg &= ~SDR_CTRLGRP_CTRLCFG_ADDRORDER_MASK;
	ctrl_cfg |= addrorder << SDR_CTRLGRP_CTRLCFG_ADDRORDER_LSB;

	return ctrl_cfg;
}

/**
 * sdr_get_addr_rw() - Get the value of DRAM ADDRW register
 * @cfg:	SDRAM controller configuration data
 *
 * Return the value of DRAM ADDRW register.
 */
static u32 sdr_get_addr_rw(const struct socfpga_sdram_config *cfg)
{
	/*
	 * SDRAM Failure When Accessing Non-Existent Memory
	 * Set SDR_CTRLGRP_DRAMADDRW_CSBITS_LSB to
	 * log2(number of chip select bits). Since there's only
	 * 1 or 2 chip selects, log2(1) => 0, and log2(2) => 1,
	 * which is the same as "chip selects" - 1.
	 */
	const int rows = get_errata_rows(cfg);
	u32 dram_addrw = cfg->dram_addrw & ~SDR_CTRLGRP_DRAMADDRW_ROWBITS_MASK;

	return dram_addrw | (rows << SDR_CTRLGRP_DRAMADDRW_ROWBITS_LSB);
}

/**
 * sdr_load_regs() - Load SDRAM controller registers
 * @cfg:	SDRAM controller configuration data
 *
 * This function loads the register values into the SDRAM controller block.
 */
static void sdr_load_regs(const struct socfpga_sdram_config *cfg)
{
	const u32 ctrl_cfg = sdr_get_ctrlcfg(cfg);
	const u32 dram_addrw = sdr_get_addr_rw(cfg);

	//debug("\nConfiguring CTRLCFG\n");
	writel(ctrl_cfg, &sdr_ctrl->ctrl_cfg);

	//debug("Configuring DRAMTIMING1\n");
	writel(cfg->dram_timing1, &sdr_ctrl->dram_timing1);

	//debug("Configuring DRAMTIMING2\n");
	writel(cfg->dram_timing2, &sdr_ctrl->dram_timing2);

	//debug("Configuring DRAMTIMING3\n");
	writel(cfg->dram_timing3, &sdr_ctrl->dram_timing3);

	//debug("Configuring DRAMTIMING4\n");
	writel(cfg->dram_timing4, &sdr_ctrl->dram_timing4);

	//debug("Configuring LOWPWRTIMING\n");
	writel(cfg->lowpwr_timing, &sdr_ctrl->lowpwr_timing);

	//debug("Configuring DRAMADDRW\n");
	writel(dram_addrw, &sdr_ctrl->dram_addrw);

	//debug("Configuring DRAMIFWIDTH\n");
	writel(cfg->dram_if_width, &sdr_ctrl->dram_if_width);

	//debug("Configuring DRAMDEVWIDTH\n");
	writel(cfg->dram_dev_width, &sdr_ctrl->dram_dev_width);

	//debug("Configuring LOWPWREQ\n");
	writel(cfg->lowpwr_eq, &sdr_ctrl->lowpwr_eq);

	//debug("Configuring DRAMINTR\n");
	writel(cfg->dram_intr, &sdr_ctrl->dram_intr);

	//debug("Configuring STATICCFG\n");
	writel(cfg->static_cfg, &sdr_ctrl->static_cfg);

	//debug("Configuring CTRLWIDTH\n");
	writel(cfg->ctrl_width, &sdr_ctrl->ctrl_width);

	//debug("Configuring PORTCFG\n");
	writel(cfg->port_cfg, &sdr_ctrl->port_cfg);

	//debug("Configuring FIFOCFG\n");
	writel(cfg->fifo_cfg, &sdr_ctrl->fifo_cfg);

	//debug("Configuring MPPRIORITY\n");
	writel(cfg->mp_priority, &sdr_ctrl->mp_priority);

	//debug("Configuring MPWEIGHT_MPWEIGHT_0\n");
	writel(cfg->mp_weight0, &sdr_ctrl->mp_weight0);
	writel(cfg->mp_weight1, &sdr_ctrl->mp_weight1);
	writel(cfg->mp_weight2, &sdr_ctrl->mp_weight2);
	writel(cfg->mp_weight3, &sdr_ctrl->mp_weight3);

	//debug("Configuring MPPACING_MPPACING_0\n");
	writel(cfg->mp_pacing0, &sdr_ctrl->mp_pacing0);
	writel(cfg->mp_pacing1, &sdr_ctrl->mp_pacing1);
	writel(cfg->mp_pacing2, &sdr_ctrl->mp_pacing2);
	writel(cfg->mp_pacing3, &sdr_ctrl->mp_pacing3);

	//debug("Configuring MPTHRESHOLDRST_MPTHRESHOLDRST_0\n");
	writel(cfg->mp_threshold0, &sdr_ctrl->mp_threshold0);
	writel(cfg->mp_threshold1, &sdr_ctrl->mp_threshold1);
	writel(cfg->mp_threshold2, &sdr_ctrl->mp_threshold2);

	//debug("Configuring PHYCTRL_PHYCTRL_0\n");
	writel(cfg->phy_ctrl0, &sdr_ctrl->phy_ctrl0);

	//debug("Configuring CPORTWIDTH\n");
	writel(cfg->cport_width, &sdr_ctrl->cport_width);

	//debug("Configuring CPORTWMAP\n");
	writel(cfg->cport_wmap, &sdr_ctrl->cport_wmap);

	//debug("Configuring CPORTRMAP\n");
	writel(cfg->cport_rmap, &sdr_ctrl->cport_rmap);

	//debug("Configuring RFIFOCMAP\n");
	writel(cfg->rfifo_cmap, &sdr_ctrl->rfifo_cmap);

	//debug("Configuring WFIFOCMAP\n");
	writel(cfg->wfifo_cmap, &sdr_ctrl->wfifo_cmap);

	//debug("Configuring CPORTRDWR\n");
	writel(cfg->cport_rdwr, &sdr_ctrl->cport_rdwr);

	//debug("Configuring DRAMODT\n");
	writel(cfg->dram_odt, &sdr_ctrl->dram_odt);

	//debug("Configuring EXTRATIME1\n");
	writel(cfg->extratime1, &sdr_ctrl->extratime1);
}

/**
 * sdram_mmr_init_full() - Function to initialize SDRAM MMR
 * @sdr_phy_reg:	Value of the PHY control register 0
 *
 * Initialize the SDRAM MMR.
 */
int sdram_mmr_init_full(unsigned int sdr_phy_reg)
{
	const struct socfpga_sdram_config *cfg = socfpga_get_sdram_config();
	const unsigned int rows =
		(cfg->dram_addrw & SDR_CTRLGRP_DRAMADDRW_ROWBITS_MASK) >>
			SDR_CTRLGRP_DRAMADDRW_ROWBITS_LSB;
	int ret;

	writel(rows, &sysmgr_regs->iswgrp_handoff[4]);

	sdr_load_regs(cfg);

	/* saving this value to SYSMGR.ISWGRP.HANDOFF.FPGA2SDR */
	writel(cfg->fpgaport_rst, &sysmgr_regs->iswgrp_handoff[3]);

	// /* only enable if the FPGA is programmed */
	// if (fpgamgr_test_fpga_ready()) {
	// 	ret = sdram_write_verify(&sdr_ctrl->fpgaport_rst,
	// 				 cfg->fpgaport_rst);
	// 	if (ret)
	// 		return ret;
	// }

	/* Restore the SDR PHY Register if valid */
	if (sdr_phy_reg != 0xffffffff)
		writel(sdr_phy_reg, &sdr_ctrl->phy_ctrl0);

	/* Final step - apply configuration changes */
	//debug("Configuring STATICCFG\n");
	clrsetbits_le32(&sdr_ctrl->static_cfg,
			SDR_CTRLGRP_STATICCFG_APPLYCFG_MASK,
			1 << SDR_CTRLGRP_STATICCFG_APPLYCFG_LSB);

	sdram_set_protection_config(0, sdram_calculate_size() - 1);

	sdram_dump_protection_config();

	return 0;
}
