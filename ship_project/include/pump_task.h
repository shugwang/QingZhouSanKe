#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_io.h"
#include "hi_gpio.h"

#include "ship_config.h"

/// @brief 初始化水泵电机的控制引脚
/// @param 无
void pump_init(void);


/// @brief 水泵抽水
/// @param  
void pump_water(void);

/// @brief 排空水
/// @param  
void drain_water(void);



/// @brief 停止水泵
/// @param  
void stop_pump(void);

/**
 * @description: 水泵控制模块任务
 * @param {*}
 * @return {*}
 */
void pump_Task(void);

void pump_Task2_create(void);