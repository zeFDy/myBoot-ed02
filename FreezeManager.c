#include "common.h"


#define FREEZE_CHANNEL_NUM		(4)

typedef enum {
	FREEZE_CTRL_FROZEN = 0,
	FREEZE_CTRL_THAWED = 1
} FREEZE_CTRL_CHAN_STATE;

#define SOCFPGA_SYSMGR_ADDRESS		                    0xffd08000
#define SYSMGR_FRZCTRL_ADDRESS 							0x40
#define FRZCTRL_ADDRESS									SOCFPGA_SYSMGR_ADDRESS + SYSMGR_FRZCTRL_ADDRESS

#define SYSMGR_FRZCTRL_SRC_VIO1_ENUM_SW 				0x0
#define SYSMGR_FRZCTRL_SRC_VIO1_ENUM_HW 				0x1
#define SYSMGR_FRZCTRL_VIOCTRL_SLEW_MASK 				0x00000010
#define SYSMGR_FRZCTRL_VIOCTRL_WKPULLUP_MASK 			0x00000008
#define SYSMGR_FRZCTRL_VIOCTRL_TRISTATE_MASK 			0x00000004
#define SYSMGR_FRZCTRL_VIOCTRL_BUSHOLD_MASK 			0x00000002
#define SYSMGR_FRZCTRL_VIOCTRL_CFG_MASK 				0x00000001
#define SYSMGR_FRZCTRL_HIOCTRL_SLEW_MASK 				0x00000010
#define SYSMGR_FRZCTRL_HIOCTRL_WKPULLUP_MASK 			0x00000008
#define SYSMGR_FRZCTRL_HIOCTRL_TRISTATE_MASK 			0x00000004
#define SYSMGR_FRZCTRL_HIOCTRL_BUSHOLD_MASK 			0x00000002
#define SYSMGR_FRZCTRL_HIOCTRL_CFG_MASK 				0x00000001
#define SYSMGR_FRZCTRL_HIOCTRL_REGRST_MASK 				0x00000080
#define SYSMGR_FRZCTRL_HIOCTRL_OCTRST_MASK 				0x00000040
#define SYSMGR_FRZCTRL_HIOCTRL_OCT_CFGEN_CALSTART_MASK 	0x00000100
#define SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK 				0x00000020
#define SYSMGR_FRZCTRL_HWCTRL_VIO1REQ_MASK 				0x00000001
#define SYSMGR_FRZCTRL_HWCTRL_VIO1STATE_ENUM_FROZEN 	0x2
#define SYSMGR_FRZCTRL_HWCTRL_VIO1STATE_ENUM_THAWED 	0x1


/*
 * Default state from cold reset is FREEZE_ALL; the global
 * flag is set to TRUE to indicate the IO banks are frozen
 */
static u32 frzctrl_channel_freeze[FREEZE_CHANNEL_NUM]
	= { 	FREEZE_CTRL_FROZEN, FREEZE_CTRL_FROZEN,
			FREEZE_CTRL_FROZEN, FREEZE_CTRL_FROZEN	};

