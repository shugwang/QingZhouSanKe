#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "bsp_ds18b20.h"
#include "bsp_sensors.h"

//控制任务
osThreadId_t Sensors_Task5_ID; //任务ID

void Sensors_Task5(void)
{
    static uint8_t i=0;
    float temper;
    sensors_data data={
        .pH=0.0f,
        .tds=0.0f,
        .temper=0.0f
    };
    

    while(ds18b20_init())
	{
		printf("DS18B20 init failed, check connection!\r\n");
		sleep(10); //10s
	}
	printf("DS18B20 init success!\r\n");

    pH_init();
    tds_init();

    while (1) 
    {
        i++;
        if(i%10==0)
        {
            data.temper = ds18b20_gettemperture();
            printf("temperatur: %.3f degree\r\n",data.temper);

			// data.pH     = get_pH_value();
            // printf("PH值: %.3f\r\n",data.pH);

            data.tds    = get_tds_value(data.temper, kValue);
            printf("tds: %.3fppm\r\n",data.tds);
        }

        usleep(1000*1000); //1s
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

float get_pH_value()
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
