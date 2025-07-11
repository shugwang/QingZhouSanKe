#include "ship_config.h"

#include <stdbool.h>
#include <stdint.h>

#include "cmsis_os2.h"

// 指令fifo的数据结构
#define FIFO_SIZE 8       // 每个消费者的FIFO大小（可根据需要调整）
#define CMD_LENGTH 4       // 指令长度
#define MUX_TIME_OUT osWaitForever    
#define CONSUMERS_NUM CMD_TASK_ID_MAX    // 消费者数量

typedef struct
{
    char task_id;
    char func_id;
    char data_1;
    char data_2;
}COMMAND;

//以下方案存在问题。系统调度时会使mqtt的system panic
typedef struct {
    COMMAND cmd[FIFO_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
    osMutexId_t mux_id;    // 每个FIFO独立的互斥锁
}CMD_FIFO;


bool CMD_FIFO_Init(CMD_FIFO *fifo);
bool CMD_FIFO_PUSH(CMD_FIFO *fifo, COMMAND *cmd);
bool CMD_FIFO_POP(CMD_FIFO *fifo, COMMAND *cmd_out);