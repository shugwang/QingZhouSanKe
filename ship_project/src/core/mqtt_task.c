
#include "mqtt_task.h"
#include "cmd_fifo.h"

osThreadId_t mqtt_send_Task3_id;   // mqtt订阅数据任务
osThreadId_t mqtt_recv_Task4_id;   // mqtt发布数据任务

extern int8_t mqtt_sub_payload_callback(unsigned char *topic, unsigned char *payload);

void mqtt_recv_task(void)
{

    while (1) 
    {
        MQTTClient_sub();
        usleep(MQTT_RECV_TASK_TIME);
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
        printf("[success] MQTTClient_connectServer\r\n");
    }
    sleep(TASK_INIT_TIME);

    // 初始化MQTT客户端
    if (MQTTClient_init("mqtt_client_123", "username", "password") != 0) 
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
    options.stack_size = 1024*5;
    options.priority = osPriorityNormal;

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
        CMD_FIFO * fifo_p;
        fifo_p = &CMD_Q[MQTT_PUB_ID];
        if( CMD_FIFO_POP(fifo_p, &cmd) ){
            printf("mqtt_pub_task pop cmd: %x %x %x %x \r\n",cmd.task_id, cmd.func_id, cmd.data_1, cmd.data_2 );
            if( cmd.func_id == 0x01){ //上传传感器信息
                MQTTClient_pub(MQTT_TOPIC_PUB, "hello world!!!", strlen("hello world!!!"));
            }
        }
        usleep(1000*1000);
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
    taskOptions.priority = osPriorityNormal; // 任务的优先级

    mqtt_send_Task3_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &taskOptions); // 创建任务
    if (mqtt_send_Task3_id != NULL)
    {
        printf("ID = %d, mqtt_send_Task3_id Create OK!\r\n", mqtt_send_Task3_id);
    } else {
        printf("mqtt_send_Task3_id Create Failed!\r\n");
    }
}