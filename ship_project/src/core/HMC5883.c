#include "navigation.h"
#include "hi_i2c.h"

// 初始化I2C（Hi3861的I2C0默认引脚：GPIO0-SCL, GPIO1-SDA）
hi_u32 HMC5883_I2C_Init(void) {
    
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(HMC_SDA_PIN, HI_IO_PULL_UP);                // 设置GPIO上拉
    hi_io_set_func(HMC_SDA_PIN, HI_IO_FUNC_GPIO_13_I2C0_SDA);  // 设置IO为串口功能
    // hi_gpio_set_dir(HMC_SDA_PIN, HI_GPIO_DIR_OUT);             // 设置GPIO为输出模式

    hi_io_set_pull(HMC_SCL_PIN, HI_IO_PULL_UP);                // 设置GPIO上拉
    hi_io_set_func(HMC_SCL_PIN, HI_IO_FUNC_GPIO_14_I2C0_SCL);  // 设置IO为串口功能
    // hi_gpio_set_dir(HMC_SCL_PIN, HI_GPIO_DIR_OUT);             // 设置GPIO为输出模式

    return hi_i2c_init(HI_I2C_IDX_0, 100000);  // 初始化I2C0，频率100kHz
}

// 向HMC5883写入配置寄存器
hi_u32 HMC5883_WriteReg(uint8_t reg, uint8_t val) {
    hi_i2c_data i2c_data = {0};
    uint8_t buf[2] = {reg, val};
    
    i2c_data.send_buf = buf;
    i2c_data.send_len = 2;
    
    return hi_i2c_write(HI_I2C_IDX_0, HMC5883_ADDR, &i2c_data);
}

// 从HMC5883读取数据
hi_u32 HMC5883_ReadData(int16_t *x, int16_t *y, int16_t *z) {
    hi_i2c_data i2c_data = {0};
    uint8_t reg = 0x03;  // 起始寄存器地址
    uint8_t buf[6] = {0};

    // 先写入寄存器地址
    i2c_data.send_buf = &reg;
    i2c_data.send_len = 1;
    hi_u32 ret = hi_i2c_write(HI_I2C_IDX_0, HMC5883_ADDR, &i2c_data);
    if (ret != HI_ERR_SUCCESS) {
        return ret;
    }

    // 读取6字节数据
    i2c_data.receive_buf = buf;
    i2c_data.receive_len = 6;
    ret = hi_i2c_read(HI_I2C_IDX_0, HMC5883_ADDR, &i2c_data);
    if (ret == HI_ERR_SUCCESS) {
        *x = (buf[0] << 8) | buf[1];
        *z = (buf[2] << 8) | buf[3];
        *y = (buf[4] << 8) | buf[5];
    }
    return ret;
}