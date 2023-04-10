#include "common.h"

#define SDMMC_BASE_ADDRESS	0xFF704000

#define DWMCI_CTRL		    0x000
#define	DWMCI_PWREN		    0x004
#define DWMCI_CLKDIV		0x008
#define DWMCI_CLKSRC		0x00C
#define DWMCI_CLKENA		0x010
#define DWMCI_TMOUT		    0x014
#define DWMCI_CTYPE		    0x018
#define DWMCI_BLKSIZ		0x01C
#define DWMCI_BYTCNT		0x020
#define DWMCI_INTMASK		0x024
#define DWMCI_CMDARG		0x028
#define DWMCI_CMD		    0x02C
#define DWMCI_RESP0		    0x030
#define DWMCI_RESP1		    0x034
#define DWMCI_RESP2		    0x038
#define DWMCI_RESP3		    0x03C
#define DWMCI_MINTSTS		0x040
#define DWMCI_RINTSTS		0x044
#define DWMCI_STATUS		0x048
#define DWMCI_FIFOTH		0x04C
#define DWMCI_CDETECT		0x050
#define DWMCI_WRTPRT		0x054
#define DWMCI_GPIO		    0x058
#define DWMCI_TCMCNT		0x05C
#define DWMCI_TBBCNT		0x060
#define DWMCI_DEBNCE		0x064
#define DWMCI_USRID		    0x068
#define DWMCI_VERID		    0x06C
#define DWMCI_HCON		    0x070
#define DWMCI_UHS_REG		0x074
#define DWMCI_BMOD		    0x080
#define DWMCI_PLDMND		0x084
#define DWMCI_DBADDR		0x088
#define DWMCI_IDSTS		    0x08C
#define DWMCI_IDINTEN		0x090
#define DWMCI_DSCADDR		0x094
#define DWMCI_BUFADDR		0x098
#define DWMCI_DATA		    0x200

/* Interrupt Mask register */
#define DWMCI_INTMSK_ALL	0xffffffff
#define DWMCI_INTMSK_RE		(1 << 1)
#define DWMCI_INTMSK_CDONE	(1 << 2)
#define DWMCI_INTMSK_DTO	(1 << 3)
#define DWMCI_INTMSK_TXDR	(1 << 4)
#define DWMCI_INTMSK_RXDR	(1 << 5)
#define DWMCI_INTMSK_DCRC	(1 << 7)
#define DWMCI_INTMSK_RTO	(1 << 8)
#define DWMCI_INTMSK_DRTO	(1 << 9)
#define DWMCI_INTMSK_HTO	(1 << 10)
#define DWMCI_INTMSK_FRUN	(1 << 11)
#define DWMCI_INTMSK_HLE	(1 << 12)
#define DWMCI_INTMSK_SBE	(1 << 13)
#define DWMCI_INTMSK_ACD	(1 << 14)
#define DWMCI_INTMSK_EBE	(1 << 15)

/* Raw interrupt Regsiter */
#define DWMCI_DATA_ERR	(DWMCI_INTMSK_EBE | DWMCI_INTMSK_SBE | DWMCI_INTMSK_HLE |\
			DWMCI_INTMSK_FRUN | DWMCI_INTMSK_EBE | DWMCI_INTMSK_DCRC)
#define DWMCI_DATA_TOUT	(DWMCI_INTMSK_HTO | DWMCI_INTMSK_DRTO)
/* CTRL register */
#define DWMCI_CTRL_RESET	    (1 << 0)
#define DWMCI_CTRL_FIFO_RESET	(1 << 1)
#define DWMCI_CTRL_DMA_RESET	(1 << 2)
#define DWMCI_DMA_EN		    (1 << 5)
#define DWMCI_CTRL_SEND_AS_CCSD	(1 << 10)
#define DWMCI_IDMAC_EN		    (1 << 25)
#define DWMCI_RESET_ALL		    (DWMCI_CTRL_RESET | DWMCI_CTRL_FIFO_RESET |\
				                DWMCI_CTRL_DMA_RESET)

