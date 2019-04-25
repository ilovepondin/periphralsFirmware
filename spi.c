#include "spi.h"
#include "debug.h"
#define SPI0_FIFO_SIZE 16
#define SPI_NUM_POSITION 16

#define write 1
#define read 1

/*===========================================================*
*                    SPI-Flash r/w example                   *
*                                                            *
*============================================================*/

void spi_en()
{
    U8 a;
    U16 i, j;
    U32 readbuff[10];
    U32 addrerase = 0x8000;


    //enable SMU GPIO
    P_SMU->FUNC_Ctrl |= C_Func_GPIO_En;
    //
    P_PORTB->MFP |= C_MFP_SPI0_SCLK_SIO01_En;
    //enable chip_select
    P_PORTB->MFP |= C_MFP_SPI0_CSB_En;



    //LDO SPI0 enable
    P_SMU->PWR_Ctrl |= C_LDOSPI0_En;
    //enable spi clock
    P_SMU->FUNC_Ctrl |= C_Func_SPI_En;
    //fifo reset
    P_SPI0->FIFO_RST |= C_SPI_FIFO_Reset;
    //spi clock = system clock / selected divider :1, 2, 4, 8, 16, 32, 64, 128
    P_SPI0->Ctrl |= C_SPI_CLK_Div_1
            //SPI Clock_Idle_Polarity Setting: hi or low
            | C_SPI_Pol_Low
            //SPI Clock Latching Phase Setting: 1st edge or 2nd edge latch
            | C_SPI_Phase_1st_Edge
            //1IO, 2IO or 4IO select
            | C_SPI_DataMode_1IO;



    

    /*============================================================*
    *                      flash erase sector                     *
    *                                                             *
    * ============================================================*/
    //flash erase
    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //spi-flash write enable cmd
    P_SPI0->Data = SPI_WREN_CMD;
    //ctrl bits clear
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    //start to send data |data mode 1IO selected
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //clear BIT_SET and reset data mode
    P_SPI0->Ctrl &= (~C_SPI_DataMode);
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;
    //chip select input high
    P_PORTB->Data |= (1<<0);



    //sector erase
    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //spi-flash sector_erase cmd
    P_SPI0->Data = SPI_SECTOR_ERASE_CMD;
    //ctrl bits clear
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    //start to send data |data mode 1IO selected
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //setting address to start erasing
    P_SPI0->Data=((addrerase&0xFF)<<16)|(addrerase&0xFF00)|(addrerase>>16);
    //start to send data |data mode 1IO selected
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    //start to send | byte numbers to send:3  | data mode 1IO
    P_SPI0->Ctrl |= (C_SPI_Tx_Start |((3-1)<<SPI_NUM_POSITION)| C_SPI_DataMode_1IO);
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //clear BIT_SET and reset data mode
    P_SPI0->Ctrl &= (~(C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;
    //chip select input high
    P_PORTB->Data |= (1<<0);


    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //set read status register1 cmd
    P_SPI0->Data = SPI_READ_STS_REG1_CMD;
    //clear bits
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    //start to send data |data mode 1IO selected
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //clear bits and reset data mode 1IO
    P_SPI0->Ctrl &= (~C_SPI_DataMode);
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;


    //wait for reading 1byte time per data
    do{
        P_SPI0->Ctrl &= (~C_SPI_FIFO_Length);
        P_SPI0->Ctrl |= (C_SPI_Rx_Start | ((1-1)<<SPI0_FIFO_SIZE));
        //wait for sending data
        do{
        }while(P_SPI0->Ctrl&C_SPI_Rx_Busy);
    }while(P_SPI0->Data&0x1);

    //chip select input high
    P_PORTB->Data |= (1<<0);
    //WDT clear
    P_WDT->CLR = C_WDT_CLR_Data;



    /*============================================================*
     *                      SPI Write Example                     *
     *                                                            *
     * ===========================================================*/



    //spi write example
#if write
    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //flash write cmd
    P_SPI0->Data = SPI_WREN_CMD;
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //chip select input high
    P_PORTB->Data |= (1<<0);

    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //set page program cmd(write cmd)
    P_SPI0->Data = SPI_PAGE_PRG_CMD;
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //set write address
    P_SPI0->Data = ((0x8000&0xFF)<<16)|(0x8000&0xFF00)|(0x8000>>16);
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    //start to send | send byte numbers |select data mode 1IO
    P_SPI0->Ctrl |= (C_SPI_Tx_Start |((3-1)<<SPI_NUM_POSITION)| C_SPI_DataMode_1IO);
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);

    P_SPI0->Ctrl &= (~(C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;

    // write data content
    for (i = 0; i < 4 ; i++)
    {
        //data
        P_SPI0->Data = 0x12345671+i;
        dprintf("P_SPI0->Data = %d\n\r", P_SPI0->Data);
    }
    dprintf("P_SPI0->Data = %d\n\r", P_SPI0->Data);

    //setting number byte to send, and start to send
    P_SPI0->Ctrl &= (~C_SPI_FIFO_Length);
    //send byte numbers| start to send
    P_SPI0->Ctrl |= (16-1 << SPI_NUM_POSITION) | C_SPI_Tx_Start;
    //waiting for send out
    while(P_SPI0->Ctrl & C_SPI_Tx_Start);
    //chip select input high
    P_PORTB->Data |= (1<<0);





    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //setting read status register cmd
    P_SPI0->Data=SPI_READ_STS_REG1_CMD;
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length|C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_Tx_Start| C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    P_SPI0->Ctrl &= (~C_SPI_DataMode);
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;
    //wait for reading 1byte time per data
    do{
        P_SPI0->Ctrl &= (~C_SPI_FIFO_Length);
        P_SPI0->Ctrl |= (C_SPI_Rx_Start | (0<<SPI_NUM_POSITION));
        //wait for sending data
        do{
        }while(P_SPI0->Ctrl&C_SPI_Rx_Busy);
    }while(P_SPI0->Data&0x1);

    //chip select input high
    P_PORTB->Data |= (1<<0);



#endif






    /*============================================================*
    *                      SPI read example                       *
    *                                                             *
    * ============================================================*/

#if read
    

    //sending flash read cmd
    //chip select input low
    P_PORTB->Data &= ~(1<<0);
    //setting read flash cmd
    P_SPI0->Data=SPI_READ_CMD;
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_Tx_Start | C_SPI_DataMode_1IO;
    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);
    //setting read start address
    P_SPI0->Data = ((0x8000&0xFF)<<16)|(0x8000&0xFF00)|(0x8000>>16);
    P_SPI0->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    P_SPI0->Ctrl |= (C_SPI_Tx_Start |((3-1)<<SPI_NUM_POSITION)| C_SPI_DataMode_1IO);

    //wait for sending data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Tx_Busy);

    P_SPI0->Ctrl &= (~(C_SPI_DataMode));
    P_SPI0->Ctrl |= C_SPI_DataMode_1IO;



    //read data

    P_SPI0->Ctrl &= (~C_SPI_FIFO_Length);
    //setting number bytes to read, and start to read
    P_SPI0->Ctrl |= ((16-1) << SPI_NUM_POSITION) | C_SPI_Rx_Start;

    //wait for reading data
    do{
    }while(P_SPI0->Ctrl&C_SPI_Rx_Busy);

    //wait for reading
    while(P_SPI0->Ctrl & C_SPI_Rx_Start);

    //read 4bytes(P_SPI0->Data) for once ,4 times for 1FIFO size

    for(a=0;a<4;a++)
    {

        readbuff[a] = P_SPI0->Data;
        dprintf("readbuff[%d] = %d\n\r", a, readbuff[a]);

    }

    //chip select input high
    P_PORTB->Data |= (1<<0);

#endif

}










