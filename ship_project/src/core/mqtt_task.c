
#include "mqtt_task.h"
#include "cmd_fifo.h"

#ifdef CMD_FIFO_QD
extern CMD_FIFO CMD_Q[CONSUMERS_NUM];
#else
extern osMessageQueueId_t MsgQueue_ID2;
#endif

osThreadId_t mqtt_send_Task3_id;   // mqtt订阅数据任务
osThreadId_t mqtt_recv_Task4_id;   // mqtt发布数据任务

extern int8_t mqtt_sub_payload_callback(unsigned char *topic, unsigned char *payload);

#include "bsp_sensors.h"
extern sensors_data data;
char sensors_data_pub_buff[200];

void mqtt_recv_task(void)
{

    while (1) 
    {
        MQTTClient_sub();
        usleep(RECV_TASK_GAP);
    }
}

void mqtt_send_task(void)
{
    uint8_t res=0;
    // 连接Wifi
    if (WiFi_connectHotspots(WIFI_SSID, WIFI_PAWD) != WIFI_SUCCESS) 
    {
        printf("[error] WiFi_connectHotspots\r\n");
    } 
    else 
    {
        printf("[success] WiFi_connectHotspots\r\n");
    }
    // 连接MQTT服务器
    if (MQTTClient_connectServer(SERVER_IP_ADDR, SERVER_IP_PORT) != 0) 
    {
        printf("[error] MQTTClient_connectServer: %s:%d \r\n",SERVER_IP_ADDR,SERVER_IP_PORT);
    } 
    else 
    {
        printf("[success] MQTTClient_connectServer %s:%d \r\n",SERVER_IP_ADDR,SERVER_IP_PORT);
    }
    sleep(TASK_INIT_TIME);

    // 初始化MQTT客户端
    if (MQTTClient_init("mqtt_client_123",USERNAME , PASSWORD) != 0) 
    {
        printf("[error] MQTTClient_init\r\n");
    } 
    else 
    {
        printf("[success] MQTTClient_init\r\n");
    }
    sleep(TASK_INIT_TIME);

    // 订阅Topic
    if (MQTTClient_subscribe(MQTT_TOPIC_SUB) != 0) 
    {
        printf("[error] MQTTClient_subscribe\r\n");
    } 
    else 
    {
        printf("[success] MQTTClient_subscribe\r\n");
    }
    sleep(TASK_INIT_TIME); 

    p_MQTTClient_sub_callback = &mqtt_sub_payload_callback;
    osThreadAttr_t options;
    options.name = "mqtt_recv_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = 1024*10;
    options.priority = osPriorityAboveNormal;

    mqtt_recv_Task4_id = osThreadNew((osThreadFunc_t)mqtt_recv_task, NULL, &options);
    if (mqtt_recv_Task4_id != NULL) 
    {
        printf("ID = %d, Create mqtt_recv_Task4_id is OK!\r\n", mqtt_recv_Task4_id);
    } 
    else
    {
        printf(" Create mqtt_recv_Task4_id is failed!\r\n");
    }


    while (1) 
    {
        COMMAND cmd;
        #ifdef CMD_FIFO_QD
        CMD_FIFO * fifo_p;
        fifo_p = &CMD_Q[MQTT_PUB_ID];
        if( CMD_FIFO_POP(fifo_p, &cmd) ){
        #else
        if( osMessageQueueGet(MsgQueue_ID2, &cmd, 0, MESSAGE_TIMEOUT) == osOK ){
        #endif
            printf("mqtt_pub_task pop cmd: %x %x %x %x \r\n",cmd.task_id, cmd.func_id, cmd.data_1, cmd.data_2 );
            if( cmd.func_id == 0x01){ //上传传感器信息
                int ret = snprintf(sensors_data_pub_buff, sizeof(sensors_data_pub_buff),
                    "{\"pH\":%.3f,\"oxygen\":%d,\"turbidity\":%d,\"temperature\":%.3f,\"conductivity\":%.3f}",
                    data.pH, 0, 0, data.temper, data.tds);
                
                if (ret < 0) {
                    // 格式化错误
                    printf("MQTT_TOPIC_PUB snprintf error.");
                }else{
                    MQTTClient_pub(MQTT_TOPIC_PUB, (unsigned char *)sensors_data_pub_buff, ret);
                }
                
            }
        }
        int ret = snprintf(sensors_data_pub_buff, sizeof(sensors_data_pub_buff),
            "{\"x\":%d,\"y\":%d,\"z\":%d}",
            data.x, data.y, data.z);
        
        if (ret < 0) {
            // 格式化错误
            printf("MQTT_TOPIC_PUB snprintf error.");
        }else{
            MQTTClient_pub("navigationTopic", (unsigned char *)sensors_data_pub_buff, ret);
        }
        usleep(SEND_TASK_GAP);
    }
}

//任务创建
void wifi_mqtt_send_Task3_create(void)
{
    osThreadAttr_t taskOptions;
    
    taskOptions.name = "mqttTask";       // 任务的名字
    taskOptions.attr_bits = 0;               // 属性位
    taskOptions.cb_mem = NULL;               // 堆空间地址
    taskOptions.cb_size = 0;                 // 堆空间大小
    taskOptions.stack_mem = NULL;            // 栈空间地址
    taskOptions.stack_size = 1024*5;           // 栈空间大小 单位:字节
    taskOptions.priority = osPriorityAboveNormal; // 任务的优先级

    mqtt_send_Task3_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &taskOptions); // 创建任务
    if (mqtt_send_Task3_id != NULL)
    {
        printf("ID = %d, mqtt_send_Task3_id Create OK!\r\n", mqtt_send_Task3_id);
    } else {
        printf("mqtt_send_Task3_id Create Failed!\r\n");
    }
}