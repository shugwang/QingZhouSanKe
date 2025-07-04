#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "bsp_ds18b20.h"


//控制任务
osThreadId_t DS18B20_Task_ID; //任务ID

void DS18B20_Task(void)
{
    static uint8_t i=0;
    float temper;

    while(ds18b20_init())
	{
		printf("DS18B20检测失败，请插好!\r\n");
		sleep(10); //10s
	}
	printf("DS18B20检测成功!\r\n");

    while (1) 
    {
        i++;
        if(i%10==0)
        {
            temper=ds18b20_gettemperture();
			if(temper<0)
			{
				printf("检测的温度为：-");
			}
			else
			{
				printf("检测的温度为： ");
			}
			printf("%.1f°C\r\n",temper);
        }

        usleep(1000*1000); //1s
    }
}
//任务创建
void ds18b20_task_create(void)
{
    osThreadAttr_t taskOptions;
    taskOptions.name = "ds18b20Task";       // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityNormal1; // 任务的优先级

    DS18B20_Task_ID = osThreadNew((osThreadFunc_t)DS18B20_Task, NULL, &taskOptions); // 创建任务
    if (DS18B20_Task_ID != NULL)
    {
        printf("ID = %d, Task Create OK!\n", DS18B20_Task_ID);
    }
}
