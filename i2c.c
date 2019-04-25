#include "i2c.h"


/*============================================================*
*               I2C TX/RX for EEPROM example                  *
*               I2C SCL pin: pa13                             *
*               I2C SDA pin: pa12                             *
* ============================================================*/



//////////////////////////////////////////////////////////////////////
//  Defines
//////////////////////////////////////////////////////////////////////
#define  I2C_WAIT_COMPLETE          0
#define  I2C_FINISH                 0
#define  I2C_ERR_ARB_LOSE           C_I2C_ARB_LOSE_Flag     //(1<<4)
#define  I2C_ERR_ACK                C_I2C_ACK               //(1<<10)
#define  I2C_ERR_NACK               (C_I2C_ACK | 0x10000)
#define  I2C_ERR_TIMEOUT            (1<<15)
#define  TIMEOUT_THRESHOLD          2000




//I2C initiate function
//PORTA pin12 for multifunction: sda & scl
//set Master address and address mask
    //set master mode
//set address mode 7bit
//set i2c setup time : Clock Setup Time(0~31), Standard Mode must be >= 250ns, Fast Mode must be >= 100ns, Plus Mode must be >= 50ns
//set hold time: Clock Hold Time(0~31), Standard Mode must be >= 300ns, Fast Mode must be >= 300ns
//Clock Low Duty (High/Low Ratio): Clock Low Duty 50%
//high clock time
//defines the pulse width of spikes that must be suppressed by the input filter. Pulse width = T_SP * tpclk

void I2C_Init_sample(void)
{

    P_SMU->FUNC_Ctrl |= C_Func_I2C_En | C_Func_GPIO_En;
    P_PORTA->MFP |= C_MFP_I2C_En;
    P_I2C->Addr  = _I2C_MASTER_ADDRESS&C_I2C_Addr;

    P_I2C->Ctrl  = C_I2C_Mode_Master;
    P_I2C->Ctrl |= C_I2C_Addr_Mode_7bit;
    P_I2C->Ctrl |= C_I2C_Fast_Setup_Time;
    P_I2C->Ctrl |= C_I2C_Fast_Hold_Time;
    P_I2C->Ctrl |= C_I2C_SCL_LowDuty_50;
    P_I2C->Ctrl |= C_I2C_STD_SCLHi_Time;
    P_I2C->Ctrl |= (C_I2C_Spike_Time<<C_I2C_Spike_Time_Offset);
    P_I2C->Ctrl |= C_I2C_En;

}

U32 WaitComplete(void)
{
    volatile U32 state = 0;
    U32 timeout = 0;

    while(1)
    {
        state = P_I2C->Status;
        if(state & C_I2C_CMPL_Flag)
        {
            P_I2C->Status = P_I2C->Status;
            return I2C_WAIT_COMPLETE;
        }
        else if(state & C_I2C_ARB_LOSE_Flag)
        {
            P_I2C->Status = P_I2C->Status;
            return I2C_ERR_ARB_LOSE;
        }
        else if(++timeout > TIMEOUT_THRESHOLD)
        {
            P_I2C->Status = P_I2C->Status;
            return I2C_ERR_TIMEOUT;
        }
    }
}



