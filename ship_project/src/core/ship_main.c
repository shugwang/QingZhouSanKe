/**
 * ***************************************************************************************************
 * @file        ship_main.c
 * @author      wsg
 * @brief       无人控制船的主函数，负责启动、结束各个任务
 * @date        2025年4月27日
 * ***************************************************************************************************
 */

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "ship_config.h"
#include "ship_drive.h"
#include "mqtt_task.h"
#include "pump_task.h"
#include "cmd_fifo.h"

// 指令队列
CMD_FIFO CMD_Q[CONSUMERS_NUM];

int8_t mqtt_sub_payload_callback(unsigned char *topic, unsigned char *payload)
{
    // cmd:00 01 01 02, task_id func_id data_1 data_2
    COMMAND cmd;
    sscanf(payload , "%d %d %d %d", &cmd.task_id, &cmd.func_id, &cmd.data_1, &cmd.data_2);
    if(cmd.task_id<CONSUMERS_NUM && cmd.task_id>=0){
        // if(CMD_FIFO_PUSH(&CMD_Q[cmd.task_id],&cmd)){
        //     printf("push cmd: %x %x %x %x \r\n",cmd.task_id, cmd.func_id, cmd.data_1, cmd.data_2 );
        // }
    }
}




/// @brief 任务ID，全局变量
extern osThreadId_t move_Task1_ID; 
extern osThreadId_t pump_Task2_ID;
extern osThreadId_t mqtt_send_Task3_id;   // mqtt订阅数据任务
extern osThreadId_t mqtt_recv_Task4_id;   // mqtt发布数据任务


/**
 * @description: 初始化并创建任务
 * @param {*}
 * @return {*}
 */
static void ship_main(void)
{
    // 初始化指令空间
    for(int i=0;i<CONSUMERS_NUM;i++){
        CMD_FIFO_Init(&CMD_Q[i]);
    }

    move_Task1_create();
    pump_Task2_create();
    wifi_mqtt_send_Task3_create();
    // ds18b20_task_create();//任务创建

}

SYS_RUN(ship_main);




