#include "ship_config.h"

#include <stdbool.h>
#include <stdint.h>

#include "cmsis_os2.h"

// 指令fifo的数据结构
#define FIFO_SIZE 64       // 每个消费者的FIFO大小（可根据需要调整）
#define CMD_LENGTH 4       // 指令长度
#define CONSUMERS_NUM 2    // 消费者数量
#define MUX_TIME_OUT 0    // 非阻塞申请

typedef struct
{
    char task_id;
    char func_id;
    char data_1;
    char data_2;
}COMMAND;

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