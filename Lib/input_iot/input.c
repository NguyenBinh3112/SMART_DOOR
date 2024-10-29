#include "stdio.h"
#include "esp_log.h"
#include <driver/gpio.h>
#include "input_iot_no_count.h"
#include "rom/gpio.h"


// input_callback_t inputcall_back = NULL;

// static void IRAM_ATTR gpio_input_hanler(void *arg){
//           int gpio_num = (uint32_t) arg;
//           inputcall_back(gpio_num);
// }


void input_io_create_no_count(gpio_num_t gpio_num, type_interrupt type){
     gpio_pad_select_gpio(gpio_num);
     gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
     gpio_set_pull_mode(gpio_num,GPIO_PULLUP_ONLY);
    //  gpio_set_intr_type(gpio_num,type);
    //  gpio_install_isr_service(0);
    //  gpio_isr_handler_add(gpio_num,gpio_input_hanler,(void*) gpio_num);
     
}



// void input_set_callback_no_count(void *cb){
//    inputcall_back = cb;
// }

