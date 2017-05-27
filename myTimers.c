//***** Header Files **********************************************************
#include <driverlib.h>
#include "myTimers.h"

//***** Global Variables ******************************************************

//*****************************************************************************
// Initialize Timer
//*****************************************************************************
void initTimers(void)
{
    //*************************************************************************
    // 1. Setup Timer (TAR, TACTL)
    //    TimerA1 in Continuous mode using ACLK
    //    Toggle LED2 (Green) on/off every 2 seconds using timer interrupt (TA1IFG)
    //*************************************************************************
    Timer_A_initContinuousModeParam initContParam = { 0 };
        initContParam.clockSource =                 TIMER_A_CLOCKSOURCE_SMCLK;       // Use ACLK (slower clock)
        initContParam.clockSourceDivider =          TIMER_A_CLOCKSOURCE_DIVIDER_12;  // Input clock = ACLK / 1 = 10KHz
        initContParam.timerInterruptEnable_TAIE =   TIMER_A_TAIE_INTERRUPT_ENABLE;  // Enable TAR -> 0 interrupt
        initContParam.timerClear =                  TIMER_A_DO_CLEAR;               // Clear TAR & clock divider
        initContParam.startTimer =                  false;                          // Don't start the timer, yet
    Timer_A_initContinuousMode( TIMER_A1_BASE, &initContParam );

    //*************************************************************************
    // 2. Setup Capture & Compare features
    //*************************************************************************
    //    This example does not use these features

    //*************************************************************************
    // 3. Clear/enable flags and start timer
    //*************************************************************************
    Timer_A_clearTimerInterrupt( TIMER_A1_BASE );                                   // Clear TA1IFG

    //This enable was already done by the configureContinuousMode function
    //Timer_A_enableInterrupt( TIMER_A1_BASE );                                     // Enable interrupt on counter (TAR) rollover

    Timer_A_startCounter(
        TIMER_A1_BASE,
        TIMER_A_CONTINUOUS_MODE
    );
}


