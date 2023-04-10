#include "common.h"


#define SOCFPGA_SYSMGR_ADDRESS		0xffd08000
#define SYSMGR_FRZCTRL_ADDRESS 		0x40
#define FRZCTRL_ADDRESS				SOCFPGA_SYSMGR_ADDRESS + SYSMGR_FRZCTRL_ADDRESS
#define SOCFPGA_MPUL2_ADDRESS		0xfffef000
#define CONFIG_SYS_PL310_BASE		SOCFPGA_MPUL2_ADDRESS
#define SOCFPGA_MPUSCU_ADDRESS		0xfffec000
#define SOCFPGA_L3REGS_ADDRESS		0xff800000
#define SOCFPGA_OSC1TIMER0_ADDRESS	0xffd00000
#define CONFIG_SYS_TIMERBASE		SOCFPGA_OSC1TIMER0_ADDRESS
#define SOCFPGA_RSTMGR_ADDRESS		0xffd05000
#define SOCFPGA_SCANMGR_ADDRESS		0xfff02000
#define FRZCTRL_ADDRESS				SOCFPGA_SYSMGR_ADDRESS + SYSMGR_FRZCTRL_ADDRESS


#define SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK 				0x00000020


#define 	SCANMGR_STAT			SOCFPGA_SCANMGR_ADDRESS + 0x00
#define 	SCANMGR_EN				SOCFPGA_SCANMGR_ADDRESS + 0x04
#define 	SCANMGR_FIFOSINGLEBYTE	SOCFPGA_SCANMGR_ADDRESS + 0x10
#define 	SCANMGR_FIFODOUBLEBYTE	SOCFPGA_SCANMGR_ADDRESS + 0x14
#define 	SCANMGR_FIFOQUADBYTE	SOCFPGA_SCANMGR_ADDRESS + 0x1C


/*
 * Maximum polling loop to wait for IO scan chain engine becomes idle
 * to prevent infinite loop. It is important that this is NOT changed
 * to delay using timer functions, since at the time this function is
 * called, timer might not yet be inited.
 */
#define SCANMGR_MAX_DELAY			100
#define SCANMGR_STAT_ACTIVE			(1 << 31)
#define SCANMGR_STAT_WFIFOCNT_MASK	0x70000000

#define JTAG_BP_INSN		(1 << 0)
#define JTAG_BP_TMS			(1 << 1)
#define JTAG_BP_PAYLOAD		(1 << 2)
#define JTAG_BP_2BYTE		(1 << 3)
#define JTAG_BP_4BYTE		(1 << 4)


#define CONFIG_HPS_IOCSR_SCANCHAIN0_LENGTH	764
#define CONFIG_HPS_IOCSR_SCANCHAIN1_LENGTH	1719
#define CONFIG_HPS_IOCSR_SCANCHAIN2_LENGTH	955
#define CONFIG_HPS_IOCSR_SCANCHAIN3_LENGTH	16766

const unsigned long FDy_iocsr_scan_chain0_table[] = {
	0x00000000,	0x00000000,	0x0FF00000,	0xC0000000,	0x0000003F,	0x00008000,	0x00060180,	0x18060000,
	0x18000000,	0x00018060,	0x00000000,	0x00004000,	0x000300C0,	0x0C030000,	0x0C000000,	0x00000030,
	0x0000C030,	0x00002000,	0x00018060,	0x06018000,	0x06000000,	0x00000018,	0x00006018,	0x00001000,
};

const unsigned long FDy_iocsr_scan_chain1_table[] = {
	0x00100000,	0x300C0000,	0x300000C0,	0x000000C0,	0x000300C0,	0x00008000,	0x00080000,	0x20000000,
	0x00000000,	0x00000080,	0x00020000,	0x00004000,	0x000300C0,	0x10000000,	0x0C000000,	0x00000030,
	0x0000C030,	0x00002000,	0x06018060,	0x06018000,	0x01FE0000,	0xF8000000,	0x00000007,	0x00001000,
	0x0000C030,	0x0300C000,	0x03000000,	0x0000300C,	0x0000300C,	0x00000800,	0x00000000,	0x00000000,
	0x01800000,	0x00000006,	0x00002000,	0x00000400,	0x00000000,	0x00C03000,	0x00000003,	0x00000000,
	0x00000000,	0x00000200,	0x00601806,	0x00000000,	0x80600000,	0x80000601,	0x00000601,	0x00000100,
	0x00300C03,	0xC0300C00,	0xC0300000,	0xC0000300,	0x000C0300,	0x00000080,
};

