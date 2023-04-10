/******************************************************************************
*
* Copyright 2014 Altera Corporation. All Rights Reserved.
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
* 3. The name of the author may not be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED. IN NO
* EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "alt_interrupt.h"
#include "alt_globaltmr.h"
#include "alt_timers.h"
#include "alt_clock_manager.h"
#include "alt_watchdog.h"
#include "socal/hps.h"

#define TIMER_LOAD_VALUE 32000

ALT_STATUS_CODE system_init(void);
ALT_STATUS_CODE system_uninit(void);
ALT_STATUS_CODE timer0_freerun_demo(void);
ALT_STATUS_CODE timer1_oneshot_demo(void);
ALT_STATUS_CODE global_timer_demo(void);
ALT_STATUS_CODE watch_dog_demo(void);

// Initialize interrupt subsystem and setup timer interrupt
static ALT_STATUS_CODE soc_int_setup();

// Cleanup interrupt subsystem and the specific timer interrupt
static ALT_STATUS_CODE soc_int_cleanup();

// ISR callback for the General Purpose Timer
static void glbl_timer_isr_callback();

// ISR callback for the General Purpose Timer
static void gp_timer_isr_callback();

// ISR callback for the Watchdog0
static void wdog0_isr_callback();

volatile bool Global_Timer_Interrupt_Fired = false;
volatile bool GP_Timer_Interrupt_Fired = false;
volatile bool WDOG0_Interrupt_Fired = false;
uint64_t cntr_value[10];
uint32_t intr_cnt = 0;

/******************************************************************************/
/*!
 * Initialize system
 *
 * \return      result of the function
 */
ALT_STATUS_CODE system_init(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    printf("\n");
    printf("INFO: System Initialization.\n");

    // Initialize global timer
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting up Global Timer.\n");
        if(!alt_globaltmr_int_is_enabled())
        {
            status = alt_globaltmr_init();
        }
    }

    // Initialize general purpose timers
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Initializing General Purpose Timers.\n");
        status = alt_gpt_all_tmr_init();
    }

    // Initialize watchdog0 timer
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Initializing Watchdog 0 Timer.\n");
        status = alt_wdog_init();
    }

    return status;
}

/******************************************************************************/
/*!
 * Initialize interrupt subsystem and setup Timer interrupts.
 *
 * \return      result of the function
 */
static ALT_STATUS_CODE soc_int_setup()
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    int cpu_target = 0x1; //CPU0 will handle the interrupts

    printf("\n");
    printf("INFO: Interrupt Setup.\n");

    // Initialize global interrupts
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_global_init();
    }

    // Initialize CPU interrupts
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_cpu_init();
    }

    // Set interrupt trigger type
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_trigger_set(ALT_INT_INTERRUPT_PPI_TIMER_GLOBAL, ALT_INT_TRIGGER_AUTODETECT);
    }

    // Enable interrupt at the distributor level
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_enable(ALT_INT_INTERRUPT_PPI_TIMER_GLOBAL);
    }

    // Set interrupt distributor target
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_target_set(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, cpu_target);
    }

    // Set interrupt trigger type
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_trigger_set(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, ALT_INT_TRIGGER_AUTODETECT);
    }


    // Enable interrupt at the distributor level
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_enable(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ);
    }

    // Set interrupt distributor target
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_target_set(ALT_INT_INTERRUPT_WDOG0_IRQ, cpu_target);
    }

    // Set interrupt trigger type
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_trigger_set(ALT_INT_INTERRUPT_WDOG0_IRQ, ALT_INT_TRIGGER_AUTODETECT);
    }

    // Enable interrupt at the distributor level
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_enable(ALT_INT_INTERRUPT_WDOG0_IRQ);
    }

    // Enable CPU interrupts
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_cpu_enable();
    }

    // Enable global interrupts
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_global_enable();
    }

    return status;
}

/******************************************************************************/
/*!
 * Global Timer Module ISR Callback
 *
 * \param       icciar
 * \param       context ISR context.
 * \return      none
 */

static void glbl_timer_isr_callback()
{

    // Clear int source don't care about the return value
    alt_globaltmr_int_clear_pending();
    cntr_value[intr_cnt] = alt_globaltmr_get64();
    intr_cnt++;

    // Notify main thread after 10 interrupts
    if (intr_cnt == 10)
    {
        Global_Timer_Interrupt_Fired = true;
    }
}

/******************************************************************************/
/*!
 * Timer Module ISR Callback
 *
 * \param       icciar
 * \param       context ISR context.
 * \return      none
 */

static void gp_timer_isr_callback()
{

    // Clear interrupt source don't care about the return value
    alt_gpt_int_clear_pending(ALT_GPT_SP_TMR1);

    // Notify main thread
    GP_Timer_Interrupt_Fired = true;
}