/* Freeze HPS IOs */
void FDy_sys_mgr_frzctrl_freeze_req(void)
{
	u32 ioctrl_reg_offset;
	u32 reg_value;
	u32 reg_cfg_mask;
	u32 channel_id;

	/* select software FSM */
	//writel(SYSMGR_FRZCTRL_SRC_VIO1_ENUM_SW,	&freeze_controller_base->src);
	writel(SYSMGR_FRZCTRL_SRC_VIO1_ENUM_SW,	FRZCTRL_ADDRESS + 0x14);

	/* Freeze channel 0 to 2 */
	for (channel_id = 0; channel_id <= 2; channel_id++) 
	{
	
		//ioctrl_reg_offset = (u32)( &freeze_controller_base->vioctrl + channel_id);
		ioctrl_reg_offset = (u32)( FRZCTRL_ADDRESS + 0x00 + channel_id*sizeof(unsigned int));

		/*
		 * Assert active low enrnsl, plniotri
		 * and niotri signals
		 */

		reg_cfg_mask =		SYSMGR_FRZCTRL_VIOCTRL_SLEW_MASK
						| 	SYSMGR_FRZCTRL_VIOCTRL_WKPULLUP_MASK
						| 	SYSMGR_FRZCTRL_VIOCTRL_TRISTATE_MASK;

		clrbits_le32(ioctrl_reg_offset,	reg_cfg_mask);

		/*
		 * Note: Delay for 20ns at min
		 * Assert active low bhniotri signal and de-assert
		 * active high csrdone
		 */

		reg_cfg_mask = 		SYSMGR_FRZCTRL_VIOCTRL_BUSHOLD_MASK
						| 	SYSMGR_FRZCTRL_VIOCTRL_CFG_MASK;

		clrbits_le32(ioctrl_reg_offset,	reg_cfg_mask);

		/* Set global flag to indicate channel is frozen */
		frzctrl_channel_freeze[channel_id] = FREEZE_CTRL_FROZEN;
	}

	/* Freeze channel 3 */
	/*
	 * Assert active low enrnsl, plniotri and
	 * niotri signals
	 */
	reg_cfg_mask =		SYSMGR_FRZCTRL_HIOCTRL_SLEW_MASK
					| 	SYSMGR_FRZCTRL_HIOCTRL_WKPULLUP_MASK
					| 	SYSMGR_FRZCTRL_HIOCTRL_TRISTATE_MASK;

	//clrbits_le32(&freeze_controller_base->hioctrl, reg_cfg_mask);
	clrbits_le32(FRZCTRL_ADDRESS + 0x10, reg_cfg_mask);

	/*
	 * assert active low bhniotri & nfrzdrv signals,
	 * de-assert active high csrdone and assert
	 * active high frzreg and nfrzdrv signals
	 */

	//reg_value = readl(&freeze_controller_base->hioctrl);
	reg_value = readl(FRZCTRL_ADDRESS + 0x10);

	reg_cfg_mask = 	SYSMGR_FRZCTRL_HIOCTRL_BUSHOLD_MASK
				| 	SYSMGR_FRZCTRL_HIOCTRL_CFG_MASK;

	reg_value	= (reg_value & ~reg_cfg_mask)
				| SYSMGR_FRZCTRL_HIOCTRL_REGRST_MASK
				| SYSMGR_FRZCTRL_HIOCTRL_OCTRST_MASK;

	//writel(reg_value, &freeze_controller_base->hioctrl);
	writel(reg_value, FRZCTRL_ADDRESS + 0x10);

	/*
	 * assert active high reinit signal and de-assert
	 * active high pllbiasen signals
	 */
	//reg_value = readl(&freeze_controller_base->hioctrl);
	reg_value = readl(FRZCTRL_ADDRESS + 0x10);
	
	reg_value		= (	reg_value &
						~SYSMGR_FRZCTRL_HIOCTRL_OCT_CFGEN_CALSTART_MASK)
						| SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK;

	//writel(reg_value, &freeze_controller_base->hioctrl);
	writel(reg_value, FRZCTRL_ADDRESS + 0x10);

	/* Set global flag to indicate channel is frozen */
	frzctrl_channel_freeze[channel_id] = FREEZE_CTRL_FROZEN;
}

