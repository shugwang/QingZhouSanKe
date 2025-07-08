/*******************

主要功能：读取GPS、电子罗盘、红外避障的数据并解析

*********************/

#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_uart.h"
#include "ship_config.h"
#include "stdint.h"


// 磁力计
#define HMC5883_ADDR 0x1E  // HMC5883的I2C地址

// 初始化I2C（Hi3861的I2C0默认引脚：GPIO0-SCL, GPIO1-SDA）
hi_u32 HMC5883_I2C_Init(void);
hi_u32 HMC5883_WriteReg(uint8_t reg, uint8_t val);
hi_u32 HMC5883_ReadData(int16_t *x, int16_t *y, int16_t *z) ;



// GPS 

//GPS函数声明
void GPS_init(void);
void GPS_get_data(unsigned char* uartReadBuff);
double dm_to_dd(double dm);


//串口 管脚定义
#define UART_TX_PIN            GPS_TX_PIN
#define UART_TX_GPIO_FUN       GPS_TX_GPIO_FUN
#define UART_RX_PIN            GPS_RX_PIN
#define UART_RX_GPIO_FUN       GPS_RX_GPIO_FUN

#define UART_BUFF_SIZE          1024
//串口功能函数声明
void uart_init(hi_uart_idx uart_idx,uint32_t baud);
int uart_send_data(hi_uart_idx uart_idx,uint8_t *dat,uint32_t len);
int uart_read_data(hi_uart_idx uart_idx,uint8_t *dat,uint32_t len);