const unsigned long FDy_iocsr_scan_chain2_table[] = {
	0x300C0300,	0x00000000,	0x0FF00000,	0x00000000,	0x0C0300C0,	0x00008000,	0x18060180,	0x18060000,
	0x18000000,	0x00018060,	0x00018060,	0x00004000,	0x000300C0,	0x0C030000,	0x00000030,	0x00000000,
	0x0300C030,	0x00002000,	0x00018060,	0x06018000,	0x06000000,	0x00000018,	0x00006018,	0x00001000,
	0x0000C030,	0x00000000,	0x03000000,	0x0000000C,	0x00C0300C,	0x00000800,
};

const unsigned long FDy_iocsr_scan_chain3_table[] = {
	0x0C420D80,	0x082000FF,	0x0A804001,	0x07900000,	0x08020000,	0x00100000,	0x0A800000,	0x07900000,
	0x08020000,	0x00100000,	0xC8800000,	0x00003001,	0x00C00722,	0x00000000,	0x00000021,	0x82000004,
	0x05400000,	0x03C80000,	0x04010000,	0x00080000,	0x05400000,	0x03C80000,	0x05400000,	0x03C80000,
	0xE4400000,	0x00001800,	0x00600391,	0x800E4400,	0x00000001,	0x40000002,	0x02A00000,	0x01E40000,
	0x02A00000,	0x01E40000,	0x02A00000,	0x01E40000,	0x02A00000,	0x01E40000,	0x72200000,	0x80000C00,
	0x003001C8,	0xC0072200,	0x1C880000,	0x20000300,	0x00040000, 0x50670000,	0x00000010,	0x24590000,
	0x00001000,	0xA0000034,	0x0D000001,	0xE0680B2C,	0x20834038,	0x11441A00,	0x80B2C0D0,	0x34038E06,
	0x01A00208,	0x2C0D0000,	0x38E0680B,	0x00208340,	0xD000001A,	0x0680B2C0,	0x10040000,	0x00200000,
	0x10040000,	0x00200000,	0x15000000,	0x0F200000,	0x15000000,	0x0F200000,	0x01FE0000,	0x00000000,
	0x01800E44,	0x00391000,	0x007F8006,	0x00000000,	0x0A800001,	0x07900000,	0x0A800000,	0x07900000,
	0x0A800000,	0x07900000,	0x08020000,	0x00100000,	0xC8800000,	0x00003001,	0x00C00722,	0x00000FF0,
	0x72200000,	0x80000C00,	0x05400000,	0x02480000,	0x04000000,	0x00080000,	0x05400000,	0x03C80000,
	0x05400000,	0x03C80000,	0x6A1C0000,	0x00001800,	0x00600391,	0x800E4400,	0x1A870001,	0x40000600,
	0x02A00040,	0x01E40000,	0x02A00000,	0x01E40000,	0x02A00000,	0x01E40000,	0x02A00000,	0x01E40000,
	0x72200000,	0x80000C00,	0x003001C8,	0xC0072200,	0x1C880000,	0x20000300,	0x00040000,	0x50670000,
	0x00000010,	0x24590000,	0x00001000,	0xA0000034,	0x0D000001,	0xE0680B2C,	0x20834038,	0x11441A00,
	0x80B2C0D0,	0x34038E06,	0x01A00040,	0x2C0D0002,	0x38E0680B,	0x00208340,	0xD001041A,	0x0680B2C0,
	0x10040000,	0x00200000,	0x10040000,	0x00200000,	0x15000000,	0x0F200000,	0x15000000,	0x0F200000,
	0x01FE0000,	0x00000000,	0x01800E44,	0x00391000,	0x007F8006,	0x00000000,	0x99300001,	0x34343400,
	0xAA0D4000,	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0x00040100,	0x00000800,
	0x00000000,	0x00001208,	0x00482000,	0x01000000,	0x00000000,	0x00410482,	0x0006A000,	0x0001B400,
	0x00020000,	0x00000400,	0x0002A000,	0x0001E400,	0x5506A000,	0x00E1D400,	0x00000000,	0xC880090C,
	0x00003001,	0x90400000,	0x00000000,	0x2020C243,	0x2A835000,	0x0070EA00,	0x2A835000,	0x0070EA00,
	0x2A835000,	0x0070EA00,	0x00010040,	0x00000200,	0x00000000,	0x00000482,	0x00120800,	0x00002000,
	0x80000000,	0x00104120,	0x00000200,	0xAC0D5F80,	0xFFFFFFFF,	0x14F3690D,	0x1A041414,	0x00D00000,
	0x18864000,	0x49247A06,	0xABCF23D7,	0xF7DE791E,	0x0356E388,	0x821A0000,	0x0000D000,	0x05960680,
	0xD749247A,	0x1EABCF23,	0x88F7DE79,	0x000356E3,	0x00080200,	0x00001000,	0x00080200,	0x00001000,
	0x000A8000,	0x00075000,	0x541A8000,	0x03875001,	0x10000000,	0x00000000,	0x0080C000,	0x41000000,
	0x00003FC2,	0x00820000,	0xAA0D4000,	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0xAA0D4000,	0x01C3A800,
	0x00040100,	0x00000800,	0x00000000,	0x00001208,	0x00482000,	0x00008000,	0x00000000,	0x00410482,
	0x0006A000,	0x0001B400,	0x00020000,	0x00000400,	0x00020080,	0x00000400,	0x5506A000,	0x00E1D400,	
	0x00000000,	0x0000090C,	0x00000010,	0x90400000,	0x00000000,	0x2020C243,	0x2A835000,	0x0070EA00,	
	0x2A835000,	0x0070EA00,	0x2A835000,	0x0070EA00,	0x00015000,	0x0000F200,	0x00000000,
	0x00000482,	0x00120800,	0x00600391,	0x80000000,	0x00104120,	0x00000200,	0xAC0D5F80,	0xFFFFFFFF,
	0x14F3690D,	0x1A041414,	0x00D00000,	0x18864000,	0x49247A06,	0xABCF23D7,	0xF7DE791E,	0x0356E388,
	0x821A01C7,	0x0000D000,	0x00000680,	0xD749247A,	0x1EABCF23,	0x88F7DE79,	0x000356E3,	0x00080200,
	0x00001000,	0x00080200,	0x00001000,	0x000A8000,	0x00075000,	0x541A8000,	0x03875001,	0x10000000,
	0x00000000,	0x0080C000,	0x41000000,	0x04000002,	0x00820000,	0xAA0D4000,	0x01C3A800,	0xAA0D4000,
	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0x00040100,	0x00000800,	0x00000000,	0x00001208,	0x00482000,
	0x00008000,	0x00000000,	0x00410482,	0x0006A000,	0x0001B400,	0x00020000,	0x00000400,	0x0002A000,
	0x0001E400,	0x5506A000,	0x00E1D400,	0x00000000,	0xC880090C,	0x00003001,	0x90400000,	0x00000000,
	0x2020C243,	0x2A835000,	0x0070EA00,	0x2A835000,	0x0070EA00,	0x2A835000,	0x0070EA00,	0x00010040,
	0x00000200,	0x00000000,	0x00000482,	0x00120800,	0x00002000,	0x80000000,	0x00104120,	0x00000200,
	0xAC0D5F80,	0xFFFFFFFF,	0x14F3690D,	0x1A041414,	0x00D00000,	0x18864000,	0x49247A06,	0xABCF23D7,
	0xF7DE791E,	0x0356E388,	0x821A0000,	0x0000D000,	0x00000680,	0xD749247A,	0x1EABCF23,	0x88F7DE79,
	0x000356E3,	0x00080200,	0x00001000,	0x00080200,	0x00001000,	0x000A8000,	0x00075000,	0x541A8000,
	0x03875001,	0x10000000,	0x00000000,	0x0080C000,	0x41000000,	0x04000002,	0x00820000,	0xAA0D4000,
	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0xAA0D4000,	0x01C3A800,	0x00040100,	0x00000800,	0x00000000,
	0x00001208,	0x00482000,	0x00008000,	0x00000000,	0x00410482,	0x0006A000,	0x0001B400,	0x00020000,	
	0x00000400,	0x00020080,	0x00000400,	0x5506A000,	0x00E1D400,	0x00000000,	0x0000090C,	0x00000010,	
	0x90400000,	0x00000000,	0x2020C243,	0x2A835000,	0x0070EA00,	0x2A835000,	0x0070EA00,	0x2A835000,	
	0x0070EA00,	0x00010040,	0x00000200,	0x00000000,
	0x00000482,	0x00120800,	0x00400000,	0x80000000,	0x00104120,	0x00000200,	0xAC0D5F80,	0xFFFFFFFF,
	0x14F1690D,	0x1A041414,	0x00D00000,	0x18864000,	0x49247A06,	0xABCF23D7,	0xF7DE791E,	0x0356E388,
	0x821A0000,	0x0000D000,	0x00000680,	0xD749247A,	0x1EABCF23,	0x88F7DE79,	0x000356E3,	0x00080200,
	0x00001000,	0x00080200,	0x00001000,	0x000A8000,	0x00075000,	0x541A8000,	0x03875001,	0x10000000,
	0x00000000,	0x0080C000,	0x41000000,	0x04000002,	0x00820000,	0x00489800,	0x801A1A1A,	0x00000200,
	0x80000004,	0x00000200,	0x80000004,	0x00000200,	0x80000004,	0x00000200,	0x00000004,	0x00040000,
	0x10000000,	0x00000000,	0x00000040,	0x00010000,	0x40002000,	0x00000100,	0x40000002,	0x00000100,
	0x40000002,	0x00000100,	0x40000002,	0x00000100,	0x00000002,	0x00020000,	0x08000000,	0x00000000,
	0x00000020,	0x00008000,	0x20001000,	0x00000080,	0x20000001,	0x00000080,	0x20000001,	0x00000080,
	0x20000001,	0x00000080,	0x00000001,	0x00010000,	0x04000000,	0x00FF0000,	0x00000000,	0x00004000,
	0x00000800,	0xC0000001,	0x00041419,	0x40000000,	0x04000816,	0x000D0000,	0x00006800,	0x00000340,
	0xD000001A,	0x06800000,	0x00340000,	0x0001A000,	0x00000D00,	0x40000068,	0x1A000003,	0x00D00000,
	0x00068000,	0x00003400,	0x000001A0,	0x00000401,	0x00000008,	0x00000401,	0x00000008,	0x00000401,
	0x00000008,	0x00000401,	0x80000008,	0x0000007F,	0x20000000,	0x00000000,	0xE0000080,	0x0000001F,
	0x00004000,
	};