/* Unfreeze/Thaw HPS IOs */
void FDy_sys_mgr_frzctrl_thaw_req(void)
{
	u32 ioctrl_reg_offset;
	u32 reg_cfg_mask;
	u32 reg_value;
	u32 channel_id;
	unsigned long eosc1_freq;

	/* select software FSM */
	//writel(SYSMGR_FRZCTRL_SRC_VIO1_ENUM_SW,	&freeze_controller_base->src);
	writel(SYSMGR_FRZCTRL_SRC_VIO1_ENUM_SW,	FRZCTRL_ADDRESS + 0x14);

	/* Thaw channel 0 to 2 */
	for (channel_id = 0; channel_id <= 2; channel_id++) 
	{
		//ioctrl_reg_offset = (u32)(&freeze_controller_base->vioctrl + channel_id);
		ioctrl_reg_offset = (u32)( FRZCTRL_ADDRESS + 0x00 + channel_id*sizeof(unsigned int));

		/*
		 * Assert active low bhniotri signal and
		 * de-assert active high csrdone
		 */
		reg_cfg_mask
			= SYSMGR_FRZCTRL_VIOCTRL_BUSHOLD_MASK
			| SYSMGR_FRZCTRL_VIOCTRL_CFG_MASK;
			
		setbits_le32(ioctrl_reg_offset,	reg_cfg_mask);

		/*
		 * Note: Delay for 20ns at min
		 * de-assert active low plniotri and niotri signals
		 */
		reg_cfg_mask
			= SYSMGR_FRZCTRL_VIOCTRL_WKPULLUP_MASK
			| SYSMGR_FRZCTRL_VIOCTRL_TRISTATE_MASK;

		setbits_le32(ioctrl_reg_offset,	reg_cfg_mask);

		/*
		 * Note: Delay for 20ns at min
		 * de-assert active low enrnsl signal
		 */
		setbits_le32(ioctrl_reg_offset,	SYSMGR_FRZCTRL_VIOCTRL_SLEW_MASK);

		/* Set global flag to indicate channel is thawed */
		frzctrl_channel_freeze[channel_id] = FREEZE_CTRL_THAWED;
	}

	/* Thaw channel 3 */
	/* de-assert active high reinit signal */
	//clrbits_le32(&freeze_controller_base->hioctrl,	SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK);
	clrbits_le32(FRZCTRL_ADDRESS + 0x10,	SYSMGR_FRZCTRL_HIOCTRL_DLLRST_MASK);

	/*
	 * Note: Delay for 40ns at min
	 * assert active high pllbiasen signals
	 */
	//setbits_le32(&freeze_controller_base->hioctrl,		SYSMGR_FRZCTRL_HIOCTRL_OCT_CFGEN_CALSTART_MASK);
	setbits_le32(FRZCTRL_ADDRESS + 0x10,	SYSMGR_FRZCTRL_HIOCTRL_OCT_CFGEN_CALSTART_MASK);


	/* Delay 1000 intosc cycles. The intosc is based on eosc1. */
	//eosc1_freq = cm_get_osc_clk_hz(1) / 1000;	/* kHz */
	//udelay(DIV_ROUND_UP(1000000, eosc1_freq));
	//udelay(10);	// FDy
	for(int i=0;i<10000;i++)
	{
		volatile	int	j=i*2;
	}
	
	/*
	 * de-assert active low bhniotri signals,
	 * assert active high csrdone and nfrzdrv signal
	 */
	//reg_value = readl(&freeze_controller_base->hioctrl);
	reg_value = readl(FRZCTRL_ADDRESS + 0x10);


	reg_value = (reg_value
		| SYSMGR_FRZCTRL_HIOCTRL_BUSHOLD_MASK
		| SYSMGR_FRZCTRL_HIOCTRL_CFG_MASK)
		& ~SYSMGR_FRZCTRL_HIOCTRL_OCTRST_MASK;

	//writel(reg_value, &freeze_controller_base->hioctrl);
	writel(reg_value, FRZCTRL_ADDRESS + 0x10);

	/*
	 * Delay 33 intosc
	 * Use worst case which is fatest eosc1=50MHz, delay required
	 * is 1/50MHz * 33 = 660ns ~= 1us
	 */
	//udelay(1);
	for(int i=0;i<1000;i++)
	{
		volatile	int	j=i*2;
	}

	/* de-assert active low plniotri and niotri signals */
	reg_cfg_mask
		= SYSMGR_FRZCTRL_HIOCTRL_WKPULLUP_MASK
		| SYSMGR_FRZCTRL_HIOCTRL_TRISTATE_MASK;

	//setbits_le32(&freeze_controller_base->hioctrl, reg_cfg_mask);
	setbits_le32(FRZCTRL_ADDRESS + 0x10, reg_cfg_mask);


	/*
	 * Note: Delay for 40ns at min
	 * de-assert active high frzreg signal
	 */
	//clrbits_le32(&freeze_controller_base->hioctrl, SYSMGR_FRZCTRL_HIOCTRL_REGRST_MASK);
	clrbits_le32(FRZCTRL_ADDRESS + 0x10, SYSMGR_FRZCTRL_HIOCTRL_REGRST_MASK);


	/*
	 * Note: Delay for 40ns at min
	 * de-assert active low enrnsl signal
	 */
	//setbits_le32(&freeze_controller_base->hioctrl,		SYSMGR_FRZCTRL_HIOCTRL_SLEW_MASK);
	setbits_le32(FRZCTRL_ADDRESS + 0x10, SYSMGR_FRZCTRL_HIOCTRL_SLEW_MASK);

	/* Set global flag to indicate channel is thawed */
	frzctrl_channel_freeze[channel_id] = FREEZE_CTRL_THAWED;
}


