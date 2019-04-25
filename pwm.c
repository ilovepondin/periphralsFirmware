//======================================================================================================================================================
//                                                PWM output example
//Output PWM pulse: pa8
//Function description: Setting PWM clock(sysclk/div = 1mhz), COUNTER(preload data: 500), and PWM DUTY(90%). Then output PWM pulse(500us per PWM cycle).
//=======================================================================================================================================================
#define   DUTY_PERCENT                 90               //number:0~100%
#define   COUNTER                     500               //cycle interval unit:us
#define   DUTY_REG_SHIFT               16               //IO0_Duty register bit
#define   PERCENTAGE                  100               //%


int main(void)
{ 
    //PWM clock divider: 32
    //PWM clock = sysclk / divider = 32mhz / 32 = 1mhz
    //COUNTER: down count cycle numbers
    //DUTY(IO0_Duty: duty cycles) = COUNTER(cycles) * (duty(0~100) / 100)) = 500 * 90 / 100 = 450(sysclk time)
    //Auto reload enable: when COUNTER down count to 0, reloads COUNTER and jumps to EXT interruption(per 500us)

    P_SMU->FUNC_Ctrl |=  C_Func_PWM_En | C_Func_GPIO_En;
    P_PORTA->MFP |= C_MFP_PWMA0_En;

    P_PWMA->TMR_Ctrl = (C_PWM_CLK_Div_32 | C_TMR_Auto_Reload_En);
    P_PWMA->TMR_Ctrl |= C_TMR_CLK_Src_HiCLK;


    P_PWMA->TMR_INT_Ctrl = C_TMR_En;              //PWM interruption enabled
    INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);             //CPU interruption enabled

    P_PWMA->TMR_Data = COUNTER;                   //preload data for down count
    P_PWMA->IO0_Duty = ((COUNTER * DUTY_PERCENT / PERCENTAGE)<<DUTY_REG_SHIFT) | C_PWM_Duty_Pol_Low;    //setting duty and pulse start low
    P_PWMA->IO0_Duty |= C_PWM_OUT_En;
    P_PWMA->TMR_Ctrl |= C_TMR_En;


    while(1)
    {
        P_WDT->CLR = C_WDT_CLR_Data;
    }

}

