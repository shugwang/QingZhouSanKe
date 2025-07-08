#include "hi_io.h"
#include "hi_gpio.h"

#include "hi_adc.h"

#include "ship_config.h"

typedef struct 
{
    float pH;        // PH 6.5-8.5
    float temper;         // temp 0-50°
    float tds;          // 0-500ppm,parts per million百万分比浓度 

    int16_t x;
    int16_t y;
    int16_t z;
}sensors_data;

void Sensors_Task5(void);
void Sensors_Task5_create(void);

// adc
float get_adc_voltage(hi_adc_channel_index adc_channel);
uint16_t get_adc_value(hi_adc_channel_index adc_channel);

//temperature


// ph 
float get_pH_value(void);
void pH_init(void);

//tds
void tds_init(void);
float get_tds_value(float temper, float K);