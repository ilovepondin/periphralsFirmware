/*============================================================*
*                       I2C R/W example                       *
*             GPIO MFP bit[20]:PA4 input voltage              *
*                                                             *
* ============================================================*/

#include "i2c.h"

#define   LENTH              4
#define   ADDRESS            0x800
int main(void)
{ 


    //write data
    U8 Tx_Data[LENTH] = {0xAA,1,2,3};
    //read data
    U8 Rx_Data[LENTH];


    while(1)
    {

        I2C_WriteByte(address, lenth, Tx_Data);
        I2C_ReadByte(address, lenth, Rx_Data);



        P_WDT->CLR = C_WDT_CLR_Data;

    }

}

