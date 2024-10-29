#ifndef __OUTPUT_IOT_H_
#define __OUTPUT_IOT_H_

#include "esp_err.h"
#include "hal/gpio_types.h"


typedef enum{
    LOW = 0,
    HIGH = 1,
}type_output;



void output_io_create(gpio_num_t gpio_num);
void output_set_level(gpio_num_t gpio_num, type_output type );
void output_toggle_pin(gpio_num_t gpio_num);

#endif