int FDy_iocsr_get_config_table(const unsigned int chain_id,
							   const unsigned long **table,
			   				   unsigned int *table_len)
{
	switch (chain_id) {
	case 0:
		*table = FDy_iocsr_scan_chain0_table;
		*table_len = CONFIG_HPS_IOCSR_SCANCHAIN0_LENGTH;
		break;
	case 1:
		*table = FDy_iocsr_scan_chain1_table;
		*table_len = CONFIG_HPS_IOCSR_SCANCHAIN1_LENGTH;
		break;
	case 2:
		*table = FDy_iocsr_scan_chain2_table;
		*table_len = CONFIG_HPS_IOCSR_SCANCHAIN2_LENGTH;
		break;
	case 3:
		*table = FDy_iocsr_scan_chain3_table;
		*table_len = CONFIG_HPS_IOCSR_SCANCHAIN3_LENGTH;
		break;
	default:
		return -22 /*EINVAL*/;
	}

	return 0;
}

/**
 * scan_mgr_jtag_io() - Access the JTAG chain
 * @flags:	Control flags, used to configure the action on the JTAG
 * @iarg:	Instruction argument
 * @parg:	Payload argument or data
 *
 * Perform I/O on the JTAG chain
 */
static void FDy_scan_mgr_jtag_io(const u32 flags, const u8 iarg, const u32 parg)
{
	u32 data = parg;

	if (flags & JTAG_BP_INSN) {	/* JTAG instruction */
		/*
		 * The SCC JTAG register is LSB first, so make
		 * space for the instruction at the LSB.
		 */
		data <<= 8;
		if (flags & JTAG_BP_TMS) {
			data |= (0 << 7);	/* TMS instruction. */
			data |= iarg & 0x3f;	/* TMS arg is 6 bits. */
			if (flags & JTAG_BP_PAYLOAD)
				data |= (1 << 6);
		} else {
			data |= (1 << 7);	/* TDI/TDO instruction. */
			data |= iarg & 0xf;	/* TDI/TDO arg is 4 bits. */
			if (flags & JTAG_BP_PAYLOAD)
				data |= (1 << 4);
		}
	}

	if (flags & JTAG_BP_4BYTE)
	{
		// writel(data, &scan_manager_base->fifo_quad_byte);
		writel(data, SCANMGR_FIFOQUADBYTE);
	}
	else if (flags & JTAG_BP_2BYTE)
	{
		// writel(data & 0xffff, &scan_manager_base->fifo_double_byte);
		writel(data & 0xffff, SCANMGR_FIFODOUBLEBYTE);
	}
	else
	{
		// writel(data & 0xff, &scan_manager_base->fifo_single_byte);
		writel(data & 0xff, SCANMGR_FIFOSINGLEBYTE);
	}
}