U32 WaitReadByte(void)
{
    volatile U32 sts=0;

    while(1)
    {
        sts = P_I2C->Status;
        if(sts & C_I2C_BYTE_Rx_Flag)
        {
            P_I2C->Status = P_I2C->Status;
            return I2C_WAIT_COMPLETE;
        }
        else if(sts & C_I2C_ARB_LOSE_Flag)
        {
            P_I2C->Status = P_I2C->Status;
            return I2C_ERR_ARB_LOSE;
        }
    }
}
//I2C write example function
U32 I2C_WriteByte(U16 addr, U16 len, U8 data[])
{
    U16 i;
    U32 return_state;
    P_I2C->Data = (addr>>8) & 0xFF;
    P_I2C->Data = addr & 0xFF;
    P_I2C->Mode = C_I2C_Master_Dir_Tx | C_I2C_PHA_Start | C_I2C_PHA_Addr | C_I2C_PHA_Data | 2;
    //Issue a data Transaction
    P_I2C->Cmd = C_I2C_CMD_Issue;
    return_state = WaitComplete();
    //reset I2C
//    P_I2C->Cmd = C_I2C_CMD_Reset;

    if(return_state)
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return return_state;
    }
    if(!(P_I2C->Status & C_I2C_ACK))
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return I2C_ERR_ACK;
    }

    //write data
    for(i=0; len>0; len-=4)
    {
        P_I2C->Data = data[i++];
        P_I2C->Data = data[i++];
        P_I2C->Data = data[i++];
        P_I2C->Data = data[i++];
//        P_I2C->Mode = C_I2C_PHA_Data;
        P_I2C->Cmd  = C_I2C_CMD_Issue | 4;
        P_I2C->Cmd  = C_I2C_CMD_Reset;
        return_state = WaitComplete();
        if(return_state)
        {
            P_I2C->Cmd   = C_I2C_CMD_Reset;
            return return_state;
        }
        if(!(P_I2C->Status & C_I2C_ACK))
        {
            P_I2C->Cmd   = C_I2C_CMD_Reset;
            return I2C_ERR_ACK;
        }
    }

    //STOP
    P_I2C->Mode = C_I2C_PHA_Stop;
    P_I2C->Cmd  = C_I2C_CMD_Issue;
    return_state = WaitComplete();

    P_I2C->Cmd  = C_I2C_CMD_Reset;
    if(return_state)	return return_state;

    return I2C_FINISH;
}


//I2C read example function
//set master in , send start pulse when start to send, send addr, send data after address
//Issue a data Transaction


U32 I2C_ReadByte(U16 addr, U16 len, U8 data[])
{
    U16 i;
    U32 return_state;


    P_I2C->Data = (addr>>8) & 0xFF;
    P_I2C->Data = addr & 0xFF;
    P_I2C->Mode = C_I2C_Master_Dir_Tx | C_I2C_PHA_Start | C_I2C_PHA_Addr | C_I2C_PHA_Data | 2;

    P_I2C->Cmd  = C_I2C_CMD_Issue;
    return_state = WaitComplete();
    if(return_state)
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return return_state;
    }
    if(!(P_I2C->Status & C_I2C_ACK))
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return I2C_ERR_ACK;
    }

    //Re-START + Device
    P_I2C->Mode = C_I2C_Master_Dir_Rx | C_I2C_PHA_Start | C_I2C_PHA_Addr;
    P_I2C->Cmd  = C_I2C_CMD_Issue;
    return_state = WaitComplete();
    if(return_state)
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return return_state;
    }
    if(!(P_I2C->Status & C_I2C_ACK))
    {
        P_I2C->Cmd   = C_I2C_CMD_Reset;
        return I2C_ERR_ACK;
    }

    //Read DATA
    P_I2C->Mode = C_I2C_Master_Dir_Rx | C_I2C_PHA_Data | len;
    P_I2C->Cmd  = C_I2C_CMD_Issue;
    for( i=0 ; len>0; len--)
    {
        return_state = WaitReadByte();
        if(return_state)
        {
            P_I2C->Cmd   = C_I2C_CMD_Reset;
            return return_state;
        }
        if( len != 1 )
            P_I2C->Cmd  = C_I2C_CMD_Resp_ACK;
        else
            P_I2C->Cmd  = C_I2C_CMD_Resp_NACK;
        data[i++] = P_I2C->Data;
    }

    //STOP
    P_I2C->Mode = C_I2C_PHA_Stop;
    P_I2C->Cmd  = C_I2C_CMD_Issue;
    return_state = WaitComplete();
    P_I2C->Cmd   = C_I2C_CMD_Reset;
    if(return_state)     return return_state;

    return I2C_FINISH;
}