/******************************************************************************/
/*!
 * Watchdog Module ISR Callback
 *
 * \param       icciar
 * \param       context ISR context.
 * \return      none
 */

static void wdog0_isr_callback()
{

    // Clear interrupt source don't care about the return value
    alt_wdog_int_clear(ALT_WDOG0);

    // Notify main thread
    WDOG0_Interrupt_Fired = true;
}

/******************************************************************************/
/*!
 * Set OSC0 Timer 0 to free-running.
 *
 * \return      result of the function
 */
ALT_STATUS_CODE timer0_freerun_demo(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    uint32_t cnt_value = 65536;
    uint32_t timer_val;


    printf("\n");
    printf("INFO: OSC 1 Timer 0 Free-run Demo.\n");

    // Set OSC1 Timer 0 to free running mode
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting OSC1 Timer0 mode.\n");
        status = alt_gpt_mode_set(ALT_GPT_OSC1_TMR0, ALT_GPT_RESTART_MODE_PERIODIC);
    }

    // Set OSC1 Timer 0 counter reset value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting OSC1 Timer0 restart count value.\n");
        status = alt_gpt_counter_set(ALT_GPT_OSC1_TMR0, cnt_value);
    }

    // Set OSC1 Timer 0 to start running
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Starting OSC1 Timer0.\n");
        status = alt_gpt_tmr_start(ALT_GPT_OSC1_TMR0);
    }

    // Check if OSC1 Timer 0 is running
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Checking if OSC1 Timer0 is running.\n");
        status = alt_gpt_tmr_is_running(ALT_GPT_OSC1_TMR0);
        if(status == ALT_E_TRUE)
        {
            printf("INFO: OSC1 Timer0 is running.\n");
            status = ALT_E_SUCCESS;
        }
        else
        {
            printf("FAIL: OSC1 Timer0 is not running.\n");
            status = ALT_E_ERROR;
        }
    }

    // Get Parameters of Osc1 Timer 0
    if(status == ALT_E_SUCCESS)
    {
        timer_val = alt_gpt_freq_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 running at %" PRIu32 "Hz.\n",timer_val);

        timer_val = alt_gpt_time_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Period in seconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_time_millisecs_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Period in milliseconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_time_microsecs_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Period in microseconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_curtime_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Time to zero in seconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_curtime_millisecs_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Time to zero in milliseconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_curtime_microsecs_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Time to zero in microseconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_maxtime_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Max time in seconds is %" PRIu32 ".\n",timer_val);

        timer_val = alt_gpt_maxtime_millisecs_get(ALT_GPT_OSC1_TMR0);
        printf("INFO: OSC1 Timer0 Max time in milliseconds is %" PRIu32 ".\n",timer_val);
    }

    return status;
}

/******************************************************************************/
/*!
 * Set SP Timer 1 to one-shot. (Clocked by l4_sp_clk)
 *
 * \return      result of the function
 */
ALT_STATUS_CODE timer1_oneshot_demo(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    printf("\n");
    printf("INFO: SP Timer 1 One-shot Demo.\n");

    // Set SP Timer 1 to one shot
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting SP Timer 1 mode.\n");
        status = alt_gpt_mode_set(ALT_GPT_SP_TMR1, ALT_GPT_RESTART_MODE_ONESHOT);
    }

    // Load SP Timer 1 value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting SP Timer 1 count value.\n");
        status = alt_gpt_counter_set(ALT_GPT_SP_TMR1, 1024);
    }

    // Register timer ISR
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_register(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ, gp_timer_isr_callback, NULL);
    }

    // Enable interrupt SP Timer 1
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Enabling SP Timer1 Interrupt.\n");
        status = alt_gpt_int_enable(ALT_GPT_SP_TMR1);
    }

    // Set SP Timer 1 to start running
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Starting SP Timer1.\n");
        status = alt_gpt_tmr_start(ALT_GPT_SP_TMR1);
    }

    // Set SP Timer 1 to start running (this routine returns true, false or bad arg)
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Checking if SP Timer 1 is running.\n");
        status = alt_gpt_tmr_is_running(ALT_GPT_SP_TMR1);
        if(alt_gpt_tmr_is_running(ALT_GPT_SP_TMR1) == ALT_E_TRUE)
        {
            status = ALT_E_SUCCESS;
            printf("INFO: SP Timer 1 is running.\n");
        }
        else
        {
            status = ALT_E_ERROR;
            printf("FAIL: SP Timer 1 is not running.\n");
        }
    }

    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Waiting for SP Timer1 Interrupt.\n");
        while (!GP_Timer_Interrupt_Fired)
        {
        }

        status = alt_gpt_int_disable(ALT_GPT_SP_TMR1);
        printf("INFO: SP Timer1 Interrupt Fired.\n");
    }

    return status;
}

