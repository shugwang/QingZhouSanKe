#ifndef __SHIP_CONFIG_H__
#define __SHIP_CONFIG_H__

// MQTT指令设计
typedef enum {
    DRIVE_ID=0,
    PUMP_ID,
    MQTT_PUB_ID,
    CMD_TASK_ID_MAX
}CMD_TASK_ID;

// MQTT 网络连接
#define WIFI_SSID "Xiami 13"
#define WIFI_PAWD "12345678"

#define SERVER_IP_ADDR "44.249.200.171"    //broker.emqx.io
#define SERVER_IP_PORT 1883

// IO引脚分配 基于ZK-5AD
// 螺旋桨驱动电机
#define MOTOR1_PWM         HI_PWM_PORT_PWM3

#define D0_PIN         HI_IO_NAME_GPIO_0
#define D0_GPIO_FUN    HI_IO_FUNC_GPIO_0_GPIO
#define D0_PWM_FUN     HI_IO_FUNC_GPIO_0_PWM3_OUT

#define D1_PIN         HI_IO_NAME_GPIO_6
#define D1_GPIO_FUN    HI_IO_FUNC_GPIO_6_GPIO
#define D1_PWM_FUN     HI_IO_FUNC_GPIO_6_PWM3_OUT

#define MOTOR2_PWM         HI_PWM_PORT_PWM2

#define D2_PIN         HI_IO_NAME_GPIO_2
#define D2_GPIO_FUN    HI_IO_FUNC_GPIO_2_GPIO
#define D2_PWM_FUN     HI_IO_FUNC_GPIO_2_PWM2_OUT

#define D3_PIN         HI_IO_NAME_GPIO_5
#define D3_GPIO_FUN    HI_IO_FUNC_GPIO_5_GPIO
#define D3_PWM_FUN     HI_IO_FUNC_GPIO_5_PWM2_OUT

// 水泵驱动电机
#define PUMP_IN1_PIN         HI_IO_NAME_GPIO_8
#define PUMP_IN1_GPIO_FUN    HI_IO_FUNC_GPIO_8_GPIO
#define PUMP_IN2_PIN         HI_IO_NAME_GPIO_11
#define PUMP_IN2_GPIO_FUN    HI_IO_FUNC_GPIO_11_GPIO


// IO引脚分配 基于L298N
// 螺旋桨驱动电机
#define ENA_PIN         HI_IO_NAME_GPIO_0
#define ENA_GPIO_FUN    HI_IO_FUNC_GPIO_0_PWM3_OUT
#define ENA_PWM         HI_PWM_PORT_PWM3
#define IN1_PIN         HI_IO_NAME_GPIO_7
#define IN1_GPIO_FUN    HI_IO_FUNC_GPIO_7_GPIO
#define IN2_PIN         HI_IO_NAME_GPIO_10
#define IN2_GPIO_FUN    HI_IO_FUNC_GPIO_10_GPIO

#define ENB_PIN         HI_IO_NAME_GPIO_1
#define ENB_GPIO_FUN    HI_IO_FUNC_GPIO_1_PWM4_OUT
#define ENB_PWM         HI_PWM_PORT_PWM4
#define IN3_PIN         HI_IO_NAME_GPIO_5
#define IN3_GPIO_FUN    HI_IO_FUNC_GPIO_5_GPIO
#define IN4_PIN         HI_IO_NAME_GPIO_6
#define IN4_GPIO_FUN    HI_IO_FUNC_GPIO_6_GPIO


#endif