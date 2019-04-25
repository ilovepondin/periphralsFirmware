#include "timer.h"

//===============================================================================
// TIMER
// TMR clock: C_TMR_CLK_Src_HiCLK
// TMRCK=TMCK0/16
// TMR reload in overflow:C_TMR_Auto_Reload_En
// TMR jump to isr per 100clk cycles: PA1 changes high or low every times interruption occurs
// TMR INT enable
// C_TMR_En
//===============================================================================

void Timer_en()
{
    //SMU TMR enable
    P_SMU->FUNC_Ctrl |= C_Func_TMR_En;

    //TMR_clock:4 Timer Clock Source Select :C_TMR_CLK_Src_HiCLK, C_TMR_CLK_Src_LoCLK,
    //C_TMR_CLK_Src_ExtCLK_Async: Without System Clock Sampling, C_TMR_CLK_Src_ExtCLK_Sync: With System clock sampling
    //| Devider:timer counter = (target_period × timer_frequency ÷ timer_divide)-1 . divider: 1, 2, 4, 8, 16, 32, 64, 128 divider
    //| Auto_Reload_En:Timer Auto-reload in Underflow: enable or disable
    //| Timer enable

    P_TIMER0->Ctrl = C_TMR_CLK_Src_HiCLK
            | C_TMR_CLK_Div_16
            | C_TMR_Auto_Reload_En
            | C_TMR_En;


    //target_period: when < 0, reload or not reload depending on Line23 ctrl
    P_TIMER0->Data = 100-1;
    //Timer Interrupt Enable/Disable
    P_TIMER0->INT_Ctrl |= C_TMR_IEn;
    //andes cpu interrupt enable
    INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);

}

