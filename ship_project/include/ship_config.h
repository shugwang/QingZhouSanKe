#ifndef __SHIP_CONFIG_H__
#define __SHIP_CONFIG_H__

// 任务执行间隔
#define DRIVE_TASK_GAP          200*1000 //us
#define PUMP_TASK_GAP           5*1000*1000 //us
#define SEND_TASK_GAP           1000*1000 //us
#define RECV_TASK_GAP           200*1000 //us
#define SENSOR_TASK_GAP         1000*1000 //us

// MQTT指令设计
typedef enum {
    DRIVE_ID=0,
    PUMP_ID,
    MQTT_PUB_ID,
    
    CMD_TASK_ID_MAX
}CMD_TASK_ID;

#define MQON0          16              //MsgQueueObjectNumber
#define MQON1          4
#define MQON2          4
#define MESSAGE_TIMEOUT 100

// MQTT 网络连接
#define WIFI_SSID "Xiami 13"
#define WIFI_PAWD "12345678"

#define SERVER_IP_ADDR "47.111.93.229"    //broker.emqx.io
#define SERVER_IP_PORT 1883
#define USERNAME "123"
#define PASSWORD "123"

// 传感器引脚分配
#define pH_PIN HI_IO_NAME_GPIO_9            // pH计模拟输入引脚(根据实际接线调整) //黑
#define pH_OFFSET 0.00f                      // 校准偏移量, ≤0.3
#define pH_ADC_CHANNEL  HI_ADC_CHANNEL_4

#define tds_PIN HI_IO_NAME_GPIO_4            // tds计模拟输入引脚(根据实际接线调整)
#define tds_ADC_CHANNEL  HI_ADC_CHANNEL_1
#define kValue      1.0f                    // 修正系数,kValue=TDS_standard/TDS_messure

#define temper_PIN    HI_IO_NAME_GPIO_7
#define temper_GPIO_FUN   HI_IO_FUNC_GPIO_7_GPIO


// GPS 
#define GPS_TX_PIN            HI_IO_NAME_GPIO_11
#define GPS_TX_GPIO_FUN       HI_IO_FUNC_GPIO_11_UART2_TXD
#define GPS_RX_PIN            HI_IO_NAME_GPIO_12
#define GPS_RX_GPIO_FUN       HI_IO_FUNC_GPIO_12_UART2_RXD
#define GPS_UART_IDX          HI_UART_IDX_2

//HMC 5833
#define HMC_SDA_PIN             HI_IO_NAME_GPIO_13
#define HMC_SDA_GPIO_FUN        HI_IO_FUNC_GPIO_13_I2C0_SDA
#define HMC_SCL_PIN             HI_IO_NAME_GPIO_14
#define HMC_SCL_GPIO_FUN        HI_IO_FUNC_GPIO_14_I2C0_SCL

// IO引脚分配 基于ZK-5AD
// 螺旋桨驱动电机
#define MOTOR1_PWM         HI_PWM_PORT_PWM3

#define D0_PIN         HI_IO_NAME_GPIO_0                //红
#define D0_GPIO_FUN    HI_IO_FUNC_GPIO_0_GPIO       
#define D0_PWM_FUN     HI_IO_FUNC_GPIO_0_PWM3_OUT

#define D1_PIN         HI_IO_NAME_GPIO_6                //棕
#define D1_GPIO_FUN    HI_IO_FUNC_GPIO_6_GPIO
#define D1_PWM_FUN     HI_IO_FUNC_GPIO_6_PWM3_OUT

#define MOTOR2_PWM         HI_PWM_PORT_PWM2

#define D2_PIN         HI_IO_NAME_GPIO_2                //橙
#define D2_GPIO_FUN    HI_IO_FUNC_GPIO_2_GPIO
#define D2_PWM_FUN     HI_IO_FUNC_GPIO_2_PWM2_OUT

#define D3_PIN         HI_IO_NAME_GPIO_5                //黄
#define D3_GPIO_FUN    HI_IO_FUNC_GPIO_5_GPIO
#define D3_PWM_FUN     HI_IO_FUNC_GPIO_5_PWM2_OUT

// 水泵驱动电机
#define PUMP_IN1_PIN         HI_IO_NAME_GPIO_8          //绿
#define PUMP_IN1_GPIO_FUN    HI_IO_FUNC_GPIO_8_GPIO
#define PUMP_IN2_PIN         HI_IO_NAME_GPIO_10         //蓝
#define PUMP_IN2_GPIO_FUN    HI_IO_FUNC_GPIO_10_GPIO


#ifdef L298N
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

#endif