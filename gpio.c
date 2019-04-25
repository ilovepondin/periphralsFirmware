/*==============================================================================================*
*        GPIO enable & GPIO external interrupt enable.                                          *
*        Input PA15 : GPIO external interrupt pin.                                              *
*        Output PA1 : Interrupt ISR output.                                                     *
*        Function description: input Voltage to PA15 turns state to ISR(isr.c line 124).        *
*                              GPIOA_ISR changes PA1 output everytimes interrupt.               *
* ==============================================================================================*/


#define GPIO_PORTA_PA1_DIR (1<<1)

int main(void)
{ 

    P_SMU->FUNC_Ctrl |= C_Func_GPIO_En;                 //SMU GPIO enable
    P_PORTA->DIR &= ~GPIO_PORTA_PA1_DIR;                //GPIO PORTA set output
    P_PORTA->MFP |= C_MFP_EXT_INT_En;                   //gpio multifunction enables external interrupt
    P_PORT_EXT->EXT_INT_Trig = C_EXT_INT_Trig_Rising;   //external interrupt triggered by rising edge
    P_PORT_EXT->EXT_INT_Ctrl = C_EXT_INT_IEn;           //GPIO Interruption enabled

    INTC_IrqCmd(IRQ_BIT_GPIOA, ENABLE);                 //cpu interrupt


    while(1)
    {

        P_WDT->CLR = C_WDT_CLR_Data;                        //watch dog clear
    }
}