/*
static const struct socfpga_scan_manager *scan_manager_base =				(void *)(SOCFPGA_SCANMGR_ADDRESS);
static const struct socfpga_freeze_controller *freeze_controller_base =		(void *)(SOCFPGA_SYSMGR_ADDRESS + SYSMGR_FRZCTRL_ADDRESS);
static struct 		socfpga_system_manager *sys_mgr_base =					(struct socfpga_system_manager *)SOCFPGA_SYSMGR_ADDRESS;
*/
/**
 * scan_chain_engine_is_idle() - Check if the JTAG scan chain is idle
 * @max_iter:	Maximum number of iterations to wait for idle
 *
 * Function to check IO scan chain engine status and wait if the engine is
 * is active. Poll the IO scan chain engine till maximum iteration reached.
 */
static u32 FDy_scan_chain_engine_is_idle(u32 max_iter)
{
	const u32 mask = SCANMGR_STAT_ACTIVE | SCANMGR_STAT_WFIFOCNT_MASK;
	u32 status;

	/* Poll the engine until the scan engine is inactive. */
	do {
		//status = readl(&scan_manager_base->stat);
		status = readl(SCANMGR_STAT);
		if (!(status & mask))
			return 0;
	} while (max_iter--);

	return -110 /*ETIMEDOUT*/;
}

