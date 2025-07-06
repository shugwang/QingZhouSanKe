#include "pump_task.h"
#include "cmd_fifo.h"

osThreadId_t pump_Task2_ID;

#ifdef CMD_FIFO_QD
extern CMD_FIFO CMD_Q[CONSUMERS_NUM];
#else
extern osMessageQueueId_t MsgQueue_ID1;
#endif

void pump_Task(void)
{
    pump_init();
    COMMAND cmd;
    #ifdef CMD_FIFO_QD
    CMD_FIFO * fifo_p;
    fifo_p = &CMD_Q[PUMP_ID];
    while(1){
        if( CMD_FIFO_POP(&CMD_Q[PUMP_ID], &cmd) ){
    #else
    while(1){
        if( osMessageQueueGet(MsgQueue_ID1, &cmd, 0, MESSAGE_TIMEOUT) == osOK ){
    #endif
            printf("pump_task pop cmd: %x %x %x %x \r\n",cmd.task_id, cmd.func_id, cmd.data_1, cmd.data_2 );
            if( cmd.func_id == 0x01){ 
                printf(" start pump water\r\n ");
                pump_water();
            }
            else if( cmd.func_id == 0x02 ){
                printf(" start drain water\r\n ");
                drain_water();
            }
            else if( cmd.func_id == 0x03 ){
                printf(" stop pump\r\n ");
                stop_pump();
            }
        }

        usleep(1000*1000);
    }
}


void pump_init(void)
{
    hi_gpio_init();                                            // GPIO初始化
    hi_io_set_pull(PUMP_IN1_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(PUMP_IN1_PIN, PUMP_IN1_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(PUMP_IN1_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式

    hi_io_set_pull(PUMP_IN2_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(PUMP_IN2_PIN, PUMP_IN2_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(PUMP_IN2_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
}

// 还需要考虑什么时候让水泵停止

void pump_water(void){
    hi_gpio_set_ouput_val(PUMP_IN1_PIN,0);
    hi_gpio_set_ouput_val(PUMP_IN2_PIN,1);
}

void drain_water(void)
{
    hi_gpio_set_ouput_val(PUMP_IN1_PIN,1);
    hi_gpio_set_ouput_val(PUMP_IN2_PIN,0);
}

void stop_pump(void)
{
    hi_gpio_set_ouput_val(PUMP_IN1_PIN,0);
    hi_gpio_set_ouput_val(PUMP_IN2_PIN,0);
}

void pump_Task2_create(void)
{
    osThreadAttr_t options;
    options.name = "pump_task2";       // 任务的名字
    options.attr_bits = 0;      // 属性位
    options.cb_mem = NULL;      // 堆空间地址
    options.cb_size = 0;        // 堆空间大小
    options.stack_mem = NULL;   // 栈空间地址
    options.stack_size = 1024;  // 栈空间大小 单位:字节
    options.priority = osPriorityNormal;  // 任务的优先级

    pump_Task2_ID = osThreadNew((osThreadFunc_t)pump_Task, NULL, &options);      // 创建任务1
    if (pump_Task2_ID != NULL) {
        printf("ID = %d, Create pump_Task2_ID is OK!\r\n", pump_Task2_ID);
    }
}