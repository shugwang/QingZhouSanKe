#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_pwm.h"
#include "hi_io.h"
#include "hi_gpio.h"

#include "ship_config.h"

#define PRESCALER       40000
#define DUTY_50         PRESCALER/2
#define SPEED(a)         (hi_u16)( (float)a/128 * (PRESCALER)*0.95 + 1)   

typedef enum {
    MOTOR_1=1,
    MOTOR_2
} motor_id;

typedef enum {
    MOTOR_STOP,     // 不转（停止或者刹车）
    MOTOR_FORWARD,  // 正转
    MOTOR_REVERSE   // 反转
} motor_dir;

static void         ZK5AD_init();
static motor_dir    get_motor_dir(motor_id motor);
static void         stop_motor(motor_id motor);
static void         set_motor_dir(motor_id motor,motor_dir dir);
static void         set_motor_speed(motor_id motor,hi_s8 speed);
static void         set_motor_logic_val(motor_id motor,hi_s8 speed);


void motor_init(void);

#ifdef L298N

/// @brief 设置左电机的转速和旋转方向
/// @param speed 有符号速度，127级变速
void set_left_motor_speed(hi_s8 speed);

/// @brief 设置右电机的转速和旋转方向
/// @param speed 有符号速度，127级变速
void set_right_motor_speed(hi_s8 speed);

void motor_stop(void);

#endif

void MOVE_Task(void);
void move_Task1_create(void);