/**
 * scan_mgr_jtag_insn_data() - Send JTAG instruction and data
 * @iarg:	Instruction argument
 * @data:	Associated data
 * @dlen:	Length of data in bits
 *
 * This function is used when programming the IO chains to submit the
 * instruction followed by variable length payload.
 */
static int	FDy_scan_mgr_jtag_insn_data(const u8 iarg, const unsigned long *data,
			const unsigned int dlen)
{
	int i, j;

	FDy_scan_mgr_jtag_io(JTAG_BP_INSN | JTAG_BP_2BYTE, iarg, dlen - 1);

	/* 32 bits or more remain */
	for (i = 0; i < dlen / 32; i++)
		FDy_scan_mgr_jtag_io(JTAG_BP_4BYTE, 0x0, data[i]);

	if ((dlen % 32) > 24) {	/* 31...24 bits remain */
		FDy_scan_mgr_jtag_io(JTAG_BP_4BYTE, 0x0, data[i]);
	} else if (dlen % 32) {	/* 24...1 bit remain */
		for (j = 0; j < dlen % 32; j += 8)
			FDy_scan_mgr_jtag_io(0, 0x0, data[i] >> j);
	}

	return FDy_scan_chain_engine_is_idle(SCANMGR_MAX_DELAY);
}

/**
 * scan_mgr_io_scan_chain_prg() - Program HPS IO Scan Chain
 * @io_scan_chain_id:		IO scan chain ID
 */