/* CMD register */
#define DWMCI_CMD_RESP_EXP	    (1 << 6)
#define DWMCI_CMD_RESP_LENGTH	(1 << 7)
#define DWMCI_CMD_CHECK_CRC	    (1 << 8)
#define DWMCI_CMD_DATA_EXP	    (1 << 9)
#define DWMCI_CMD_RW		    (1 << 10)
#define DWMCI_CMD_SEND_STOP	    (1 << 12)
#define DWMCI_CMD_ABORT_STOP    (1 << 14)
#define DWMCI_CMD_PRV_DAT_WAIT	(1 << 13)
#define DWMCI_CMD_UPD_CLK	    (1 << 21)
#define DWMCI_CMD_USE_HOLD_REG	(1 << 29)
#define DWMCI_CMD_START		    (1 << 31)

/* CLKENA register */
#define DWMCI_CLKEN_ENABLE	(1 << 0)
#define DWMCI_CLKEN_LOW_PWR	(1 << 16)

/* Card-type registe */
#define DWMCI_CTYPE_1BIT	0
#define DWMCI_CTYPE_4BIT	(1 << 0)
#define DWMCI_CTYPE_8BIT	(1 << 16)

/* Status Register */
#define DWMCI_BUSY		    (1 << 9)
#define DWMCI_FIFO_MASK	    0x1fff
#define DWMCI_FIFO_SHIFT    17

/* FIFOTH Register */
#define MSIZE(x)		    ((x) << 28)
#define RX_WMARK(x)		    ((x) << 16)
#define TX_WMARK(x)		    (x)
#define RX_WMARK_SHIFT		16
#define RX_WMARK_MASK		(0xfff << RX_WMARK_SHIFT)

#define DWMCI_IDMAC_OWN		(1 << 31)
#define DWMCI_IDMAC_CH		(1 << 4)
#define DWMCI_IDMAC_FS		(1 << 3)
#define DWMCI_IDMAC_LD		(1 << 2)

/*  Bus Mode Register */
#define DWMCI_BMOD_IDMAC_RESET	(1 << 0)
#define DWMCI_BMOD_IDMAC_FB	    (1 << 1)
#define DWMCI_BMOD_IDMAC_EN	    (1 << 7)

/* UHS register */
#define DWMCI_DDR_MODE	        (1 << 16)

/* quirks */
#define DWMCI_QUIRK_DISABLE_SMU		(1 << 0)

unsigned int	drvsel  =3;     // dts data DE10 nano
unsigned int	smplsel =0;     // dts data DE10 nano

#define SOCFPGA_CLKMGR_ADDRESS		0xffd04000
#define CLKMGR_PERIPHPLL_EN			SOCFPGA_CLKMGR_ADDRESS + 0xA0

#define SOCFPGA_SYSMGR_ADDRESS		0xffd08000



static inline void dwmci_writel(int reg, u32 val)
{
	writel(val, SDMMC_BASE_ADDRESS + reg);
}

static inline void dwmci_writew(int reg, u16 val)
{
	writew(val, SDMMC_BASE_ADDRESS + reg);
}

static inline void dwmci_writeb(int reg, u8 val)
{
	writeb(val, SDMMC_BASE_ADDRESS + reg);
}
static inline u32 dwmci_readl(int reg)
{
	return readl(SDMMC_BASE_ADDRESS + reg);
}

static inline u16 dwmci_readw(int reg)
{
	return readw(SDMMC_BASE_ADDRESS + reg);
}

static inline u8 dwmci_readb(int reg)
{
	return readb(SDMMC_BASE_ADDRESS + reg);
}

void    mmc_clksel()
{
	//struct dwmci_socfpga_priv_data *priv = host->priv;
	//u32 sdmmc_mask = ((priv->smplsel & 0x7) << SYSMGR_SDMMC_SMPLSEL_SHIFT) |
	//		 ((priv->drvsel & 0x7) << SYSMGR_SDMMC_DRVSEL_SHIFT);
	
    // Cyclone Gen 5
    #define SYSMGR_SDMMC_SMPLSEL_SHIFT	            3
    #define SYSMGR_SDMMC_DRVSEL_SHIFT	            0
    #define CLKMGR_PERPLLGRP_EN_SDMMCCLK_MASK		0x00000100

    u32 sdmmc_mask = ((smplsel & 0x7) << SYSMGR_SDMMC_SMPLSEL_SHIFT) | ((drvsel & 0x7) << SYSMGR_SDMMC_DRVSEL_SHIFT);

	/* Disable SDMMC clock. */
	//clrbits_le32(&clock_manager_base->per_pll.en, CLKMGR_PERPLLGRP_EN_SDMMCCLK_MASK);
	clrbits_le32(CLKMGR_PERIPHPLL_EN, CLKMGR_PERPLLGRP_EN_SDMMCCLK_MASK);

	//debug("%s: drvsel %d smplsel %d\n", __func__, priv->drvsel, priv->smplsel);
	//writel(sdmmc_mask, &system_manager_base->sdmmcgrp_ctrl);
    writel(sdmmc_mask, SOCFPGA_SYSMGR_ADDRESS + 0x0108);

	//debug("%s: SYSMGR_SDMMCGRP_CTRL_REG = 0x%x\n", __func__, readl(&system_manager_base->sdmmcgrp_ctrl));

	/* Enable SDMMC clock */
	//setbits_le32(&clock_manager_base->per_pll.en, CLKMGR_PERPLLGRP_EN_SDMMCCLK_MASK);
	setbits_le32(CLKMGR_PERIPHPLL_EN, CLKMGR_PERPLLGRP_EN_SDMMCCLK_MASK);

}


