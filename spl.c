#include "common.h"
#include "hwlibs/alt_sdmmc.h"

// ---- ClockManager ----
void FDy_cm_basic_init(void);

// ---- FreezeManager ----
void FDy_sys_mgr_frzctrl_freeze_req(void);
void FDy_sys_mgr_frzctrl_thaw_req(void);

// ---- ScanManager ----
int FDy_scan_mgr_configure_iocsr(void);
void FDy_sysmgr_pinmux_init(void);

// ---- FpgaManager ----
int fpgamgr_test_fpga_ready(void);
int socfpga_load(const void *rbf_data, size_t rbf_size);

// ---- sdram.c ----
int sdram_mmr_init_full(unsigned int sdr_phy_reg);
unsigned long sdram_calculate_size(void);

// ---- sequencer.c ----
int sdram_calibration_full(void);

// for printi
#define USE_DIVIDE
/*
uint64_t int_tables[19][9] = 
{
          {  9000000000000000000ul, 8000000000000000000ul, 7000000000000000000ul, 
             6000000000000000000ul, 5000000000000000000ul, 4000000000000000000ul, 
             3000000000000000000ul, 2000000000000000000ul, 1000000000000000000ul}, 
          {  900000000000000000ul, 800000000000000000ul, 700000000000000000ul, 
             600000000000000000ul, 500000000000000000ul, 400000000000000000ul, 
             300000000000000000ul, 200000000000000000ul, 100000000000000000ul}, 
          {  90000000000000000ul, 80000000000000000ul, 70000000000000000ul, 
             60000000000000000ul, 50000000000000000ul, 40000000000000000ul, 
             30000000000000000ul, 20000000000000000ul, 10000000000000000ul}, 
          {  9000000000000000ul, 8000000000000000ul, 7000000000000000ul, 
             6000000000000000ul, 5000000000000000ul, 4000000000000000ul, 
             3000000000000000ul, 2000000000000000ul, 1000000000000000ul}, 
          {  900000000000000ul, 800000000000000ul, 700000000000000ul, 
             600000000000000ul, 500000000000000ul, 400000000000000ul, 
             300000000000000ul, 200000000000000ul, 100000000000000ul}, 
          {  90000000000000ul, 80000000000000ul, 70000000000000ul, 
             60000000000000ul, 50000000000000ul, 40000000000000ul, 
             30000000000000ul, 20000000000000ul, 10000000000000ul}, 
          {  9000000000000ul, 8000000000000ul, 7000000000000ul, 
             6000000000000ul, 5000000000000ul, 4000000000000ul,
             3000000000000ul, 2000000000000ul, 1000000000000ul},
          {  900000000000ul, 800000000000ul, 700000000000ul, 600000000000ul,
             500000000000ul, 400000000000ul, 300000000000ul, 200000000000ul,
             100000000000ul}, 
          {  90000000000ul, 80000000000ul, 70000000000ul, 60000000000ul,
             50000000000ul, 40000000000ul, 30000000000ul, 20000000000ul,
             10000000000ul}, 
          {  9000000000u, 8000000000u, 7000000000, 6000000000, 5000000000, 
             4000000000, 3000000000, 2000000000, 1000000000}, 
          {  900000000, 800000000, 700000000, 600000000, 
             500000000, 400000000, 300000000, 200000000, 100000000},
          {  90000000, 80000000, 70000000, 60000000, 
             50000000, 40000000, 30000000, 20000000, 10000000},
          {  9000000, 8000000, 7000000, 6000000, 
             5000000, 4000000, 3000000, 2000000, 1000000},
          {  900000, 800000, 700000, 600000, 
             500000, 400000, 300000, 200000, 100000},
          {90000, 80000, 70000, 60000, 50000, 40000, 30000, 20000, 10000},
          {9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000},
          {900, 800, 700, 600, 500, 400, 300, 200, 100},
          {90, 80, 70, 60, 50, 40, 30, 20, 10},
          {9, 8, 7, 6, 5, 4, 3, 2, 1}
};
*/

#define 	__THIS_VERSION__		"v0.0.3"
// menu.rbf en partition 1 :
//#define 	MENU_RBF_ADDRESS		6144*512
#define	 	MENU_RBF_ADDRESS		3145728
#define 	MENU_RBF_SIZE			2399864
#define 	MENU_RBF_SIZE_ROUND		2400256

// HDMI_TX.rbf en partition 2 :
//#define 	HDMI_TX_RBF_ADDRESS		210944*512
//#define 	HDMI_TX_RBF_ADDRESS		108003328
// HDMI_TX.rbf en partition 1 :
//#define 	HDMI_TX_RBF_ADDRESS		6144*512
#define	 	HDMI_TX_RBF_ADDRESS		3145728
#define 	HDMI_TX_RBF_SIZE		7007204
#define 	HDMI_TX_RBF_SIZE_ROUND	7007232

#define 	SECTOR_SIZE				512
	

uint32_t	timerValueHigh;
uint32_t	timerValueLow;

char		tempBuffer[20];

// Card information
ALT_SDMMC_CARD_INFO_t Card_Info;
ALT_SDMMC_CARD_MISC_t card_misc_cfg;

// SD/MMC Device size - hardcode max supported size for now
uint64_t Sdmmc_Device_Size;