/******************************************************************************/
/*!
 * Demo the Global Timer Functions
 *
 * \return      result of the function
 */
ALT_STATUS_CODE global_timer_demo(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    uint64_t overhead, cntr_value_diff;

    printf("\n");
    printf("INFO: Global Timer Demo.\n");

    // Start Global Timer
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Starting Global Timer.\n");
        status = alt_globaltmr_start();
    }

    // Register Global Timer ISR
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_register(ALT_INT_INTERRUPT_PPI_TIMER_GLOBAL, glbl_timer_isr_callback, NULL);
    }

    // Get Global counter value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Measuring for loop time.\n");
        cntr_value[1] = alt_globaltmr_get64();
        cntr_value[2] = alt_globaltmr_get64();
        overhead = cntr_value[2] - cntr_value[1];

        cntr_value[1] = alt_globaltmr_get64();
        for (volatile int cntr = 1; cntr <= 511; cntr ++ )
        {
        }
        cntr_value[2] = alt_globaltmr_get64();
        cntr_value_diff = cntr_value[2] - cntr_value[1] - overhead;
        printf("INFO: For loop time elapsed = %" PRIu64 ".\n", cntr_value_diff);
    }

    // Set Global Timer autoinc value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting Global Timer Comparator.\n");
        cntr_value[1] = alt_globaltmr_get64();
        status = alt_globaltmr_comp_set64(cntr_value[1] + TIMER_LOAD_VALUE);
    }

    // Set autoinc value
    if(status == ALT_E_SUCCESS)
    {
        status = alt_globaltmr_autoinc_set(TIMER_LOAD_VALUE);
    }

    // Set Global Timer to autoinc mode
    if(status == ALT_E_SUCCESS)
    {
        status = alt_globaltmr_autoinc_mode_start();
    }

    if(status == ALT_E_SUCCESS)
    {
        status = alt_globaltmr_comp_mode_start();
    }

    // Turn on Global Timer Interrupt
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Enabling Global Timer Interrupts.\n");
        status = alt_globaltmr_int_enable();
    }

    if(status == ALT_E_SUCCESS)
    {
        while (!Global_Timer_Interrupt_Fired)
        {
        }

        status = alt_globaltmr_int_disable();
        // Display the deltas between the interrupts
        printf("INFO: Global Timer Interrupt Deltas.\n");
        for (int i = 1; i <10; i++)
        {
            printf("INFO: Global Timer Comparator Interrupt Delta %i = %" PRIu64".\n", i, cntr_value[i] - cntr_value[i-1]);
        }
    }

    return status;
}

/******************************************************************************/
/*!
 * Watchdog Demo
 *
 * \return      result of the function
 */
ALT_STATUS_CODE watch_dog_demo(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    uint32_t timer_val = 0, new_timer_val = 0;

    printf("\n");
    printf("INFO: CPU0 Watch Dog Demo.\n");

    // Set Watchdog 0 to interrupt then reset mode
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting CPU0 Watchdog mode to Interrupt then Reset.\n");
        status = alt_wdog_response_mode_set(ALT_WDOG0, ALT_WDOG_INT_THEN_RESET);
    }

    // Set Watchdog 0 Init value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting CPU0 Watchdog Counter Initial Value.\n");
        status = alt_wdog_counter_set(ALT_WDOG0_INIT, 0);
    }

    // Set Watchdog 0 counter value
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Setting CPU0 Watchdog Counter Reset Value.\n");
        status = alt_wdog_counter_set(ALT_WDOG0, 1);
    }

    // Register wdog ISR
    if (status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_register(ALT_INT_INTERRUPT_WDOG0_IRQ, wdog0_isr_callback, NULL);
    }

    // Start Watchdog 0
    if (status == ALT_E_SUCCESS)
    {
        printf("INFO: Starting Watchdog 0 Timer.\n");
        status = alt_wdog_start(ALT_WDOG0);
    }

    // Check Watchdog 0 running
    if (status == ALT_E_SUCCESS)
    {
        if (alt_wdog_tmr_is_enabled(ALT_WDOG0) == true)
        {
            printf("INFO: Watchdog 0 Timer is running.\n");
        }
        else
        {
            printf("INFO: Watchdog 0 Timer is not running.\n");
        }
    }

    // Get Parameters of Wdog
    if(status == ALT_E_SUCCESS)
    {
        timer_val = alt_wdog_counter_get_inittime_millisecs(ALT_WDOG0);
        printf("INFO: Watchdog 0 Initial time = %" PRIu32 " millisecs.\n",timer_val);

        timer_val = alt_wdog_counter_get_curtime_millisecs(ALT_WDOG0);
        printf("INFO: Watchdog 0 Current time = %" PRIu32 " millisecs.\n",timer_val);

        // Lets kill some time
        for (volatile int cntr = 1; cntr <= 127; cntr ++ )
        {
        }

        // Get the counter value
        timer_val = alt_wdog_counter_get_current(ALT_WDOG0);
        printf("INFO: Watchdog 0 count = %" PRIu32 ".\n",timer_val);

        // Pet the dog
        alt_wdog_reset(ALT_WDOG0);

        // Get the new counter value should be greater than last time
        new_timer_val = alt_wdog_counter_get_current(ALT_WDOG0);
        printf("INFO: Watchdog 0 count after reset= %" PRIu32 ".\n",new_timer_val);

        if(new_timer_val < timer_val)
        {
            printf("INFO: Watchdog 0 didn't reset.\n");
            status = ALT_E_ERROR;
        }
        else
        {
            printf("INFO: Watchdog 0 Reset.\n");
        }

    }

    // Wait for watch dog to interrupt
    if(status == ALT_E_SUCCESS)
    {
        printf("INFO: Waiting for Watchdog 0 Interrupt.\n");
        while (!WDOG0_Interrupt_Fired)
        {
        }

        printf("INFO: Watchdog 0 Interrupt Fired.\n");

    }

    // Stop Watchdog 0
    if (status == ALT_E_SUCCESS)
    {
        printf("INFO: Stopping Watchdog 0 Timer.\n");
        status = alt_wdog_stop(ALT_WDOG0);
    }

    return status;
}