static int FDy_scan_mgr_io_scan_chain_prg(const unsigned int io_scan_chain_id)
{
	u32 io_scan_chain_len_in_bits;
	const unsigned long *iocsr_scan_chain;
	unsigned int rem, idx = 0;
	int ret;

	ret = FDy_iocsr_get_config_table(io_scan_chain_id, &iocsr_scan_chain, &io_scan_chain_len_in_bits);
	if (ret)
		return 1;

	/*
	 * De-assert reinit if the IO scan chain is intended for HIO. In
	 * this, its the chain 3.
	 */
	if (io_scan_chain_id == 3)
	{
		// clrbits_le32(&freeze_controller_base->hioctrl, SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK);
		clrbits_le32(FRZCTRL_ADDRESS + 0x10, SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK);
	}

	/*
	 * Check if the scan chain engine is inactive and the
	 * WFIFO is empty before enabling the IO scan chain
	 */
	ret = FDy_scan_chain_engine_is_idle(SCANMGR_MAX_DELAY);
	if (ret)
		return ret;

	/*
	 * Enable IO Scan chain based on scan chain id
	 * Note: only one chain can be enabled at a time
	 */
	//setbits_le32(&scan_manager_base->en, 1 << io_scan_chain_id);
	setbits_le32(SCANMGR_EN, 1 << io_scan_chain_id);

	/* Program IO scan chain. */
	while (io_scan_chain_len_in_bits) 
	{
		if (io_scan_chain_len_in_bits > 128)	rem = 128;
		else									rem = io_scan_chain_len_in_bits;

		ret = FDy_scan_mgr_jtag_insn_data(0x0, &iocsr_scan_chain[idx], rem);
		if (ret)
			goto error;
		io_scan_chain_len_in_bits -= rem;
		idx += 4;
	}

	/* Disable IO Scan chain when configuration done*/
	//clrbits_le32(&scan_manager_base->en, 1 << io_scan_chain_id);
	clrbits_le32(SCANMGR_EN, 1 << io_scan_chain_id);

	return 0;

error:
	/* Disable IO Scan chain when error detected */
	//clrbits_le32(&scan_manager_base->en, 1 << io_scan_chain_id);
	clrbits_le32(SCANMGR_EN, 1 << io_scan_chain_id);

	return ret;
}


int FDy_scan_mgr_configure_iocsr(void)
{
	int status = 0;

	/* configure the IOCSR through scan chain */
	status |= FDy_scan_mgr_io_scan_chain_prg(0);
	status |= FDy_scan_mgr_io_scan_chain_prg(1);
	status |= FDy_scan_mgr_io_scan_chain_prg(2);
	status |= FDy_scan_mgr_io_scan_chain_prg(3);
	return status;
}


// /*
//  * Populate the value for SYSMGR.FPGAINTF.MODULE based on pinmux setting.
//  * The value is not wrote to SYSMGR.FPGAINTF.MODULE but
//  * CONFIG_SYSMGR_ISWGRP_HANDOFF.
//  */
// static void FDy_populate_sysmgr_fpgaintf_module(void)
// {
// 	uint32_t handoff_val = 0;

// 	/* ISWGRP_HANDOFF_FPGAINTF */
// 	writel(0, &sysmgr_regs->iswgrp_handoff[2]);

// 	/* Enable the signal for those HPS peripherals that use FPGA. */
// 	if (readl(&sysmgr_regs->nandusefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_NAND;
// 	if (readl(&sysmgr_regs->rgmii1usefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_EMAC1;
// 	if (readl(&sysmgr_regs->sdmmcusefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_SDMMC;
// 	if (readl(&sysmgr_regs->rgmii0usefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_EMAC0;
// 	if (readl(&sysmgr_regs->spim0usefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_SPIM0;
// 	if (readl(&sysmgr_regs->spim1usefpga) == SYSMGR_FPGAINTF_USEFPGA)
// 		handoff_val |= SYSMGR_FPGAINTF_SPIM1;

// 	/* populate (not writing) the value for SYSMGR.FPGAINTF.MODULE
// 	based on pinmux setting */
// 	setbits_le32(&sysmgr_regs->iswgrp_handoff[2], handoff_val);

// 	handoff_val = readl(&sysmgr_regs->iswgrp_handoff[2]);
// 	if (fFDy_pgamgr_test_fpga_ready()) {
// 		/* Enable the required signals only */
// 		writel(handoff_val, &sysmgr_regs->fpgaintfgrp_module);
// 	}
// }

