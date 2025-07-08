
#include "ship_drive.h"
#include "cmd_fifo.h"

osThreadId_t move_Task1_ID; 
#ifdef CMD_FIFO_QD
extern CMD_FIFO CMD_Q[CONSUMERS_NUM];
#else
extern osMessageQueueId_t MsgQueue_ID0;
#endif
void MOVE_Task(void)
{
    motor_init();//电机控制模块 初始化
    COMMAND cmd;
    #ifdef CMD_FIFO_QD
    CMD_FIFO * fifo_p;
    fifo_p = &CMD_Q[DRIVE_ID];
    while(1) {
        if( CMD_FIFO_POP(fifo_p, &cmd) ){
    #else
    while(1) {
        if( osMessageQueueGet(MsgQueue_ID0, &cmd, 0, MESSAGE_TIMEOUT) == osOK ){
    #endif
            printf("drive_task pop cmd: %x %x %x %x \r\n",cmd.task_id, cmd.func_id, cmd.data_1, cmd.data_2 );
            if( cmd.func_id == 0x01){ //设置两个电机的转速
                set_motor_speed(1,cmd.data_1);
                set_motor_speed(2,cmd.data_2);
            }
            else if( cmd.func_id == 0x02){ //两个电机刹车
                stop_motor(1);
                stop_motor(2);
            }

            else if( cmd.func_id == 0x03){ //设置电机1的转速
                set_motor_speed(1,cmd.data_1);
            }
            else if( cmd.func_id == 0x04){ //设置电机2的转速
                set_motor_speed(2,cmd.data_2);
            }

            else if( cmd.func_id == 0x05){ //设置电机1的正反转,只看data_1的符号
                set_motor_logic_val(1,cmd.data_1);
            }
            else if( cmd.func_id == 0x06){ //设置电机2的正反转
                set_motor_logic_val(2,cmd.data_2);
            }

        }
        
        usleep(DRIVE_TASK_GAP);
    }
}

void motor_init()
{
   ZK5AD_init();
}

// ZK-5AD的控制函数

static void ZK5AD_init()
{
    hi_gpio_init();                                            // GPIO初始化

    hi_pwm_init(MOTOR1_PWM);
    hi_pwm_start(MOTOR1_PWM,DUTY_50,PRESCALER);                    // 设置PWM的占空比和分频系数

    hi_io_set_pull(D0_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(D0_PIN, D0_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(D0_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_gpio_set_ouput_val(D0_PIN,0);

    hi_io_set_pull(D1_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(D1_PIN, D1_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(D1_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_gpio_set_ouput_val(D1_PIN,0);

    hi_pwm_init(MOTOR2_PWM);
    hi_pwm_start(MOTOR2_PWM,DUTY_50,PRESCALER);                    // 设置PWM的占空比和分频系数

    hi_io_set_pull(D2_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(D2_PIN, D2_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(D2_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_gpio_set_ouput_val(D2_PIN,0);

    hi_io_set_pull(D3_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(D3_PIN, D2_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(D3_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_gpio_set_ouput_val(D3_PIN,0);

}

static motor_dir get_motor_dir(motor_id motor)
{
    hi_u8 pinA,pinB;
    
    if(motor == MOTOR_1){
        hi_io_get_func(D0_PIN, &pinA);
        hi_io_get_func(D1_PIN, &pinB);

        if( pinA == D0_PWM_FUN ){
            return MOTOR_FORWARD;
        }
        else if(pinB == D1_PWM_FUN){
            return MOTOR_REVERSE;
        }
        else if(pinA == D0_GPIO_FUN & pinB == D1_GPIO_FUN){
            hi_gpio_value pinA_val,pinB_val;
            hi_gpio_get_output_val(D0_PIN,&pinA_val);
            hi_gpio_get_output_val(D1_PIN,&pinB_val);
            if( pinA_val == 1 & pinB_val == 0){
                return MOTOR_FORWARD;
            }else if( pinA_val == 0 & pinB_val == 1){
                return MOTOR_REVERSE;
            }else{
                return MOTOR_STOP;
            }
        }

    }
    else if(motor == MOTOR_2){
        hi_io_get_func(D2_PIN, &pinA);
        hi_io_get_func(D3_PIN, &pinB);

        if(pinA == D2_PWM_FUN){
            return MOTOR_FORWARD;
        }
        else if(pinB == D3_PWM_FUN){
            return MOTOR_REVERSE;
        }
        else if(pinA == D2_GPIO_FUN & pinB == D3_GPIO_FUN){
            hi_gpio_value pinA_val,pinB_val;
            hi_gpio_get_output_val(D2_PIN,&pinA_val);
            hi_gpio_get_output_val(D3_PIN,&pinB_val);
            if( pinA_val == 1 & pinB_val == 0){
                return MOTOR_FORWARD;
            }else if( pinA_val == 0 & pinB_val == 1){
                return MOTOR_REVERSE;
            }else{
                return MOTOR_STOP;
            }
        }
    }
    
    return 3; // 错误方向
}

static void stop_motor(motor_id motor)
{
    if(motor == MOTOR_1){
        hi_io_set_func(D0_PIN, D0_GPIO_FUN);
        hi_io_set_func(D1_PIN, D1_GPIO_FUN);
        hi_gpio_set_ouput_val(D0_PIN,1);
        hi_gpio_set_ouput_val(D1_PIN,1);
    }
    else if(motor == MOTOR_2){
        hi_io_set_func(D2_PIN, D2_GPIO_FUN);
        hi_io_set_func(D3_PIN, D3_GPIO_FUN);
        hi_gpio_set_ouput_val(D2_PIN,1);
        hi_gpio_set_ouput_val(D3_PIN,1);
    }
}

static void set_motor_dir(motor_id motor,motor_dir dir)
{
    // 获取方向
    motor_dir cur_dir = get_motor_dir(motor);
    if( cur_dir == dir ){
        return;
    }
    // 换向前需将电机暂停
    printf("[info] motor %d cur dir :[%d] , dir :[%d]\r\n",motor, cur_dir,dir);
    stop_motor(motor);
    usleep(1000);
    
    // 设置方向
    if(motor == MOTOR_1){
        if(dir == MOTOR_FORWARD){
            hi_io_set_func(D0_PIN, D0_PWM_FUN);
            hi_io_set_func(D1_PIN, D1_GPIO_FUN);
            
            // hi_gpio_set_ouput_val(D0_PIN,1);
            hi_gpio_set_ouput_val(D1_PIN,0);
        }
        else if(dir == MOTOR_REVERSE){
            hi_io_set_func(D0_PIN, D0_GPIO_FUN);
            hi_io_set_func(D1_PIN, D1_PWM_FUN);

            hi_gpio_set_ouput_val(D0_PIN,0);
            // hi_gpio_set_ouput_val(D1_PIN,1);
        }
        else{
            hi_io_set_func(D0_PIN, D0_GPIO_FUN);
            hi_io_set_func(D1_PIN, D1_GPIO_FUN);
            hi_gpio_set_ouput_val(D0_PIN,0);
            hi_gpio_set_ouput_val(D1_PIN,0);
        }
        
    }
    else if(motor == MOTOR_2){
        if(dir == MOTOR_FORWARD){
            hi_io_set_func(D2_PIN, D2_PWM_FUN);
            hi_io_set_func(D3_PIN, D3_GPIO_FUN);
            
            // hi_gpio_set_ouput_val(D2_PIN,1);
            hi_gpio_set_ouput_val(D3_PIN,0);
        }
        else if(dir == MOTOR_REVERSE){
            hi_io_set_func(D2_PIN, D2_GPIO_FUN);
            hi_io_set_func(D3_PIN, D3_PWM_FUN);
            
            hi_gpio_set_ouput_val(D2_PIN,0);
            // hi_gpio_set_ouput_val(D3_PIN,1);
        }
        else{
            hi_io_set_func(D2_PIN, D2_GPIO_FUN);
            hi_io_set_func(D3_PIN, D3_GPIO_FUN);
            hi_gpio_set_ouput_val(D2_PIN,0);
            hi_gpio_set_ouput_val(D3_PIN,0);
        }
    }
    
}

static void set_motor_logic_val(motor_id motor,hi_s8 speed)
{
    motor_dir dir;
    if( speed > 0 ) {
        dir = MOTOR_FORWARD;
    }
    else if( speed == 0 ){
        dir = MOTOR_STOP;
    } 
    else{
        dir = MOTOR_REVERSE;
    }
    // 获取方向
    motor_dir cur_dir = get_motor_dir(motor);
    if( cur_dir == dir ){
        return;
    }
    // 换向前需将电机暂停
    printf("[info] motor %d cur dir :[%d] , dir :[%d]\r\n",motor, cur_dir,dir);
    stop_motor(motor);
    usleep(1000);
    // 设置速度方向（正反转），若方向切换，则会先暂停电机
	if( speed > 0 ) {
        if(motor == MOTOR_1){
            hi_io_set_func(D0_PIN, D0_GPIO_FUN);
            hi_io_set_func(D1_PIN, D1_GPIO_FUN);
            hi_gpio_set_ouput_val(D0_PIN,1);
            hi_gpio_set_ouput_val(D1_PIN,0);
        }
        else if(motor == MOTOR_2){
            hi_io_set_func(D2_PIN, D2_GPIO_FUN);
            hi_io_set_func(D3_PIN, D3_GPIO_FUN);
            hi_gpio_set_ouput_val(D2_PIN,1);
            hi_gpio_set_ouput_val(D3_PIN,0);
        }
        printf("[info] motor %d dir :[%d]\r\n",motor, MOTOR_FORWARD);
    }
    else if( speed == 0 ){
        set_motor_dir(motor, MOTOR_STOP);
        printf("[info] motor %d dir :[%d]\r\n",motor, MOTOR_STOP);
    } 
    else{ //speed < 0
        if(motor == MOTOR_1){
            hi_io_set_func(D0_PIN, D0_GPIO_FUN);
            hi_io_set_func(D1_PIN, D1_GPIO_FUN);
            hi_gpio_set_ouput_val(D0_PIN,0);
            hi_gpio_set_ouput_val(D1_PIN,1);
        }
        else if(motor == MOTOR_2){
            hi_io_set_func(D2_PIN, D2_GPIO_FUN);
            hi_io_set_func(D3_PIN, D3_GPIO_FUN);
            hi_gpio_set_ouput_val(D2_PIN,0);
            hi_gpio_set_ouput_val(D3_PIN,1);
        }
        printf("[info] motor %d dir :[%d]\r\n",motor, MOTOR_REVERSE);
    }
}

static void set_motor_speed(motor_id motor,hi_s8 speed)
{
    // 设置速度方向（正反转），若方向切换，则会先暂停电机
	if( speed > 0 ) {
        set_motor_dir(motor, MOTOR_FORWARD);
    }
    else if( speed == 0 ){
        set_motor_dir(motor, MOTOR_STOP);
    } 
    else{
        set_motor_dir(motor, MOTOR_REVERSE);
        speed = -speed; // 取相反数，变成正数
    }

    // 设置速度大小
    hi_u16 duty;
    duty = SPEED(speed);
    if(motor == MOTOR_1){
        hi_pwm_start(MOTOR1_PWM,duty,PRESCALER);
    }
    else if(motor == MOTOR_2){
        hi_pwm_start(MOTOR2_PWM,duty,PRESCALER);
    }
    else {
        printf("[error] motor id error:[%d]\r\n", motor);
        return;
    }
    printf("[info] motor %d pwm duty:[%u]\r\n",motor, duty);
}


#ifdef L298N
// L298N的控制函数

void L298N_init()
{
    hi_gpio_init();                                            // GPIO初始化

    // 左侧电机引脚初始化
    hi_io_set_pull(ENA_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(ENA_PIN, ENA_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(ENA_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_pwm_init(ENA_PWM);
    hi_pwm_start(ENA_PWM,DUTY_50,PRESCALER);          // 设置PWM的占空比和分频系数

    hi_io_set_pull(IN1_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(IN1_PIN, IN1_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(IN1_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式

    hi_io_set_pull(IN2_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(IN2_PIN, IN2_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(IN2_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式

    // 右侧电机引脚初始化
    hi_io_set_pull(ENA_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(ENB_PIN, ENB_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(ENB_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式
    hi_pwm_init(ENB_PWM);
    hi_pwm_start(ENB_PWM,DUTY_50,PRESCALER);          // 设置PWM的占空比和分频系数

    hi_io_set_pull(IN3_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(IN3_PIN, IN3_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(IN3_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式

    hi_io_set_pull(IN4_PIN, HI_IO_PULL_DOWN);                  // 设置GPIO下拉
    hi_io_set_func(IN4_PIN, IN4_GPIO_FUN);                     // 设置IO为GPIO功能
    hi_gpio_set_dir(IN4_PIN, HI_GPIO_DIR_OUT);                 // 设置GPIO为输出模式

    motor_stop();
}

void motor_stop(void)
{
    hi_gpio_set_ouput_val(IN1_PIN,0);
    hi_gpio_set_ouput_val(IN2_PIN,0);

    hi_gpio_set_ouput_val(IN3_PIN,0);
    hi_gpio_set_ouput_val(IN3_PIN,0);
}

void set_left_motor_speed(hi_s8 speed)
{
    // 设置速度方向（正反转）
	if( speed > 0 ) {
        hi_gpio_set_ouput_val(IN1_PIN,0);
        hi_gpio_set_ouput_val(IN2_PIN,1);
    }
    else if( speed == 0 ){
        hi_gpio_set_ouput_val(IN1_PIN,0);
        hi_gpio_set_ouput_val(IN2_PIN,0);
    } else{
        hi_gpio_set_ouput_val(IN1_PIN,1);
        hi_gpio_set_ouput_val(IN2_PIN,0);
        speed = -speed; // 取相反数，变成正数
    }

    // 设置速度大小
    hi_u16 duty;
    duty = SPEED(speed) ;

    printf("[info] pwm duty:[%u]\r\n", duty);
    hi_pwm_start(ENA_PWM,duty,PRESCALER);

}

void set_right_motor_speed(hi_s8 speed)
{
    // 设置速度方向（正反转）
	if( speed > 0 ) {
        hi_gpio_set_ouput_val(IN3_PIN,0);
        hi_gpio_set_ouput_val(IN4_PIN,1);
    }
    else if( speed == 0 ){
        hi_gpio_set_ouput_val(IN3_PIN,0);
        hi_gpio_set_ouput_val(IN4_PIN,0);
    } else{
        hi_gpio_set_ouput_val(IN3_PIN,1);
        hi_gpio_set_ouput_val(IN4_PIN,0);
        speed = -speed; // 取相反数，变成正数
    }

    // 设置速度大小
    hi_u16 duty;
    duty = SPEED(speed) ;
    printf("[info] pwm duty:[%u]\r\n", duty);
    hi_pwm_start(ENB_PWM,duty,PRESCALER);
}
#endif



void move_Task1_create(void)
{
    osThreadAttr_t options;
    options.name = "move_Task1";       // 任务的名字
    options.attr_bits = 0;      // 属性位
    options.cb_mem = NULL;      // 堆空间地址
    options.cb_size = 0;        // 堆空间大小
    options.stack_mem = NULL;   // 栈空间地址
    options.stack_size = 1024;  // 栈空间大小 单位:字节
    options.priority = osPriorityNormal;  // 任务的优先级

    move_Task1_ID = osThreadNew((osThreadFunc_t)MOVE_Task, NULL, &options);      // 创建任务1
    if (move_Task1_ID != NULL) {
        printf("ID = %d, Create move_Task1_ID is OK!\r\n", move_Task1_ID);
    }
}