/******************************************************************************/
/*!
 * Cleanup interrupt subsystem and the specific Timer interrupts.
 *
 * \return      result of the function
 */
static ALT_STATUS_CODE soc_int_cleanup()
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    printf("\n");
    printf("INFO: Cleaning up Timer interrupts.\n");

    // Disable global interrupts
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_global_disable();
    }

    // Disable CPU interrupts
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_cpu_disable();
    }

    // Disable Global Timer interrupt at the distributor level
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_disable(ALT_INT_INTERRUPT_PPI_TIMER_GLOBAL);
    }

    // Unregister Global Timer ISR
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_unregister(ALT_INT_INTERRUPT_PPI_TIMER_GLOBAL);
    }

    // Disable Timer interrupt at the distributor level
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_disable(ALT_INT_INTERRUPT_TIMER_OSC1_1_IRQ);
    }

    // Unregister Timer ISR
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_unregister(ALT_INT_INTERRUPT_TIMER_OSC1_1_IRQ);
    }

    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_dist_disable(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ);
    }

    // Unregister Global Timer ISR
    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_isr_unregister(ALT_INT_INTERRUPT_TIMER_L4SP_1_IRQ);
    }

    return status;
}
/******************************************************************************/
/*!
 * Un-init system
 *
 * \return      result of the function
 */
ALT_STATUS_CODE system_uninit(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    printf("\n");
    printf("INFO: Shutting Down System.\n");

    if(status == ALT_E_SUCCESS)
    {
        status = alt_int_cpu_uninit();
    }

    if(status == ALT_E_SUCCESS)
    {
        status =  alt_int_global_uninit();
    }

    if(status == ALT_E_SUCCESS)
    {
        status =  alt_gpt_all_tmr_uninit();
    }

    if(status == ALT_E_SUCCESS)
    {
        status = alt_wdog_uninit();
    }

    return status;
}

/******************************************************************************/
/*!
 * Main entry point
 *
 */
int main(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;

    // System init
    if(status == ALT_E_SUCCESS)
    {
        status = system_init();
    }

    // Setup Interrupt
    if(status == ALT_E_SUCCESS)
    {
        status = soc_int_setup();
    }

    // Timer0 set to free running
    if(status == ALT_E_SUCCESS)
    {
        status = timer0_freerun_demo();
    }

    // Timer1 set to one shot
    if(status == ALT_E_SUCCESS)
    {
        status = timer1_oneshot_demo();
    }

    // Use Global timer to measure code snippet
    if(status == ALT_E_SUCCESS)
    {
        status = global_timer_demo();
    }

    // Demo CPU0 Watchdog
    if(status == ALT_E_SUCCESS)
    {
        status = watch_dog_demo();
    }

    // Cleanup Interrupt
    if(status == ALT_E_SUCCESS)
    {
        status = soc_int_cleanup();
    }

    // System uninit
    if(status == ALT_E_SUCCESS)
    {
        status = system_uninit();
    }

    // Report status
    if (status == ALT_E_SUCCESS)
    {
        printf("\nRESULT: All tests successful.\n");
        return 0;
    }
    else
    {
        printf("\nRESULT: Some failures detected.\n");
        return 1;
    }
}

