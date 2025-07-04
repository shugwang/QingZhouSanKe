#include "hi_io.h"
#include "hi_gpio.h"

typedef struct sensors_msg
{
    char *latstr;
    char *lonstr;
    hi_u8 battery;
    hi_u8 waterstate;
    double PH;
    hi_u8 temp;
    double turbidity;
    hi_u8 powerswitch;
    uint16_t tds;
    hi_u8 cpu_temp;
    hi_u8 temperature_DHT11;
    hi_u8 humidity_DHT11;
    double Pressure;
    double H;
    hi_u8 light;

};


typedef struct sensors_source
{
    double PH;        // PH 6.5-8.5
    int temp;         // temp 0-50°
    double turbidity; // 浊度 0-100NTU
    int tds;          // 0-500ppm
    int service;      // 0-100
    hi_u8 greap;
};

void sensors_data_Task(void);