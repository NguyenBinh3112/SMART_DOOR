#include <stdio.h>
#include "driver/gpio.h"
#include "input_iot_no_count.h"
#include "output_iot.h"
#include "keypad.h"
#include "esp_timer.h"

uint8_t key_current;
uint8_t key_last = 0;
uint8_t key_debounce = 0;
uint8_t debouncing =0;
uint32_t t_debounce;
uint32_t t_start_press;
uint8_t is_press;


uint8_t ROW[4] ={13,12,14,27};
uint8_t COL[4] ={26,25,33,32};

uint8_t keypad[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void __attribute__((weak)) KeypadPressingCallback(uint8_t key)
{

}
void __attribute__((weak)) KeypadPressedShortCallback(uint8_t key)
{

}
void __attribute__((weak)) KeypadReleaseCallback(uint8_t key)
{

}

void __attribute__((weak)) KeypadPressingTimeoutCallback(uint8_t key)
{

}


void select_row(uint8_t row)
{
    gpio_set_level(ROW[row],0);
}

void unSelect_row(void)
{
    for(int i=0;i<4;i++)
    {
        gpio_set_level(ROW[i],1);
    }
}

uint8_t GetKey()
{
    for(int i=0;i<4;i++)
    {
        unSelect_row();
        select_row(i);
        for(int j=0;j<4;j++)
        {
            if(gpio_get_level(COL[j])==0)
            {
                return keypad[i][j];
            }
        }
    }
    return '\0';
}

void Keypad_Filter()
{
	uint8_t key = GetKey();
	//khi van dang nhieu
	if(key != key_debounce)
	{
		debouncing = 1;
		t_debounce = esp_timer_get_time();
		key_debounce = key;
	}
	//trang thai da xac lap
	if(debouncing && (esp_timer_get_time() - t_debounce >= 15000))
	{
		key_current = key_debounce;
		debouncing = 0;
	}
}

void Keypad_Handle()
{
	Keypad_Filter();

	if(key_current != key_last)
	{

		if(key_current != 0)
		{
			is_press = 1;
			t_start_press = esp_timer_get_time();
			KeypadPressingCallback(key_current);
		}
		else
		{
			is_press = 0;
			if(esp_timer_get_time() - t_start_press <=1000000)//nhan nha nhanh
			{
				KeypadPressedShortCallback(key_last);
			}
			KeypadReleaseCallback(key_last);
		}
		key_last = key_current;
	}

	if(is_press && (esp_timer_get_time() - t_start_press >= 1500000)) //nhan giu phim
	{
		KeypadPressingTimeoutCallback(key_current);
		is_press = 0;
	}
}

void keypad_init()
{
    gpio_pad_select_gpio(COL[0]);
    gpio_set_direction(COL[0],GPIO_MODE_INPUT);
    gpio_set_pull_mode(COL[0],GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(COL[1]);
    gpio_set_direction(COL[1],GPIO_MODE_INPUT);
    gpio_set_pull_mode(COL[1],GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(COL[2]);
    gpio_set_direction(COL[2],GPIO_MODE_INPUT);
    gpio_set_pull_mode(COL[2],GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(COL[3]);
    gpio_set_direction(COL[3],GPIO_MODE_INPUT);
    gpio_set_pull_mode(COL[3],GPIO_PULLUP_ONLY);
    ////--------------------------------------------------------
    gpio_pad_select_gpio(ROW[0]);
    gpio_set_direction(ROW[0],GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ROW[1]);
    gpio_set_direction(ROW[1],GPIO_MODE_OUTPUT);;

    gpio_pad_select_gpio(ROW[2]);
    gpio_set_direction(ROW[2],GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ROW[3]);
    gpio_set_direction(ROW[3],GPIO_MODE_OUTPUT); 
}

