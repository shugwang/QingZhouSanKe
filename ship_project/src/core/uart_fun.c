#include "navigation.h"

//串口初始化
void uart_init(hi_uart_idx uart_idx,uint32_t baud)
{
    hi_uart_attribute uart_attr;
    
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(UART_TX_PIN, HI_IO_PULL_UP);                // 设置GPIO上拉
    hi_io_set_func(UART_TX_PIN, UART_TX_GPIO_FUN);            // 设置IO为串口功能
    hi_gpio_set_dir(UART_TX_PIN, HI_GPIO_DIR_OUT);             // 设置GPIO为输出模式

    hi_io_set_pull(UART_RX_PIN, HI_IO_PULL_NONE);                // 设置GPIO浮空
    hi_io_set_func(UART_RX_PIN, UART_RX_GPIO_FUN);            // 设置IO为串口功能
    hi_gpio_set_dir(UART_RX_PIN, HI_GPIO_DIR_IN);             // 设置GPIO为输入模式

    uart_attr.baud_rate=baud;
    uart_attr.data_bits=HI_UART_DATA_BIT_8;
    uart_attr.stop_bits=HI_UART_STOP_BIT_1;
    uart_attr.parity=HI_UART_PARITY_NONE;
    hi_uart_init(uart_idx,&uart_attr,NULL);
}

//串口发送数据
int uart_send_data(hi_uart_idx uart_idx, uint8_t *dat,uint32_t len)
{
    return hi_uart_write(uart_idx,dat,len);
}

//串口读取数据
int uart_read_data(hi_uart_idx uart_idx,uint8_t *dat,uint32_t len)
{
    return hi_uart_read(uart_idx,dat,len);
}