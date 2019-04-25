#ifndef I2C_H
#define I2C_H


U32 I2C_WriteByte(U16 addr, U16 len, U8 data[]);
U32 I2C_ReadByte(U16 addr, U16 len, U8 data[]);

#endif // I2C_H