const u8 FDy_sys_mgr_init_table[] = {
	0, /* EMACIO0 */
	2, /* EMACIO1 */
	2, /* EMACIO2 */
	2, /* EMACIO3 */
	2, /* EMACIO4 */
	2, /* EMACIO5 */
	2, /* EMACIO6 */
	2, /* EMACIO7 */
	2, /* EMACIO8 */
	0, /* EMACIO9 */
	2, /* EMACIO10 */
	2, /* EMACIO11 */
	2, /* EMACIO12 */
	2, /* EMACIO13 */
	0, /* EMACIO14 */
	0, /* EMACIO15 */
	0, /* EMACIO16 */
	0, /* EMACIO17 */
	0, /* EMACIO18 */
	0, /* EMACIO19 */
	3, /* FLASHIO0 */
	0, /* FLASHIO1 */
	3, /* FLASHIO2 */
	3, /* FLASHIO3 */
	0, /* FLASHIO4 */
	0, /* FLASHIO5 */
	0, /* FLASHIO6 */
	0, /* FLASHIO7 */
	0, /* FLASHIO8 */
	3, /* FLASHIO9 */
	3, /* FLASHIO10 */
	3, /* FLASHIO11 */
	0, /* GENERALIO0 */
	1, /* GENERALIO1 */
	1, /* GENERALIO2 */
	1, /* GENERALIO3 */
	1, /* GENERALIO4 */
	0, /* GENERALIO5 */
	0, /* GENERALIO6 */
	0, /* GENERALIO7 */
	0, /* GENERALIO8 */
	3, /* GENERALIO9 */
	3, /* GENERALIO10 */
	3, /* GENERALIO11 */
	3, /* GENERALIO12 */
	0, /* GENERALIO13 */
	0, /* GENERALIO14 */
	1, /* GENERALIO15 */
	1, /* GENERALIO16 */
	1, /* GENERALIO17 */
	1, /* GENERALIO18 */
	0, /* GENERALIO19 */
	0, /* GENERALIO20 */
	0, /* GENERALIO21 */
	0, /* GENERALIO22 */
	0, /* GENERALIO23 */
	0, /* GENERALIO24 */
	0, /* GENERALIO25 */
	0, /* GENERALIO26 */
	0, /* GENERALIO27 */
	0, /* GENERALIO28 */
	0, /* GENERALIO29 */
	0, /* GENERALIO30 */
	0, /* GENERALIO31 */
	2, /* MIXED1IO0 */
	2, /* MIXED1IO1 */
	2, /* MIXED1IO2 */
	2, /* MIXED1IO3 */
	2, /* MIXED1IO4 */
	2, /* MIXED1IO5 */
	2, /* MIXED1IO6 */
	2, /* MIXED1IO7 */
	2, /* MIXED1IO8 */
	2, /* MIXED1IO9 */
	2, /* MIXED1IO10 */
	2, /* MIXED1IO11 */
	2, /* MIXED1IO12 */
	2, /* MIXED1IO13 */
	0, /* MIXED1IO14 */
	3, /* MIXED1IO15 */
	3, /* MIXED1IO16 */
	3, /* MIXED1IO17 */
	3, /* MIXED1IO18 */
	3, /* MIXED1IO19 */
	3, /* MIXED1IO20 */
	0, /* MIXED1IO21 */
	0, /* MIXED2IO0 */
	0, /* MIXED2IO1 */
	0, /* MIXED2IO2 */
	0, /* MIXED2IO3 */
	0, /* MIXED2IO4 */
	0, /* MIXED2IO5 */
	0, /* MIXED2IO6 */
	0, /* MIXED2IO7 */
	0, /* GPLINMUX48 */
	0, /* GPLINMUX49 */
	0, /* GPLINMUX50 */
	0, /* GPLINMUX51 */
	0, /* GPLINMUX52 */
	0, /* GPLINMUX53 */
	0, /* GPLINMUX54 */
	0, /* GPLINMUX55 */
	0, /* GPLINMUX56 */
	0, /* GPLINMUX57 */
	0, /* GPLINMUX58 */
	0, /* GPLINMUX59 */
	0, /* GPLINMUX60 */
	0, /* GPLINMUX61 */
	0, /* GPLINMUX62 */
	0, /* GPLINMUX63 */
	0, /* GPLINMUX64 */
	0, /* GPLINMUX65 */
	0, /* GPLINMUX66 */
	0, /* GPLINMUX67 */
	0, /* GPLINMUX68 */
	0, /* GPLINMUX69 */
	0, /* GPLINMUX70 */
	1, /* GPLMUX0 */
	1, /* GPLMUX1 */
	1, /* GPLMUX2 */
	1, /* GPLMUX3 */
	1, /* GPLMUX4 */
	1, /* GPLMUX5 */
	1, /* GPLMUX6 */
	1, /* GPLMUX7 */
	1, /* GPLMUX8 */
	1, /* GPLMUX9 */
	1, /* GPLMUX10 */
	1, /* GPLMUX11 */
	1, /* GPLMUX12 */
	1, /* GPLMUX13 */
	1, /* GPLMUX14 */
	1, /* GPLMUX15 */
	1, /* GPLMUX16 */
	1, /* GPLMUX17 */
	1, /* GPLMUX18 */
	1, /* GPLMUX19 */
	1, /* GPLMUX20 */
	1, /* GPLMUX21 */
	1, /* GPLMUX22 */
	1, /* GPLMUX23 */
	1, /* GPLMUX24 */
	1, /* GPLMUX25 */
	1, /* GPLMUX26 */
	1, /* GPLMUX27 */
	1, /* GPLMUX28 */
	1, /* GPLMUX29 */
	1, /* GPLMUX30 */
	1, /* GPLMUX31 */
	1, /* GPLMUX32 */
	1, /* GPLMUX33 */
	1, /* GPLMUX34 */
	1, /* GPLMUX35 */
	1, /* GPLMUX36 */
	1, /* GPLMUX37 */
	1, /* GPLMUX38 */
	1, /* GPLMUX39 */
	1, /* GPLMUX40 */
	1, /* GPLMUX41 */
	1, /* GPLMUX42 */
	1, /* GPLMUX43 */
	1, /* GPLMUX44 */
	1, /* GPLMUX45 */
	1, /* GPLMUX46 */
	1, /* GPLMUX47 */
	1, /* GPLMUX48 */
	1, /* GPLMUX49 */
	1, /* GPLMUX50 */
	1, /* GPLMUX51 */
	1, /* GPLMUX52 */
	1, /* GPLMUX53 */
	1, /* GPLMUX54 */
	1, /* GPLMUX55 */
	1, /* GPLMUX56 */
	1, /* GPLMUX57 */
	1, /* GPLMUX58 */
	1, /* GPLMUX59 */
	1, /* GPLMUX60 */
	1, /* GPLMUX61 */
	1, /* GPLMUX62 */
	1, /* GPLMUX63 */
	1, /* GPLMUX64 */
	1, /* GPLMUX65 */
	1, /* GPLMUX66 */
	1, /* GPLMUX67 */
	1, /* GPLMUX68 */
	1, /* GPLMUX69 */
	1, /* GPLMUX70 */
	0, /* NANDUSEFPGA */
	0, /* UART0USEFPGA */
	0, /* RGMII1USEFPGA */
	0, /* SPIS0USEFPGA */
	0, /* CAN0USEFPGA */
	0, /* I2C0USEFPGA */
	0, /* SDMMCUSEFPGA */
	0, /* QSPIUSEFPGA */
	0, /* SPIS1USEFPGA */
	0, /* RGMII0USEFPGA */
	0, /* UART1USEFPGA */
	0, /* CAN1USEFPGA */
	0, /* USB1USEFPGA */
	0, /* I2C3USEFPGA */
	0, /* I2C2USEFPGA */
	0, /* I2C1USEFPGA */
	0, /* SPIM1USEFPGA */
	0, /* USB0USEFPGA */
	0 /* SPIM0USEFPGA */
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void FDy_sysmgr_get_pinmux_table(const u8 **table, unsigned int *table_len)
{
	*table = FDy_sys_mgr_init_table;
	*table_len = ARRAY_SIZE(FDy_sys_mgr_init_table);
}

/*
 * Configure all the pin muxes
 */
void FDy_sysmgr_pinmux_init(void)
{
	//uint32_t regs = (uint32_t)&sysmgr_regs->emacio[0];
	uint32_t regs = SOCFPGA_SYSMGR_ADDRESS +0x0400;
	
	const u8 *sys_mgr_init_table;
	unsigned int len;
	int i;

	FDy_sysmgr_get_pinmux_table(&sys_mgr_init_table, &len);

	for (i = 0; i < len; i++) 
	{
		writel(sys_mgr_init_table[i], regs);
		regs += sizeof(regs);
	}

	//FDy_populate_sysmgr_fpgaintf_module();		// don't need it yet
}

