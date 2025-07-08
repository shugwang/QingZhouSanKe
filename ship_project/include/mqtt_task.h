#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "bsp_wifi.h"
#include "bsp_mqtt.h"

#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/api_shell.h"

#include "ship_config.h"


#define MQTT_TOPIC_SUB "subTopic"
#define MQTT_TOPIC_PUB "pubTopic"
#define TASK_INIT_TIME 1 // s


void mqtt_recv_task(void);

void mqtt_send_task(void);

void wifi_mqtt_send_Task3_create(void);
