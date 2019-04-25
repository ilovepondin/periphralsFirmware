#include "ir.h"


/*============================================================*
*                    IR output example                        *
*                   GPIO output IR pulse: pa5                 *
* ============================================================*/

void ir_en()
{
    //SMU IR enable
    P_SMU->FUNC_Ctrl |= C_Func_IR_En;
    //GPIO IR enable
    P_PORTA->MFP |= C_MFP_IR_En;
    //IR idle level: C_IR_Idle_High or C_IR_Idle_Low
    P_IR->Ctrl |= C_IR_Idle_High;
    //IR Count Divider Clock Source from IR Clock: 32divider from 1~32
    P_IR->Ctrl |= C_IR_CNT_Div_2;
    //IR Clock Divider, Clock Source from System Clock: 16diveder from 1~16
    P_IR->Ctrl |= C_IR_CLK_Div_8;
    //IR state : C_IR_Run or C_IR_Idle
    P_IR->Ctrl |= C_IR_Run;


}





