#include "A121_Dev_Config.h"

void A121_I2C_Write_Byte(uint16_t reg, uint32_t data)
{
    uint8_t buf[6];  // 2字节寄存器 + 4字节数据

    // 先写寄存器地址
    buf[0] = (reg >> 8) & 0xFF;
    buf[1] = reg & 0xFF;

    // 写数据，高字节在前
    buf[2] = (data >> 24) & 0xFF;
    buf[3] = (data >> 16) & 0xFF;
    buf[4] = (data >> 8)  & 0xFF;
    buf[5] = data & 0xFF;

    // 调用你封装的 I2C 写函数
    I2C_Write_Nbyte(buf, 6);
}

void A121_I2C_Read_Byte(uint16_t reg, uint32_t *data)
{
    uint8_t cmd[2];  // 寄存器地址，高字节在前
    uint8_t recv[4] = {0}; // 用于接收 4 字节数据

    
    cmd[1] = reg & 0xFF;
    cmd[0] = (reg >> 8) & 0xFF;

    // 调用 I2C_Read_Nbyte，发送寄存器地址并读取 4 字节数据
    I2C_Read_Nbyte(cmd, recv, 4);

    // 将接收到的 4 字节组合成 uint32_t
    *data = ((uint32_t)recv[0] << 24) | ((uint32_t)recv[1] << 16) |
            ((uint32_t)recv[2] << 8)  | ((uint32_t)recv[3]);
}