// SD/MMC Block size
uint32_t Sdmmc_Block_Size;

/* Maximum size of test data */
#define MAX_TEST_BYTES  1024

uint32_t address;
uint32_t size;

// Buffers used for testing
//uint32_t Write_Buffer[MAX_TEST_BYTES / sizeof(uint32_t)] ;
//uint32_t Read_Buffer[MAX_TEST_BYTES / sizeof(uint32_t)];
//uint32_t *Read_Buffer;

#define PAD_RIGHT       1
#define PAD_ZERO        2
#define PAD_0x          4
#define PAD_PLUS        8

static inline void sync(void)
{
}


uint64_t  get_ticks(void)
{
	unsigned int uiCurrentValue =	readl(CONFIG_SYS_TIMERBASE + 0x04);
			
	/* increment tbu if tbl has rolled over */
	if (uiCurrentValue > timerValueLow)
		timerValueHigh++;

	timerValueLow = uiCurrentValue;	

	uint64_t	ui64returnValue = (uint64_t)(((uint64_t)timerValueHigh << 32) | timerValueLow);
	return 		ui64returnValue;
}


void FDy_putc(char c)
{
	volatile unsigned int*	Uart0_lsr 		= (unsigned int *)0xFFC02014;
	//#define UART_LSR_THRE 0x20

	//while ((serial_in(&com_port->lsr) & UART_LSR_THRE) == 0)
	while((*Uart0_lsr & 0x20)==0)	;	// check xmit empty
	
	volatile unsigned int*	Uart0Data = (unsigned int *)0xFFC02000;
	*Uart0Data = (unsigned int) (c);

}

void FDy_PutNible(unsigned char ucNibble)
{
	ucNibble = ucNibble + 0x30;
	if(ucNibble>0x39)	ucNibble+=0x07;	// 0x41 ('A') - 0x3A (>'9')

	FDy_putc(ucNibble);
}

void FDy_PutByte(unsigned char ucByte)
{
	unsigned char ucCarac = 0;

	ucCarac = (ucByte >> 4 ) & 0x0F;
	FDy_PutNible(ucCarac);

	ucCarac = ucByte & 0x0F;
	FDy_PutNible(ucCarac);
}

void FDy_putHexa32(unsigned int uiNumber)
{
	FDy_putc('0');
	FDy_putc('x');
	FDy_PutByte((uiNumber >> 24) & 0xFF);
	FDy_PutByte((uiNumber >> 16) & 0xFF);
	FDy_PutByte((uiNumber >>  8) & 0xFF);
	FDy_PutByte( uiNumber        & 0xFF);
}

void FDy_putHexa64(unsigned long long ullNumber)
{
	FDy_putc('0');
	FDy_putc('x');

	FDy_PutByte((ullNumber >> 56) & 0xFF);
	FDy_PutByte((ullNumber >> 48) & 0xFF);
	FDy_PutByte((ullNumber >> 40) & 0xFF);
	FDy_PutByte((ullNumber >> 32) & 0xFF);
	
	FDy_PutByte((ullNumber >> 24) & 0xFF);
	FDy_PutByte((ullNumber >> 16) & 0xFF);
	FDy_PutByte((ullNumber >>  8) & 0xFF);
	FDy_PutByte( ullNumber        & 0xFF);
}

void FDy_puts(char *caMessage)
{
	while(*caMessage!=0)
	{
		FDy_putc(*caMessage++);
	}
}

void HexDump(unsigned char* pucStartAddr, unsigned int uiSize)
{
	char c = 0;
	
	for(unsigned int 	uiCounter =0;
						uiCounter<uiSize;
						uiCounter+=16)
	{
		FDy_putHexa32((unsigned int)pucStartAddr + uiCounter);
		FDy_puts(" - ");
		memset(tempBuffer, 0, 20);

		for(unsigned int 	uiLineCounter=0;
							uiLineCounter<16;
							uiLineCounter++)
		{
			if(uiCounter+uiLineCounter<= uiSize)
			{
				c = (char)pucStartAddr[uiCounter+uiLineCounter];
				FDy_PutByte(c);
				FDy_putc(' ');
				if(c<32||c>'z')	tempBuffer[uiLineCounter]='.';
				else 			tempBuffer[uiLineCounter]=c;
			}	
			else
			{
				FDy_puts("   ");
				tempBuffer[uiLineCounter] = ' ';
			}
		}

		FDy_puts("- ");
		FDy_puts(tempBuffer);
		FDy_puts("\n\r");
	}
}


int prints(const char *string, int width, int pad)
{
    int padchar = ' ';
    int printed = 0;

    if (width > 0)
    {
        int len = 0;
        const char *ptr;
        
        for (ptr = string; *ptr; ++ptr) 
            ++len;
        
        if (len >= width) 
            width = 0;
        else 
            width -= len;
        
        if (pad & PAD_ZERO) 
            padchar = '0';
    }

    if (!(pad & PAD_RIGHT))
    {
        for ( ; width > 0; --width)
        {
    	    FDy_putc(padchar);
            printed++;
        }
    }

    for ( ; *string ; ++string)
    {
    	FDy_putc(*string);
        printed++;
    }

    for ( ; width > 0; --width)
    {
    	FDy_putc(padchar);
        printed++;
    }

    return printed;
}

