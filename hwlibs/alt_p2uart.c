/******************************************************************************
*
* Copyright 2013-2014 Altera Corporation. All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include <stdint.h>
#include "alt_16550_uart.h"
#include "alt_printf.h"
#include "socal/hps.h"
#include "socal/socal.h"
#include "socal/alt_uart.h"

void uart_putchar(FILEOP *port, char toprint);

typedef struct UART_INFO_s
{
  FILEOP f_op;
  int    initialized;
  ALT_16550_DEVICE_t uart_ID;
  ALT_16550_HANDLE_t mUart;
} UART_INFO_t;

static UART_INFO_t term0_st = { {uart_putchar}, 0, ALT_16550_DEVICE_SOCFPGA_UART0 };
static UART_INFO_t term1_st = { {uart_putchar}, 0, ALT_16550_DEVICE_SOCFPGA_UART1 };

FILE *term0 = (FILE *) &term0_st;
FILE *term1 = (FILE *) &term1_st;

#ifndef BAUD_RATE
#define BAUD_RATE      (115200)
#endif

static ALT_STATUS_CODE init_uart(UART_INFO_t *uartInfo)
{
    uint32_t uart_location = 0;
    ALT_STATUS_CODE status;

    status = alt_16550_init( uartInfo->uart_ID, 
                             (void *)&uart_location, 
                             ALT_CLK_L4_SP, 
                             &uartInfo->mUart);

    if (status == ALT_E_SUCCESS)
    {   
        status = alt_16550_baudrate_set(&uartInfo->mUart, BAUD_RATE); 
    }

    if (status == ALT_E_SUCCESS)
    {
        status = alt_16550_line_config_set(&uartInfo->mUart,  
                                             ALT_16550_DATABITS_8, 
                                             ALT_16550_PARITY_DISABLE, 
                                             ALT_16550_STOPBITS_1); 
    }
    
    if (status == ALT_E_SUCCESS)
    {
        status = alt_16550_fifo_enable(&uartInfo->mUart);
    }

    if (status == ALT_E_SUCCESS)
    {
        status = alt_16550_enable(&uartInfo->mUart);
    }
    return status;
}

void uart_putchar(FILEOP *port, char toprint)
{
  UART_INFO_t *p_uart = (UART_INFO_t *) port;

  if(p_uart->initialized == 0)
  {
    p_uart->initialized = 1;
    init_uart(p_uart);
  }

  if(toprint == '\n')
    uart_putchar(port, '\r');

  alt_16550_fifo_write_safe(&p_uart->mUart, &toprint, 1, 1); 
}

void alt_log_done(FILE *op)
{
    UART_INFO_t *p_uart = (UART_INFO_t *) op;
    uint32_t line_status;

    ALT_PRINTF("\n");

    /* Ensure that the TX FIFO is empty. */

    do
    {
        alt_16550_line_status_get(&p_uart->mUart, &line_status);
    } while ((line_status & ALT_16550_LINE_STATUS_TEMT) == 0);

    alt_clrbits_word(ALT_UART_IER_DLH_ADDR(p_uart->mUart.location), ALT_UART_IER_DLH_PTIME_DLH7_SET_MSK);

    /* Leave the UARTs enabled.
     * Many subsequent stages expect the UART to be already setup by preloader. */
}

