#include "cmd_fifo.h"
#include <string.h>

bool CMD_FIFO_Init(CMD_FIFO *fifo) {
        fifo->head = 0;
        fifo->tail = 0;
        fifo->count = 0;
        memset(fifo->cmd, 0, sizeof(fifo->cmd));
        fifo->mux_id = osMutexNew(NULL);
        if (fifo->mux_id == NULL) {
            printf("Create Mutex_ID failed!\n");
            return false;
        }
    return true;
}

bool CMD_FIFO_PUSH(CMD_FIFO *fifo, COMMAND *cmd)
{
    osMutexAcquire(fifo->mux_id , MUX_TIME_OUT);
    
    if (fifo->count == FIFO_SIZE) {
        osMutexRelease(fifo->mux_id);
        return false; // 该消费者的FIFO已满
    }
    
    // 复制指令数据
    memcpy(&fifo->cmd[fifo->tail], cmd, sizeof(COMMAND));
    
    fifo->tail = (fifo->tail + 1) % FIFO_SIZE;
    fifo->count++;
    
    osMutexRelease(fifo->mux_id);
    
    return true;
}

bool CMD_FIFO_POP(CMD_FIFO *fifo, COMMAND *cmd_out)
{
    osMutexAcquire(fifo->mux_id , MUX_TIME_OUT);
    
    // 无指令待执行
    if (fifo->count == 0) {
        osMutexRelease(fifo->mux_id);
        return false;
    }
    memcpy(cmd_out, &fifo->cmd[fifo->head], sizeof(COMMAND));
    fifo->head = (fifo->head + 1) % FIFO_SIZE;
    fifo->count--;
    
    osMutexRelease(fifo->mux_id);
    return true;
}