/*
        p_op:    structure that indicates where the new characters will go
        i:       value to print
        b:       base (10 or 16 usually)
        sg:      signed (1) or unsigned (0)
        width:
        pad:     Set of flags for padding:
                 pad with 0's or spaces (1) or no (0)
                  PAD_RIGHT - add spaces
                  PAD_ZERO - add 0's
                  PAD_0x - prepend with 0x
                  PAD_PLUS - add + to positive numbers
        letbase: letter base ('a' or 'A') for hex
*/


/* the following should be enough for 64 bit int */
#define PRINT_BUF_LEN 22

int printi(int64_t i, int b, int sg, int width, int pad, int letbase)
{
    char           print_buf[PRINT_BUF_LEN];
    char*          s;
    int            t, neg = 0;
    uint64_t       u = i;
    int            printed=0;
    char           scr[3];

    if(pad & PAD_0x)
    {
        scr[0] = '0';
        scr[1] = 'x';
        scr[2] = 0;
        printed += prints(scr, width, pad);
    }
    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0)
    {
        neg = 1;
        u = -i;
    }

    s  = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

#ifdef USE_DIVIDE
    while (u)
    {
        t = u % b;
        if( t >= 10 )
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }
#else
    if(b == 16)
    {
	while(u)
        {
            int count=0;
            t = u & 0xF;
            if( t >= 10 )
                t += letbase - '0' - 10;
            *--s = t + '0';
            u >>= 4;
        }
    }
    else
    {

        int count;
        int offset;
        int hasprinted=0;

        s = &print_buf[0];
        for(offset = 0; offset < 19; offset++)
        {
            t = 0;
            for(count=0; count < 9; count++)
            {
                if(int_tables[offset][count] <= u)
                {
                    u -= int_tables[offset][count];
                    t = 9-count;
                    if(offset == 0)
                        t = 4-count;
                }
            }
            if(hasprinted || t != 0)
            {
                *(++s) = t + '0';
                hasprinted = 1;
            }
        }
        *++s = '\0';
        s = &print_buf[1];
    }
#endif
	
    if (pad & PAD_PLUS)
    {
        printed++;
        FDy_putc('+');
        --width;
    }
    if (neg)
    {
        if( width && (pad & PAD_ZERO) )
        {
            printed++;
            FDy_putc('-');
            --width;
        }
        else
        {
            *--s = '-';
        }
    }

    return printed + prints(s, width, pad);
}

unsigned char 	ucLed[10] = {1,0,1,1,0,0,0,0,0,0};

void mainLoop(void)
{


	// Set everything as output
	volatile unsigned int*	ledDdr 	= (unsigned int *)0xFF709004;
	*ledDdr = (unsigned int) (0xFFFFFFFF);
	
	// Set Led HPS to 1 (On)
	volatile unsigned int*	ledData = (unsigned int *)0xFF709000;
	*ledData = (unsigned int) (1 <<24);

	// exemple
	//setbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_EN_MASK);
	//clrbits_le32(&fpgamgr_regs->ctrl, FPGAMGRREGS_CTRL_NCONFIGPULL_MASK);

	// exemple
	/*
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
	*/

	unsigned char		ucLedCounter 	=0;
	//int 		iStar 			=0;
	//char		caStar[]={'-', '/', '|', '\\'};
//	unsigned	int		uiCounter = 0;

	// to try hexdump...
	//HexDump((unsigned char*)0xFFFF0000, 512);

	while(1)
	{
		//serial_puts(".");
		
		//volatile unsigned int*	Uart0Data = (unsigned int *)0xFFC02000;
		//*Uart0Data = (unsigned int) ('*');
//		FDy_putc(caStar[iStar]);
//		FDy_putHexa32(uiCounter);
//		FDy_putc(10);
//		FDy_putc(13);
		
	//	for(int i=0;i<5000000;i++)
	//	{
	//		volatile	int	j=i*2;
	//	}
		//udelay(500 * 1000);		// 500 ms

/*
		unsigned int iInitialValue =	readl(CONFIG_SYS_TIMERBASE + 0x04);
		while(1)
		{
			unsigned int iCurrentValue =	readl(CONFIG_SYS_TIMERBASE + 0x04);
			if(iInitialValue-iCurrentValue>12500000)	break;
			// 50000000 a peu pres 2s
			// 12500000 a peu pres 500ms
			
			for(int i=0;i<5000;i++)
			{
				volatile	int	j=i*2;
			}
	
		}
*/

		// si fosc = 25 MHz
		// 1 tick = 40ns
		// 500ms = 12500000 ticks
		// 100ms =  2500000 ticks
 		// 2s    = 50000000 ticks
		// valeurs plus ou moins vérifiées par essais

		uint64_t ui64InitialValue =	get_ticks();
		while(1)
		{
			uint64_t ui64CurrentValue =	get_ticks();
			if(ui64InitialValue-ui64CurrentValue>5000000)	break;

			/*	
			for(int i=0;i<5000;i++)
			{
				volatile	int	j=i*2;
			}
			*/
		}

		/*
		if(1==iLed)
		{
			*ledData = (unsigned int) (0);
			iLed =0;
			//uiCounter = 0x55;
		}
		else
		{
			*ledData = (unsigned int) (1 <<24);
			iLed =1;
			//uiCounter = 0xAA;
		}
		*/

		if(ucLedCounter>9)		ucLedCounter=0;

		if(0==ucLed[ucLedCounter])
		{
			*ledData = (unsigned int) (0);
		}
		else
		{
			*ledData = (unsigned int) (1 <<24);
		}

		ucLedCounter++;
//		uiCounter++;

		/*
		iStar++;
		if(iStar>3)	iStar=0;
		
		FDy_putc(8);
		*/
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);
//		FDy_putc(8);

	}

}


