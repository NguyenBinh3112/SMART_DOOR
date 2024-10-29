#ifndef __LCD_H_
#define __LCD_H_

#include "esp_err.h"

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_go_right(int times);

void lcd_go_left(int times);


//esp_err_t i2c_master_write_to_device(int i2c_num,uint8_t Address,uint8_t* data,int leng, int time);

#endif