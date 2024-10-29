#ifndef __INPUT_IOT_H_
#define __INPUT_IOT_H_

#include "esp_err.h"
#include "hal/gpio_types.h"



typedef void (*input_callback_t) (int);


typedef enum {
    GPIO_INTR_rising= 1,     /*!< GPIO interrupt type : rising edge                  */
    GPIO_INTR_falling = 2,     /*!< GPIO interrupt type : falling edge                 */
    GPIO_INTR_ANY = 3,     /*!< GPIO interrupt type : both rising and falling edge */
} type_interrupt;


void input_io_create_no_count(gpio_num_t gpio_num, type_interrupt type);
void input_get_level(gpio_num_t gpio_num );
// void input_set_input_callback(void *cb);
void input_set_callback_no_count(void *cb);
#endif