/*
 * This requires UART clocks to be enabled.  In order for this to work the
 * caller must ensure that the gd pointer is valid.
 */
void FDy_preloader_console_init(void)
{
/* Recap init serial */
/*
	ier	0xFFC02000 + 4	(0x04)	<-	0x00		CLR Irq
	mcr	0xFFC02000 + 16	(0x10)	<-	0x03		RTS + DTR
	fcr	0xFFC02000 + 8	(0x08)	<-	0x07		Reset & enable FIFO
	lcr	0xFFC02000 + 12	(0x0C)	<-	0x83		dlab = 1 (access dll & dlm) 
								                + dls = 0b11 -> data 8bits
	dll	0xFFC02000 + 0	(0x00)	<-	8bits poids faible ValueBaudRate 	= 54
	dlm	0xFFC02000 + 4  (0x04)	<-	8bits poids fort   ValueBaudRate 	= 0
	lcr	0xFFC02000 + 12	(0x0C)	<-	0x03		dlab = 0 (clear access to dll & dlm) 
								                + dls = 0b11 -> data 8bits
*/

	volatile unsigned int*	Uart0_rbr_thr_dll 	= (unsigned int *)0xFFC02000;
	volatile unsigned int*	Uart0_ier_dlh 		= (unsigned int *)0xFFC02004;
	volatile unsigned int*	Uart0_fcr 			= (unsigned int *)0xFFC02008;
	volatile unsigned int*	Uart0_lcr 			= (unsigned int *)0xFFC0200C;
	volatile unsigned int*	Uart0_mcr 			= (unsigned int *)0xFFC02010;


	*Uart0_ier_dlh 		= (unsigned int) (0x00);
	*Uart0_mcr 			= (unsigned int) (0x03);
	*Uart0_fcr			= (unsigned int) (0x07);
	*Uart0_lcr 			= (unsigned int) (0x83);
	*Uart0_rbr_thr_dll	= (unsigned int) (54);
	*Uart0_ier_dlh 		= (unsigned int) (0);
	*Uart0_lcr 			= (unsigned int) (0x03);
	
	FDy_puts("\n\r");
	FDy_puts("\n\r");
	FDy_puts("                                       \\o.o/\n\r");
	FDy_puts("---------------------------------.ooO---(_)---Ooo.----------\n\r");
	FDy_puts("\n\r"); 
	FDy_puts("    BareMetal Boot inspired by U-BOOT\n\r");
	FDy_puts("    Custom Version by FDy - " __THIS_VERSION__ " ("__DATE__" - "__TIME__")\n\r");	
	FDy_puts("\n\r"); 
	FDy_puts("--------------------------------------------Oooo.-----------\n\r");
	FDy_puts("                                     .oooO  (   )\n\r");
	FDy_puts("                                     (   )   ) /\n\r");
	FDy_puts("                                      \\ (   (_/\n\r");
	FDy_puts("                                       \\_)\n\r");
	FDy_puts("\n\r");
	FDy_puts("\n\r");
	
}



#define L3REGS_REMAP_LWHPS2FPGA_MASK	0x10
#define L3REGS_REMAP_HPS2FPGA_MASK		0x08
#define L3REGS_REMAP_OCRAM_MASK			0x01

void socfpga_bridges_reset(int enable)
{
	const u32 l3mask = L3REGS_REMAP_LWHPS2FPGA_MASK |
				L3REGS_REMAP_HPS2FPGA_MASK |
				L3REGS_REMAP_OCRAM_MASK;

	if (enable) 
	{
		/* brdmodrst */
		//writel(0xffffffff, &reset_manager_base->brg_mod_reset);
		writel(0xffffffff, SOCFPGA_RSTMGR_ADDRESS + 0x1C);
	} 
	else 
	{
		//writel(0, &sysmgr_regs->iswgrp_handoff[0]);
		writel(0, SOCFPGA_SYSMGR_ADDRESS + 0x80);

		//writel(l3mask, &sysmgr_regs->iswgrp_handoff[1]);
		writel(l3mask, SOCFPGA_SYSMGR_ADDRESS + 0x84); 

		/* Check signal from FPGA. */
		if (!fpgamgr_test_fpga_ready()) 
		{
			/* FPGA not ready, do nothing. */
			//FDy_puts("FPGA not ready, aborting.\n\r");
			return;
		}

		/* brdmodrst */
		//writel(0, &reset_manager_base->brg_mod_reset);
		writel(0, SOCFPGA_RSTMGR_ADDRESS + 0x1C);

		/* Remap the bridges into memory map */
		writel(l3mask, SOCFPGA_L3REGS_ADDRESS);
	}
}
/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'.
 */
