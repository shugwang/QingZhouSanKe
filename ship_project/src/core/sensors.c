#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "bsp_ds18b20.h"
#include "bsp_sensors.h"
#include "navigation.h"

//控制任务
osThreadId_t Sensors_Task5_ID; //任务ID

sensors_data data={
    .pH=1.0f,
    .tds=0.0f,
    .temper=1.0f,

    .x = 0,
    .y = 0,
    .z = 0
};

void Sensors_Task5(void)
{
    static uint8_t i=0;
    float temper;
    unsigned char uartReadBuff[UART_BUFF_SIZE] = {0};
    int16_t x, y, z;
    hi_u32 ret;
    while(ds18b20_init())
	{
		printf("DS18B20 init failed, check connection!\r\n");
		sleep(10); //10s
	}
	printf("DS18B20 init success!\r\n");

    pH_init();
    tds_init();
    GPS_init();
    
    // 初始化HMC5883（连续测量模式）
    while( HMC5883_I2C_Init() != HI_ERR_SUCCESS ){
        printf("HMC5883_I2C_Init failed ret:%08X\r\n",HMC5883_I2C_Init());
    }
    HMC5883_WriteReg(0x00,0x70);//默认配置，8倍过采样，15Hz采样率
    HMC5883_WriteReg(0x01,0x20);//默认增益+-1.3Ga
    HMC5883_WriteReg(0x02,0x00);//连续测量
    
    while (1) 
    {
        i++;

        if(i%5==0)
        {
            data.temper = ds18b20_gettemperture();
            printf("temperatur: %.3f degree\r\n",data.temper);
            data.tds    = get_tds_value(data.temper, kValue);
            printf("tds: %.3fppm\r\n",data.tds);
            data.pH     = get_pH_value();
            printf("PH: %.3f\r\n",data.pH);    


            // if(uart_send_data(HI_UART_IDX_2,"uart_send_data is ok, i value",10)>0){
            //     printf("uart_send_data: %d\r\n",i); 
            // }else if(uart_send_data(HI_UART_IDX_2,"uart_send_data is ok, i value",sizeof("uart_send_data is ok, i value"))>0){
            //     printf("uart_send_data is ok, i value: %d\r\n",i);
            // }else{
            //     printf("i value: %d\r\n",i);
            // }
        }
        // if(i%10==0)
        // {   
        //     ret = HMC5883_ReadData(&x, &y, &z);
        //     if( ret == HI_ERR_SUCCESS){
        //         printf("x: %d, y: %d, z: %d\r\n", x, y, z);
        //         data.x = x;
        //         data.y = y;
        //         data.z = z;
        //     }else{
        //         printf("read data ret not ok  :%08X\r\n",ret);
        //         HMC5883_I2C_Init();
        //         ret = HMC5883_WriteReg(0x02,0x00);
        //         if(ret ==HI_ERR_SUCCESS)//连续测量
        //         {
        //             printf("write 02 00 ok");
        //         }else{
        //             printf("write data 02 00 ret not ok:%08X\r\n",ret);
        //             ret = HMC5883_WriteReg(0xAA,0x55);
        //             printf("write data AA 55 ret:%08X\r\n",ret);
        //         }
        //     }
            
            // GPS_get_data(uartReadBuff);
        // }

        usleep(SENSOR_TASK_GAP); //1s
    }
}

//任务创建
void Sensors_Task5_create(void)
{
    osThreadAttr_t taskOptions;
    taskOptions.name = "Sensors_Task5_create";       // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024*5;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityNormal1; // 任务的优先级

    Sensors_Task5_ID = osThreadNew((osThreadFunc_t)Sensors_Task5, NULL, &taskOptions); // 创建任务
    if (Sensors_Task5_ID != NULL)
    {
        printf("ID = %d, Sensors_Task5 Create OK!\n", Sensors_Task5_ID);
    }
}

// tds
void tds_init(void)
{
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(tds_PIN, HI_IO_PULL_UP);                   // 设置GPIO上拉
}
float get_tds_value(float temper, float K)
{
    float buf[10];                // 模拟量读取缓冲区
    
    // 从传感器获取10个采样值
    for (int i = 0; i < 10; i++) {
        buf[i] = get_adc_voltage(tds_ADC_CHANNEL);
        usleep(10*1000);          // 10ms延迟
    }
    
    // 对采样值进行排序(从小到大)
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (buf[i] > buf[j]) {
                float temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    
    // 取中间6个样本的平均值
    float avgValue = 0;
    for (int i = 2; i < 8; i++) {
        avgValue += buf[i]/6.0f;
    }
    
    // 将模拟量转换为tds值
    float tdsValue;
    float T = 1+0.02*(temper-25); //温度修正系数
    float V = avgValue*T;         //电压修正值
    float TDS_messure = 66.71*V*V*V-127.93*V*V+428.7*V;         // TDS测量值，通过校准可得到TDS标准值
    float TDS_standard=TDS_messure*K;                           // 通过标准tds溶液，可得到校准系数K
    
    printf("tds: %.2f\n", TDS_standard);
    return TDS_standard;
}


// pH
void pH_init(void)
{
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(pH_PIN, HI_IO_PULL_UP);                   // 设置GPIO上拉
}

float get_pH_value(void)
{
    float buf[10];                // 模拟量读取缓冲区
    
    // 从传感器获取10个采样值
    for (int i = 0; i < 10; i++) {
        buf[i] = get_adc_voltage(pH_ADC_CHANNEL);
        usleep(10*1000);          // 10ms延迟
    }
    
    // 对采样值进行排序(从小到大)
    for (int i = 0; i < 9; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (buf[i] > buf[j]) {
                float temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    
    // 取中间6个样本的平均值
    float avgValue = 0;
    for (int i = 2; i < 8; i++) {
        avgValue += buf[i]/6.0f;
    }
    
    // 将模拟量转换为pH值
    float phValue;
    phValue = 3.5f * avgValue + pH_OFFSET;                      // 转换为pH值
    
    printf("pH值: %.2f\n", phValue);
    return phValue;
}   

//读取AD值
uint16_t get_adc_value(hi_adc_channel_index adc_channel)
{
    uint16_t data;
    hi_adc_read(adc_channel,&data,HI_ADC_EQU_MODEL_8,HI_ADC_CUR_BAIS_DEFAULT,0xff);
    return data;
}

//读取电压
float get_adc_voltage(hi_adc_channel_index adc_channel)
{
    return hi_adc_convert_to_voltage(get_adc_value(adc_channel));
}