int dwmci_wait_reset(u32 value)
{
	unsigned long timeout = 1000;
	u32 ctrl;

	dwmci_writel(DWMCI_CTRL, value);

	while (timeout--) 
    {
		ctrl = dwmci_readl(DWMCI_CTRL);
		if (!(ctrl & DWMCI_RESET_ALL)) return 1;
	}

	return 0;
}

void    mmc_init()
{
	uint32_t    fifo_size;
    uint32_t    div, status;
	int         timeout = 10000;
	uint32_t    fifoth_val;

   	dwmci_writel(DWMCI_PWREN, 1);


	if (!dwmci_wait_reset(DWMCI_RESET_ALL)) 
    {
		FDy_puts("MMC Fail-reset!!\n\r");
		return -EIO;
	}

	/* Enumerate at 400KHz */
	//dwmci_setup_bus(mmc->cfg->f_min);

	dwmci_writel(DWMCI_CLKENA, 0);
	dwmci_writel(DWMCI_CLKSRC, 0);

    //div =0; // no divider - bypass
    div =256; // divider
    

	dwmci_writel(DWMCI_CLKDIV, div);
	dwmci_writel(DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	do 
    {
		status = dwmci_readl(DWMCI_CMD);
		if (timeout-- < 0) 
        {
			FDy_puts("Timeout!\n\r");
			return -21;
		}
	} while (status & DWMCI_CMD_START);

	dwmci_writel(DWMCI_CLKENA, DWMCI_CLKEN_ENABLE |	DWMCI_CLKEN_LOW_PWR);

	dwmci_writel(DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do 
    {
		status = dwmci_readl(DWMCI_CMD);
		if (timeout-- < 0) 
        {
			FDy_puts("Timeout!\n\r");
			return -21;
		}
	} while (status & DWMCI_CMD_START);

    FDy_puts("status = ");
	FDy_putHexa32(status);      // 0x 0    0    2    0    2    0    0    0
                                // 0b 0000 0000 0010 0000 0010 0000 0000 0000
    FDy_puts("\r\n");


	dwmci_writel(DWMCI_RINTSTS, 0xFFFFFFFF);
	dwmci_writel(DWMCI_INTMASK, 0);

	dwmci_writel(DWMCI_TMOUT, 0xFFFFFFFF);

	dwmci_writel(DWMCI_IDINTEN, 0);
	dwmci_writel(DWMCI_BMOD, 1);


	fifo_size = dwmci_readl(DWMCI_FIFOTH);
	fifo_size = ((fifo_size & RX_WMARK_MASK) >> RX_WMARK_SHIFT) + 1;
	fifoth_val = MSIZE(0x2) | RX_WMARK(fifo_size / 2 - 1) | TX_WMARK(fifo_size / 2);

	dwmci_writel(DWMCI_FIFOTH, fifoth_val);


	dwmci_writel(DWMCI_CLKENA, 0);
	dwmci_writel(DWMCI_CLKSRC, 0);

    dwmci_writel(DWMCI_CTYPE, DWMCI_CTYPE_4BIT);

    // FDy part

	//unsigned int uiCType 	    = readl(SDMMC_BASE_ADDRESS + 0x18);
	//unsigned int uiCDetect 	= readl(SDMMC_BASE_ADDRESS + 0x50);
	

// struct mmc_cmd {
// 	ushort cmdidx;
// 	uint resp_type;
// 	uint cmdarg;
// 	uint response[4];
// };

// struct mmc_data {
// 	union {
// 		char *dest;
// 		const char *src; /* src buffers don't get written to */
// 	};
// 	uint flags;
// 	uint blocks;
// 	uint blocksize;
// };

// 	struct mmc_cmd cmd;
// 	struct mmc_data data;
// 	int err;

// 	/* Get the Card Status Register */
// 	cmdidx = MMC_CMD_SEND_EXT_CSD;
// 	resp_type = MMC_RSP_R1;
// 	cmdarg = 0;

// 	blocks = 1;
// 	blocksize = MMC_MAX_BLOCK_LEN;
// 	flags = MMC_DATA_READ;

    /* Maximum block size for MMC */
    #define MMC_MAX_BLOCK_LEN	512
    #define MMC_DATA_READ		1
    #define MMC_DATA_WRITE		2

    #define MMC_CMD_GO_IDLE_STATE		    0
    #define MMC_CMD_SEND_OP_COND		    1
    #define MMC_CMD_ALL_SEND_CID		    2
    #define MMC_CMD_SET_RELATIVE_ADDR	    3
    #define MMC_CMD_SET_DSR			        4
    #define MMC_CMD_SWITCH			        6
    #define MMC_CMD_SELECT_CARD		        7
    #define MMC_CMD_SEND_EXT_CSD		    8
    #define MMC_CMD_SEND_CSD		        9
    #define MMC_CMD_SEND_CID		        10
    #define MMC_CMD_STOP_TRANSMISSION	    12
    #define MMC_CMD_SEND_STATUS		        13
    #define MMC_CMD_SET_BLOCKLEN		    16
    #define MMC_CMD_READ_SINGLE_BLOCK	    17
    #define MMC_CMD_READ_MULTIPLE_BLOCK	    18
    #define MMC_CMD_SET_BLOCK_COUNT         23
    #define MMC_CMD_WRITE_SINGLE_BLOCK	    24
    #define MMC_CMD_WRITE_MULTIPLE_BLOCK    25
    #define MMC_CMD_ERASE_GROUP_START	    35
    #define MMC_CMD_ERASE_GROUP_END		    36
    #define MMC_CMD_ERASE			        38
    #define MMC_CMD_APP_CMD			        55
    #define MMC_CMD_SPI_READ_OCR		    58
    #define MMC_CMD_SPI_CRC_ON_OFF		    59
    #define MMC_CMD_RES_MAN			        62

	dwmci_writel(DWMCI_RINTSTS, DWMCI_INTMSK_ALL);
	dwmci_writel(DWMCI_BLKSIZ, MMC_MAX_BLOCK_LEN);
	dwmci_writel(DWMCI_BYTCNT, MMC_MAX_BLOCK_LEN * 1);
	dwmci_wait_reset(DWMCI_CTRL_FIFO_RESET);

    dwmci_writel(DWMCI_CMDARG, 0);
 
	unsigned long mode;
	int flags = (DWMCI_CMD_DATA_EXP | MMC_DATA_READ | MMC_CMD_SEND_EXT_CSD | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG);

	if (flags & MMC_DATA_WRITE)		flags |= DWMCI_CMD_RW;

 
	dwmci_writel(DWMCI_CMD, flags);
 
    
    // wait 100ms
    uint64_t ui64InitialValue =	get_ticks();
	while(1)
	{
		uint64_t ui64CurrentValue =	get_ticks();
		if(ui64InitialValue-ui64CurrentValue>2500000)	break;

	}


    unsigned int    uiResp00 = dwmci_readl(DWMCI_RESP0);
    unsigned int    uiResp01 = dwmci_readl(DWMCI_RESP1);
    unsigned int    uiResp02 = dwmci_readl(DWMCI_RESP2);
    unsigned int    uiResp03 = dwmci_readl(DWMCI_RESP3);

    FDy_puts("uiResp00 = ");
	FDy_putHexa32(uiResp00);
    FDy_puts(" , uiResp01 = ");
	FDy_putHexa32(uiResp01);
    FDy_puts(" , uiResp02 = ");
	FDy_putHexa32(uiResp02);
    FDy_puts(" , uiResp03 = ");
	FDy_putHexa32(uiResp03);
    
    FDy_puts("\n\r");

}