long get_ram_size(long *base, long maxsize)
{
	volatile long *addr;
	long           save[32];
	long           cnt;
	long           val;
	long           size;
	int            i = 0;

	for (cnt = (maxsize / sizeof(long)) >> 1; cnt > 0; cnt >>= 1) {
		addr = base + cnt;	/* pointer arith! */
		sync();
		save[i++] = *addr;
		sync();
		*addr = ~cnt;
	}

	addr = base;
	sync();
	save[i] = *addr;
	sync();
	*addr = 0;

	sync();
	if ((val = *addr) != 0) {
		/* Restore the original data before leaving the function. */
		sync();
		*addr = save[i];
		for (cnt = 1; cnt < maxsize / sizeof(long); cnt <<= 1) {
			addr  = base + cnt;
			sync();
			*addr = save[--i];
		}
		return (0);
	}

	for (cnt = 1; cnt < maxsize / sizeof(long); cnt <<= 1) {
		addr = base + cnt;	/* pointer arith! */
		val = *addr;
		*addr = save[--i];
		if (val != ~cnt) {
			size = cnt * sizeof(long);
			/*
			 * Restore the original data
			 * before leaving the function.
			 */
			for (cnt <<= 1;
			     cnt < maxsize / sizeof(long);
			     cnt <<= 1) {
				addr  = base + cnt;
				*addr = save[--i];
			}
			return (size);
		}
	}

	return (maxsize);
}

void ReadAndDisplaySector(unsigned int uiSector)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
	unsigned int 	uiSectorSize = 512;
	unsigned int 	uiDestAddress =0;
	//address = 0x00000000;
	//size 	= 0x0200;

	FDy_puts("INFO: Reading from address ");
	FDy_putHexa32(uiSector*uiSectorSize);
	FDy_puts(", size = 512 bytes using block I/O.\n\r");

	uiDestAddress = 0x1000;
	memset(uiDestAddress, 0, uiSectorSize);
	status = alt_sdmmc_read(&Card_Info, uiDestAddress, (void*)(uiSector*uiSectorSize), uiSectorSize);
	if(status ==  ALT_E_SUCCESS)
	{
		FDy_puts("Read is Ok\n\r");
		HexDump(uiDestAddress, uiSectorSize);
	}
	else
	{
		FDy_puts("Read is Ko\n\r");
	}
	
}
	
