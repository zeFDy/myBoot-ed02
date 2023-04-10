TARGET = myBoot.o

#CROSS_COMPILE = arm-linux-gnueabihf-
CROSS_COMPILE = "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.07/bin/arm-none-eabi-"
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld.bfd 
CP = $(CROSS_COMPILE)objcopy 
OD = $(CROSS_COMPILE)objdump
ARCH = arm

CCOPT = -mthumb -mthumb-interwork -mabi=aapcs-linux  -mno-unaligned-access -ffunction-sections -fdata-sections -fno-common -ffixed-r9  -msoft-float -march=armv7-a
LDSCRIPT := myBoot.lds

OPTIONS = -Os -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding -mthumb -mthumb-interwork -mabi=aapcs-linux -mword-relocations  -fno-pic  -mno-unaligned-access -mno-unaligned-access -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -msoft-float -march=armv7-a -P -c 


					
start.o:				start.S
						$(CC) -x assembler-with-cpp start.S -o start.o -c
						$(OD) -d start.o >start.lst
				
ClockManager.o:			ClockManager.c
						$(CC) $(OPTIONS) ClockManager.c -o ClockManager.o
						$(OD) -d ClockManager.o >ClockManager.lst
			
FreezeManager.o:		FreezeManager.c
						$(CC) $(OPTIONS) FreezeManager.c -o FreezeManager.o
						$(OD) -d FreezeManager.o >FreezeManager.lst
			
ScanManager.o:			ScanManager.c
						$(CC) $(OPTIONS) ScanManager.c -o ScanManager.o
						$(OD) -d ScanManager.o >ScanManager.lst

FpgaManager.o:			FpgaManager.c
						$(CC) $(OPTIONS) FpgaManager.c -o FpgaManager.o
						$(OD) -d FpgaManager.o >FpgaManager.lst
			
sdram.o:				sdram.c
						$(CC) $(OPTIONS) sdram.c -o sdram.o
						$(OD) -d sdram.o >sdram.lst

sequencer.o:			sequencer.c
						$(CC) $(OPTIONS) sequencer.c -o sequencer.o
						$(OD) -d sequencer.o >sequencer.lst

hwlibs/alt_sdmmc.o:		hwlibs/alt_sdmmc.c
						$(CC) $(OPTIONS) hwlibs/alt_sdmmc.c -o hwlibs/alt_sdmmc.o
						$(OD) -d hwlibs/alt_sdmmc.o >hwlibs/alt_sdmmc.lst

hwlibs/alt_clock_manager.o:	hwlibs/alt_clock_manager.c
							$(CC) $(OPTIONS) hwlibs/alt_clock_manager.c -o hwlibs/alt_clock_manager.o
							$(OD) -d hwlibs/alt_clock_manager.o >hwlibs/alt_clock_manager.lst

hwlibs/alt_globaltmr.o:	hwlibs/alt_globaltmr.c
						$(CC) $(OPTIONS) hwlibs/alt_globaltmr.c -o hwlibs/alt_globaltmr.o
						$(OD) -d hwlibs/alt_globaltmr.o >hwlibs/alt_globaltmr.lst

hwlibs/alt_interrupt.o:	hwlibs/alt_interrupt.c
						$(CC) $(OPTIONS) hwlibs/alt_interrupt.c -o hwlibs/alt_interrupt.o
						$(OD) -d hwlibs/alt_interrupt.o >hwlibs/alt_interrupt.lst

div64.o:				div64.c
						$(CC) $(OPTIONS) div64.c -o div64.o
						$(OD) -d div64.o >div64.lst

spl.o:					spl.c
						$(CC) $(OPTIONS) spl.c -o spl.o
						$(OD) -d spl.o >spl.lst


myBoot.o:		start.o \
				spl.o \
				ClockManager.o	\
				FpgaManager.o \
				FreezeManager.o \
				ScanManager.o \
				sdram.o \
				sequencer.o \
				hwlibs/alt_sdmmc.o \
				hwlibs/alt_clock_manager.o \
				hwlibs/alt_globaltmr.o \
				hwlibs/alt_interrupt.o	\
				div64.o
				$(LD) -T myBoot.lds --gc-sections -Bstatic --gc-sections -Ttext 0xFFFF0000 start.o --start-group \
				spl.o \
				ClockManager.o \
				FpgaManager.o \
				FreezeManager.o \
				ScanManager.o \
				sdram.o \
				sequencer.o \
				hwlibs/alt_sdmmc.o \
				hwlibs/alt_clock_manager.o \
				lib.a \
				memset.o \
				div64.o \
				--end-group -o myBoot.o
				$(CP) -O binary myBoot.o myBoot.bin
				$(OD) -d myBoot.o >myBoot.lst
  
