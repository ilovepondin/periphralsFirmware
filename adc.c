/*============================================================*
*                 ADC voltage input example                   *
*             GPIO MFP bit[20]:PA4 input voltage              *
*                                                             *
* ============================================================*/

#define   CH4        DISABLE
#define   CH0        ENABLE

int main(void)
{ 
    //delete
    SYS_Init();
    //  ADC clock = system clock / divider. 32MHZ sysclk / 16 divider = 2MHZ ADC clock |
    //  sample time, part of coversion time
    //  Pre-Charge Sample&Hold Capacitor to 1/2 VDD
    //  ADC Convertion Trigger by Manual Mode (Software Trigger)
    //  Line in Channel Select
    //  ADC Trigger Source (Manual)
    //  ADC Convertion Trigger by Manual Mode (Software Trigger), Write Only
    P_SMU->FUNC_Ctrl |= C_Func_ADC_En | C_Func_GPIO_En;

    P_PORTA->MFP |= C_MFP_ADC_CH4_En;                         //GPIO multifunction enables ADC channel 4
    P_PORTA->MFP |= C_MFP_ADC_CH0_En;                         //GPIO multifunction enables ADC channel 5
#if CH4
    P_ADC->Ctrl = C_ADC_CLK_Div_16 |
            C_ADC_Sample_Cycle_16 |
            C_ADC_PreCharge_En |
            C_ADC_CH4 |
            C_ADC_Trig_Manual |
            C_ADC_En;
#elif CH0
    P_ADC->Ctrl = C_ADC_CLK_Div_16 |
            C_ADC_Sample_Cycle_16 |
            C_ADC_PreCharge_En |
            C_ADC_CH0 |
            C_ADC_Trig_Manual |
            C_ADC_En;
#endif
    P_ADC->FIFO_Ctrl = C_ADC_Data_Store_CH_Data;               //Data store to P_ADC_Data_CH4 (ADC Channel 4 Register)



    while(1)
    {
        //delete
        Delayms(500);
        P_ADC->Ctrl |= C_ADC_Manual_Start;
#if CH4
        dprintf("ADC CH4 Voltage = %d\n\r", P_ADC->Data_CH4);
#elif CH0
        dprintf("ADC CH0 Voltage = %d\n\r", P_ADC->Data_CH0);
#endif
        P_WDT->CLR = C_WDT_CLR_Data;

    }

}