void board_init_f(void)
{
	unsigned long 	reg;
	int 			sdram_size;
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
	unsigned int 	uiDestAddress =0;

	/*
	 * First C code to run. Clear fake OCRAM ECC first as SBE
	 * and DBE might triggered during power on
	 */

	#define SYSMGR_ECC_OCRAM_EN		0x01
	#define SYSMGR_ECC_OCRAM_SERR	0x08
	#define SYSMGR_ECC_OCRAM_DERR	0x10


	//reg = readl(&sysmgr_regs->eccgrp_ocram);	/* 0x140 */
	reg = readl(SOCFPGA_SYSMGR_ADDRESS + 0x0140);

	//if (reg & SYSMGR_ECC_OCRAM_SERR)		writel(SYSMGR_ECC_OCRAM_SERR | SYSMGR_ECC_OCRAM_EN, &sysmgr_regs->eccgrp_ocram);
	if (reg & SYSMGR_ECC_OCRAM_SERR)		writel(SYSMGR_ECC_OCRAM_SERR | SYSMGR_ECC_OCRAM_EN, SOCFPGA_SYSMGR_ADDRESS + 0x0140);

	//if (reg & SYSMGR_ECC_OCRAM_DERR)		writel(SYSMGR_ECC_OCRAM_DERR | SYSMGR_ECC_OCRAM_EN, &sysmgr_regs->eccgrp_ocram);
	if (reg & SYSMGR_ECC_OCRAM_DERR)		writel(SYSMGR_ECC_OCRAM_DERR | SYSMGR_ECC_OCRAM_EN, SOCFPGA_SYSMGR_ADDRESS + 0x0140);

	//memset(__bss_start, 0, __bss_end - __bss_start);

	// socfpga_nic301_slave_ns
	// access to slave non secured :
	//writel(0x1, &nic301_regs->lwhps2fpgaregs);	/* 0x20 */
	//writel(0x1, &nic301_regs->hps2fpgaregs);		/* 0x90 */
	//writel(0x1, &nic301_regs->acp);				/* 0x94 */
	//writel(0x1, &nic301_regs->rom);				/* 0x98 */
	//writel(0x1, &nic301_regs->ocram);				/* 0x9C */
	//writel(0x1, &nic301_regs->sdrdata);			/* 0xA0 */

	writel(0x01, NIC301_LWHPS2FPGAREGS	);
	writel(0x01, NIC301_HPS2FPGAREGS	);
	writel(0x01, NIC301_ACP				);
	writel(0x01, NIC301_ROM				);
	writel(0x01, NIC301_OCRAM			);
	writel(0x01, NIC301_SDRDATA			);


	/* Configure ARM MPU SNSAC register. */
	//setbits_le32(&scu_regs->sacr, 0xfff);	/* 0x54, attention a verifier */
	setbits_le32(SOCFPGA_MPUSCU_ADDRESS + 0x54, 0x0FFF);

	/* Remap SDRAM to 0x0 */
	//writel(0x1, &nic301_regs->remap);	/* remap.mpuzero */
	writel(0x01, SOCFPGA_L3REGS_ADDRESS + 0x00);

	//writel(0x1, &pl310->pl310_addr_filter_start);
	//writel(0x1, 0xFFFEFC00);	// CONFIG_SYS_PL310_BASE + 0x0C00
	writel(0x1, CONFIG_SYS_PL310_BASE + 0x0C00);	// CONFIG_SYS_PL310_BASE + addr_filter_start 


	/* freeze all IO banks */
	FDy_sys_mgr_frzctrl_freeze_req();	// freeze_controller.c

	/* Put everything into reset but L4WD0. */
	//socfpga_per_reset_all(void) :
	//const u32 l4wd0 = 1 << RSTMGR_RESET(SOCFPGA_RESET(L4WD0));
	//const u32 l4wd0 = 1 << RSTMGR_RESET(RSTMGR_L4WD0);
	const u32 l4wd0 = 0x40; 	// bit 6 (l4wd0) de permodrst 
	//writel(~l4wd0, &reset_manager_base->per_mod_reset);
	writel(~l4wd0, RSTMGR_PERMODRESET);
	//writel(0xffffffff, &reset_manager_base->per2_mod_reset);
	writel(0xffffffff, RSTMGR_PER2MODRESET);


	/* Put FPGA bridges into reset too. */
	socfpga_bridges_reset(1);				// Essai FDy - pas necessaire tant que l'on a pas de FPGA ?

	//socfpga_per_reset(RSTMGR_SDR 			/*0x11D*/,	0);	// to reduce (decode & inline)...
	//socfpga_per_reset(RSTMGR_UART0		/*0x110*/,	0);	// to reduce (decode & inline)...
	//socfpga_per_reset(RSTMGR_OSC1TIMER0	/*0x108*/,	0);	// to reduce (decode & inline)...
	
	clrbits_le32(RSTMGR_PERMODRESET, 1 << 29);		// sdr=b29
	clrbits_le32(RSTMGR_PERMODRESET, 1 << 16);		// uart0=b16
	clrbits_le32(RSTMGR_PERMODRESET, 1 << 8);		// osc1timer0=b8


	//timer_init();
	// timer init :
	#define TIMER_LOAD_VAL		0xFFFFFFFF
	//writel(TIMER_LOAD_VAL, &timer_base->load_val);
	writel(TIMER_LOAD_VAL, CONFIG_SYS_TIMERBASE + 0x00);
	//writel(TIMER_LOAD_VAL, &timer_base->curr_val);
	writel(TIMER_LOAD_VAL, CONFIG_SYS_TIMERBASE + 0x04);
	//writel(readl(&timer_base->ctrl) | 0x3, &timer_base->ctrl);
	// TimerControlReg = 0x03 - Interrupt Disabled + User-defined count mode + Timer Enable 
	writel(readl(CONFIG_SYS_TIMERBASE + 0x08) | 0x3, CONFIG_SYS_TIMERBASE + 0x08);
	// Je supose qu'avec une valeur de chargement de 0xFFFFFFFF
	// le timer est downcounter...
	timerValueHigh	=0;
	timerValueLow   =0;

	//debug("Reconfigure Clock Manager\n");
	/* reconfigure the PLLs */
	FDy_cm_basic_init(/*cm_default_cfg*/);	// to reduce...

	/* Enable bootrom to configure IOs. */
	//sysmgr_config_warmrstcfgio(1);
	//setbits_le32(&sysmgr_regs->romcodegrp_ctrl, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);
	setbits_le32(SOCFPGA_SYSMGR_ADDRESS + 0xC0, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);

	/* configure the IOCSR / IO buffer settings */
	if (FDy_scan_mgr_configure_iocsr())		// to reduce...
		while(1);

	//sysmgr_config_warmrstcfgio(0);
	//clrbits_le32(&sysmgr_regs->romcodegrp_ctrl, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);
	clrbits_le32(SOCFPGA_SYSMGR_ADDRESS + 0xC0, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);

	/* configure the pin muxing through system manager */
	//sysmgr_config_warmrstcfgio(1);
	//setbits_le32(&sysmgr_regs->romcodegrp_ctrl, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);
	setbits_le32(SOCFPGA_SYSMGR_ADDRESS + 0xC0, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);

	FDy_sysmgr_pinmux_init();	// to reduce...

	//sysmgr_config_warmrstcfgio(0);
	//clrbits_le32(&sysmgr_regs->romcodegrp_ctrl, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);
	clrbits_le32(SOCFPGA_SYSMGR_ADDRESS + 0xC0, SYSMGR_ROMCODEGRP_CTRL_WARMRSTCFGIO);


	/* De-assert reset for peripherals and bridges based on handoff */
	//reset_deassert_peripherals_handoff();
	writel(0, SOCFPGA_RSTMGR_ADDRESS + 0x14);

	socfpga_bridges_reset(0);		// Essai FDy - pas necessaire tant que l'on a pas de FPGA ?


	/* unfreeze / thaw all IO banks */
	FDy_sys_mgr_frzctrl_thaw_req();	// freeze_controller.c

	/* enable console uart printing */
	FDy_preloader_console_init();

	/* Check signal from FPGA. */
	if (!fpgamgr_test_fpga_ready()) 
	{
		/* FPGA not ready, do nothing. */
		FDy_puts("FPGA not ready.\n\r");
	}

	FDy_puts("SDRAM init.\n\r");
	if (sdram_mmr_init_full(0xffffffff) != 0) 
	{
	 	FDy_puts("SDRAM init failed.\n");
	 	while(1);
	}

	FDy_puts("SDRAM: Calibrating PHY.\n\r");
	/* SDRAM calibration */
	if (sdram_calibration_full() == 0) 
	{
	 	FDy_puts("SDRAM calibration failed.\n");
	 	while(1);
	}

	sdram_size = sdram_calculate_size();
	FDy_puts("SDRAM: ");
	FDy_putHexa32(sdram_size >> 20);
	FDy_puts(" MiB - ");
	FDy_putHexa32(sdram_size);
	FDy_puts(" Bytes\n\r");
	
	/* Sanity check ensure correct SDRAM size specified */
	FDy_puts("Sanity check ensure correct SDRAM size specified.\r\n");
	if (get_ram_size(0, sdram_size) != sdram_size) 
	{
	 	FDy_puts("SDRAM size check failed!\n");
	 	while(1);
	}

	socfpga_bridges_reset(1);

	// wake up & shut up WatchDog 
	//socfpga_per_reset(SOCFPGA_RESET(L4WD0), 1);
	//socfpga_per_reset(SOCFPGA_RESET(L4WD0), 0);
	setbits_le32(RSTMGR_PERMODRESET, 1 << 6);		// L4WD0=b6
	clrbits_le32(RSTMGR_PERMODRESET, 1 << 6);		// L4WD0=b6

	
	// mmc init
	//mmc_clksel();
	//mmc_init();

	// mmc example
   	// Setting up SD/MMC
    if(status == ALT_E_SUCCESS)
    {
        FDy_puts("INFO: Setting up SDMMC.\n\r");
        status = alt_sdmmc_init();
    }

	    if(status == ALT_E_SUCCESS)
    {
        status = alt_sdmmc_card_pwr_on();
    }

    if(status == ALT_E_SUCCESS)
    {
        status = alt_sdmmc_card_identify(&Card_Info);
    }

    if(status == ALT_E_SUCCESS)
    {
        switch(Card_Info.card_type)
        {
        case ALT_SDMMC_CARD_TYPE_MMC:
            FDy_puts("INFO: MMC Card detected.\n\r");
            break;
        case ALT_SDMMC_CARD_TYPE_SD:
            FDy_puts("INFO: SD Card detected.\n\r");
            break;
        case ALT_SDMMC_CARD_TYPE_SDIOIO:
            FDy_puts("INFO: SDIO Card detected.\n\r");
            break;
        case ALT_SDMMC_CARD_TYPE_SDIOCOMBO:
            FDy_puts("INFO: SDIOCOMBO Card detected.\n\r");
            break;
        case ALT_SDMMC_CARD_TYPE_SDHC:
            FDy_puts("INFO: SDHC Card detected.\n\r");
            break;
        default:
            FDy_puts("INFO: Card type unknown.\n\r");
            status = ALT_E_ERROR;
            break;
        }
    }

    if(status == ALT_E_SUCCESS)
    {
        status = alt_sdmmc_card_bus_width_set(&Card_Info, ALT_SDMMC_BUS_WIDTH_4);
    }

    if(status == ALT_E_SUCCESS)
    {
        alt_sdmmc_fifo_param_set((ALT_SDMMC_FIFO_NUM_ENTRIES >> 3) - 1,
            (ALT_SDMMC_FIFO_NUM_ENTRIES >> 3), ALT_SDMMC_MULT_TRANS_TXMSIZE1);
    }

    if (status == ALT_E_SUCCESS)
    {
        alt_sdmmc_card_misc_get(&card_misc_cfg);
    }

    if(status == ALT_E_SUCCESS)
    {
        FDy_puts("INFO: Card width      = ");
		FDy_putHexa32(card_misc_cfg.card_width);
        FDy_puts("\n\r");
	
		FDy_puts("INFO: Card block size = ");
        FDy_putHexa32((int)card_misc_cfg.block_size);
        FDy_puts("\n\r");
	
		Sdmmc_Block_Size = card_misc_cfg.block_size;
        Sdmmc_Device_Size = ((uint64_t)Card_Info.blk_number_high << 32) + Card_Info.blk_number_low;
        Sdmmc_Device_Size *= Card_Info.max_r_blkln;
        FDy_puts("INFO: Card size       = ");
		FDy_putHexa64(Sdmmc_Device_Size);
		FDy_puts("\n\r");
	
    }

    if(status == ALT_E_SUCCESS)
    {
		// read was ok but was slow
		// faster when you set speed to 25MHz
       	status = alt_sdmmc_card_speed_set(&Card_Info, 25000000);
    }
	
    if(status == ALT_E_SUCCESS)
    {
       status = alt_sdmmc_dma_enable();
    }



	//ReadAndDisplaySector(0);	// sector 0 is MBR

	/*
	ReadAndDisplaySector(2048);	// sector 2048 is start of partition A2 (for our sdcard)
	ReadAndDisplaySector(2049);	
	ReadAndDisplaySector(2050);	
	ReadAndDisplaySector(2051);	
	*/

	/*	
	// dump complete SBL
	FDy_puts("INFO: Reading from address ");
	FDy_putHexa32(2048*512);
	FDy_puts(", size = 512 bytes using block I/O.\n\r");

	Read_Buffer = 0x1000;
	memset(Read_Buffer, 0, 0x00010000);
	status = alt_sdmmc_read(&Card_Info, Read_Buffer, (void*)(2048*512), 0x00010000);
	if(status ==  ALT_E_SUCCESS)
	{
		FDy_puts("Read is Ok\n\r");
		HexDump(Read_Buffer, 0x00010000);
	}
	else
	{
		FDy_puts("Read is Ko\n\r");
	}
	*/

	
	// load menu.rbf 
	address = MENU_RBF_ADDRESS;
	FDy_puts("INFO: Reading menu.rbf\n\r");
	FDy_puts("INFO: Reading from address ");
	FDy_putHexa32(address);
	FDy_puts(", size = ");
	FDy_putHexa32(MENU_RBF_SIZE_ROUND);
	FDy_puts(" bytes\n\r");

	// this block is ok but was slow (as the others)
	// faster when you set speed to 25MHz
	uiDestAddress = 0x1000;

	uint64_t ui64InitialValue =	get_ticks();
	status = alt_sdmmc_read(&Card_Info, uiDestAddress, (void*)MENU_RBF_ADDRESS, MENU_RBF_SIZE_ROUND);
	if(status !=  ALT_E_SUCCESS)
	{
		FDy_puts("Read is Ko\n\r");
	}
	/*
	HexDump(0x1000, 	0x0400);	// debut de fichier rbf
	FDy_puts("Next bit :\n\r");
	HexDump(0x0024A000, 0x1000);
	*/

	uint64_t ui64EndValue =	get_ticks();

	FDy_puts("menu.rbf finished to load in ");
	//FDy_putHexa64(ui64InitialValue-ui64EndValue);		// decreasing counter
	printi(	(ui64InitialValue-ui64EndValue)		/*i*/, 	// number of ticks
		   	10			/*b*/, 
		   	0			/*sg*/, 
		   	0 			/*width*/, 
		   	PAD_RIGHT	/*pad*/, 
		   	'A' 		/*letbase*/);
	FDy_puts(" ticks -> ");
	//FDy_putHexa32((int)(ui64InitialValue-ui64EndValue)*20);
	//FDy_puts(" ns\n\r");
	/*
        i:       value to print
        b:       base (10 or 16 usually)
        sg:      signed (1) or unsigned (0)
        width:
        pad:     Set of flags for padding:
                 pad with 0's or spaces (1) or no (0)
                  PAD_RIGHT - add spaces
                  PAD_ZERO - add 0's
                  PAD_0x - prepend with 0x
                  PAD_PLUS - add + to positive numbers
        letbase: letter base ('a' or 'A') for hex
	*/
	
	printi(	(ui64InitialValue-ui64EndValue)*20 / 1000000	/*i*/, 	// from ns to ms
		   	10			/*b*/, 
		   	0			/*sg*/, 
		   	0 			/*width*/, 
		   	PAD_RIGHT	/*pad*/, 
		   	'A' 		/*letbase*/);
	FDy_puts(" ms\n\r");
	
	socfpga_load((const void *)uiDestAddress, MENU_RBF_SIZE);
	
	
//	// load HDMI_TX.rbf 
//	address = HDMI_TX_RBF_ADDRESS;
//	FDy_puts("INFO: Reading HDMI_TX.rbf\n\r");
//	FDy_puts("INFO: Reading from address ");
//	FDy_putHexa32(address);
//	FDy_puts(", size = ");
//	FDy_putHexa32(HDMI_TX_RBF_SIZE_ROUND);
//	FDy_puts(" bytes\n\r");
//
//	// this block is ok but was slow (as the others)
//	// faster when you set speed to 25MHz
//	uiDestAddress = 0x1000;
//
//	uint64_t ui64InitialValue =	get_ticks();
//	status = alt_sdmmc_read(&Card_Info, uiDestAddress, (void*)HDMI_TX_RBF_ADDRESS, HDMI_TX_RBF_SIZE_ROUND);
//	if(status !=  ALT_E_SUCCESS)
//	{
//		FDy_puts("Read is Ko\n\r");
//	}
//	/*
//	HexDump(0x1000, 	0x0400);	// debut de fichier rbf
//	FDy_puts("Next bit :\n\r");
//	HexDump(0x0024A000, 0x1000);
//	*/
//
//	uint64_t ui64EndValue =	get_ticks();
//
//	FDy_puts("HDMI_TX.rbf finished to load in ");
//	FDy_putHexa64(ui64InitialValue-ui64EndValue);		// decreasing counter
//	FDy_puts(" ticks -> ");
//	FDy_putHexa32((int)(ui64InitialValue-ui64EndValue)*20);
//	FDy_puts(" ns\n\r");
//
//	socfpga_load((const void *)uiDestAddress, HDMI_TX_RBF_SIZE);
//

	mainLoop();	// Essai FDy	- it works here
}

void hang(void)
{
	FDy_puts("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
