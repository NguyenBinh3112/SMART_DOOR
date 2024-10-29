#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <driver/gpio.h>
#include <driver/i2c.h>
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#include <spi.h>
#include <lcd.h>
#include <rfid_rc522.h>
#include <keypad.h>

// #define RELAY_SOURCE_PIN GPIO_NUM_15
#define RELAY_PIN        GPIO_NUM_2

#define MISO_PIN GPIO_NUM_19
#define MOSI_PIN GPIO_NUM_23
#define CLK_PIN  GPIO_NUM_18
#define CS_PIN   GPIO_NUM_5
#define RST_PIN  GPIO_NUM_15


TaskHandle_t lcd_password;
TaskHandle_t rfid;
TaskHandle_t Button_C_Handle;

typedef enum{
    LOCK_STATE,
    ENTER_PASS_STATE,
    OPEN_STATE,
    WAIT_STATE,
    WRONG_STATE,
    CHANGE_PASS_STATE,
    ENTER_NEW_PASS
}keyboard_state;

keyboard_state keyboard_sta = LOCK_STATE;

int64_t time_wrong;
int64_t time_true;

char pass[6] = "123456";
char buff[10];
char user_enter_password[10];
uint8_t dem = 0;
uint8_t pass_wrong_time = 0;


char buff_change_pass[10];
char user_pass_change[10];
uint8_t indexx=0;
int64_t change_time;

void whistle_run()
{
    static int x = 1;
    gpio_set_level(16,x);
    vTaskDelay(500/portTICK_RATE_MS);
    x = 1-x;
}

void whistle_stop()
{
    gpio_set_level(16,0);
}

void KeypadPressingCallback(uint8_t key)
{
    if(key == 'C')
    {
        lcd_clear();
        keyboard_sta = CHANGE_PASS_STATE;
    }
    switch (keyboard_sta)
    {
    case ENTER_NEW_PASS:
        if(key>='0'&&key<= '9')
        {
            if (dem < sizeof(buff_change_pass) - 1)  // Giới hạn kích thước buffer
            {
                buff_change_pass[dem++] = key;
                lcd_put_cur(1,dem+6-1);
                lcd_send_data('*');        
                printf("%c\n", buff_change_pass[dem - 1]);
            }
            else
            {
                printf("Buffer full\n");
            }
        }
        else if(key == 'A')
        {
            for(int i=0;i<sizeof(buff_change_pass);i++)
            {
                buff_change_pass[i] = NULL;
            }
            dem =0;
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("change pass");
            lcd_put_cur(1,0);
            lcd_send_string("NEW:");             
        }
        else if(key == 'B')
        {
            buff[dem-1] = '\0';
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("change pass");
            lcd_put_cur(1,0);
            lcd_send_string("NEW:"); 
            for(int i=0;i<dem-1;i++)
            {
                lcd_put_cur(1,i+6);
                lcd_send_data('*');            
            }
            dem--;            
        }
        else if(key == '#')
        {
            for(int i=0;i<dem-1;i++)
            {   
                strcpy(pass+i,&buff_change_pass[i]);
                printf("%s\n",pass);
            }
            keyboard_sta = LOCK_STATE;
            lcd_clear();
            lcd_put_cur(0,5);
            lcd_send_string("LOCK");
            lcd_put_cur(1,0);
            lcd_send_string("press * to open"); 
            for(int i=0;i<sizeof(buff);i++)
            {
                buff_change_pass[i] = 0;
            // printf("%d\n",buff[0]);
            }
            dem =0;              
        }        
    break;    
    case CHANGE_PASS_STATE:
        lcd_put_cur(0,0);
        lcd_send_string("change pass");
        lcd_put_cur(1,0);
        lcd_send_string("OLD:");
        if(key>='0'&&key<= '9')
        {
            if (dem < sizeof(buff) - 1)  // Giới hạn kích thước buffer
            {
                buff[dem++] = key;
                lcd_put_cur(1,dem+6-1);
                lcd_send_data('*');        
                printf("%c\n", buff[dem - 1]);
            }
            else
            {
                printf("Buffer full\n");
            }
        }
        else if(key == 'A')
        {
            for(int i=0;i<sizeof(buff);i++)
            {
                buff[i] = NULL;
            }
            dem =0;
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("change pass");
            lcd_put_cur(1,0);
            lcd_send_string("OLD:");             
        }
        else if(key == 'B')
        {
            buff[dem-1] = '\0';
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("change pass");
            lcd_put_cur(1,0);
            lcd_send_string("OLD:"); 
            for(int i=0;i<dem-1;i++)
            {
                lcd_put_cur(1,i+6);
                lcd_send_data('*');            
            }
            dem--;            
        }
        else if(key == '#')
        {
            for(int i=0;i<dem-1;i++)
            {   
                strcpy(user_enter_password+i,&buff[i]);
            }
            // sprintf(user_enter_password,"%c%c",buff[0],buff[1]);        
            printf("%s\n",user_enter_password);
            // char *p = strstr(pass, user_enter_password);
            if(strcmp(user_enter_password,pass) == 0)
            {
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("change pass");
                lcd_put_cur(1,0);
                lcd_send_string("NEW:");
                keyboard_sta = ENTER_NEW_PASS;
                for(int i=0;i<sizeof(buff);i++)
                {
                    buff[i] = 0;
                    // printf("%d\n",buff[0]);
                }
                for(int i=0;i<sizeof(user_enter_password);i++)
                {
                    user_enter_password[i] = 0;
                }
                dem =0;
            }
            else
            {
                keyboard_sta = LOCK_STATE;
                lcd_clear();
                lcd_put_cur(0,5);
                lcd_send_string("LOCK");
                lcd_put_cur(1,0);
                lcd_send_string("press * to open");  
                for(int i=0;i<sizeof(buff);i++)
                {
                    buff[i] = 0;
                    // printf("%d\n",buff[0]);
                }
                dem = 0;               
            }
        }

        break;    
    case LOCK_STATE:
        if(key == '*')
        {
            keyboard_sta = ENTER_PASS_STATE;
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("enter password");
            lcd_put_cur(1,0);
            lcd_send_string("pass:");
        }
        else if(key == 'C')
        {
            keyboard_sta = CHANGE_PASS_STATE;
        }
        break;    
    case ENTER_PASS_STATE:

        if(key == 'A')
        {
            for(int i=0;i<sizeof(buff);i++)
            {
                buff[i] = NULL;
            }
            dem =0;
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("enter password");
            lcd_put_cur(1,0);
            lcd_send_string("pass:");            
        }
        else if(key == 'B')
        {
            buff[dem-1] = '\0';
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("enter password");
            lcd_put_cur(1,0);
            lcd_send_string("pass:");
            for(int i=0;i<dem-1;i++)
            {
                lcd_put_cur(1,i+6);
                lcd_send_data('*');            
            }
            dem--;

        }
        else if(key == 'C')
        {
            keyboard_sta = CHANGE_PASS_STATE;
        }
        else if(key == '#')
        {
            for(int i=0;i<dem-1;i++)
            {   
                strcpy(user_enter_password+i,&buff[i]);
            }
            // sprintf(user_enter_password,"%c%c",buff[0],buff[1]);        
            printf("%s\n",user_enter_password);
            // char *p = strstr(pass, user_enter_password);
            if(strcmp(user_enter_password,pass) == 0)
            {
                printf("correct pass word\n");
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("correct password"); 
                lcd_put_cur(1,5);
                lcd_send_string("OPEN");
                pass_wrong_time = 0;            
                time_true = esp_timer_get_time(); 
                for(int i=0;i<sizeof(buff);i++)
                {
                    buff[i] = 0;
                    // printf("%d\n",buff[0]);
                }
                for(int i=0;i<sizeof(user_enter_password);i++)
                {
                    user_enter_password[i] = 0;
                }
                dem =0;  
                keyboard_sta = OPEN_STATE;
                gpio_set_level(RELAY_PIN,1);                
                // memset(buff,0,10);
                          
            }
            else{
                if(pass_wrong_time < 3)
                {
                    pass_wrong_time ++;
                }
                else 
                {
                    printf("doi 5s\n");
                    keyboard_sta = WAIT_STATE;
                    time_wrong = esp_timer_get_time();
                }                
                printf("wrong pass\n");
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("wrong pass: ");
                lcd_put_cur(0, 14);
                lcd_send_data(pass_wrong_time+48);
                lcd_put_cur(1,0);
                lcd_send_string("pass:");
            }
            for(int i=0;i<sizeof(buff);i++)
            {
                buff[i] = 0;
            }
            dem =0;
        }
        else
        {
            if (dem < sizeof(buff) - 1)  // Giới hạn kích thước buffer
            {
                buff[dem++] = key;
                lcd_put_cur(1,dem+6-1);
                lcd_send_data('*');        
                printf("%c\n", buff[dem - 1]);
            }
            else
            {
                printf("Buffer full\n");
            }
        }
        break;
    
    default:
        break;
    }
    

}


void state_handle_keyboard()
{
    switch (keyboard_sta)
    {
    case WAIT_STATE:
    whistle_run();
    lcd_clear();
    lcd_put_cur(0,0);
    lcd_send_string("wrong pass >3");
    lcd_put_cur(1,0);
    lcd_send_string("wait 5s");        
    if(esp_timer_get_time()-time_wrong >= 5000000)
    {
     
        keyboard_sta = LOCK_STATE;        
        whistle_stop();
        lcd_clear();
        lcd_put_cur(0,5);
        lcd_send_string("LOCK");
        lcd_put_cur(1,0);
        lcd_send_string("press * to open");           
    }
        break;
    case OPEN_STATE:
    whistle_run();
    if(esp_timer_get_time()-time_true >= 5000000)
    {
        lcd_clear();
        lcd_put_cur(0,5);
        lcd_send_string("LOCK");
        lcd_put_cur(1,0);
        lcd_send_string("press * to open");
        keyboard_sta = LOCK_STATE;
        gpio_set_level(RELAY_PIN,0);
        printf("lock\n");
        whistle_stop();
    }
        break;
    case WRONG_STATE:
        printf("wrong\n");
        vTaskDelay(1000/portTICK_RATE_MS); 
        keyboard_sta = LOCK_STATE;      
        break;
    case CHANGE_PASS_STATE:
        break;        
    default:
        break;
    }
}



static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void whistle_init()
{
    gpio_pad_select_gpio(16);
    gpio_set_direction(16, GPIO_MODE_OUTPUT);
}

void Lcd_Enter_PassWord(void *parameter)
{
    while(1)
    { 
        Keypad_Handle();
        state_handle_keyboard();
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}
uint8_t uid[5];

void MRFID(void *parameter)
{
    while(1)
    {
        static uint8_t count = 0;
        MFRC522_Check(uid);
        switch(count)
        {
            case 0:
            if(uid[0] == 0xA3 && uid[1] == 0xC9 && uid[2] == 0x60 && uid[3] == 0x19)
            {
                vTaskSuspend(lcd_password);
                keyboard_sta = OPEN_STATE;
                gpio_set_level(RELAY_PIN,1);
                time_true = esp_timer_get_time();
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("correct pass");
                lcd_put_cur(1,5);
                lcd_send_string("OPEN");
                pass_wrong_time = 0; 
                count ++;            
            }
            else if(uid[0]!=0&&uid[0]!=0xA3&&uid[1]!=0&&uid[1]!=0xC9&&uid[2]!=0&&uid[2]!=0x60&&uid[3]!=0&&uid[3]!=0x19)
            {
                vTaskSuspend(lcd_password);
                if(pass_wrong_time < 3)
                {
                    pass_wrong_time ++;
                    keyboard_sta = WRONG_STATE;
                }
                else 
                {
                    printf("doi 5s\n");
                    keyboard_sta = WAIT_STATE;
                    time_wrong = esp_timer_get_time();
                }
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("wrong pass: ");
                lcd_put_cur(0, 14);
                lcd_send_data(pass_wrong_time+48);
                lcd_put_cur(1,5);
                lcd_send_string("LOCK"); 
                count ++;            
            }
                break;
            case 1:
                    state_handle_keyboard();
                    count --;
            break;    
        }
        vTaskResume(lcd_password);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main()
{
    keypad_init();
    i2c_master_init();
    lcd_init();
    gpio_pad_select_gpio(15);
    gpio_set_direction(15, GPIO_MODE_OUTPUT);
    gpio_set_level(15,1);
    gpio_pad_select_gpio(2);
    gpio_set_direction(2, GPIO_MODE_OUTPUT);    
    whistle_init();
    lcd_put_cur(0,5);
    lcd_send_string("LOCK");
    lcd_put_cur(1,0);
    lcd_send_string("press * to open");
    spi_init(CLK_PIN, MOSI_PIN, MISO_PIN);
    MFRC522_Init(RST_PIN, CS_PIN);
    xTaskCreate(Lcd_Enter_PassWord, "lcd_enter_pass_word",4096, NULL, 2, &lcd_password);
    xTaskCreate(MRFID, "MRFID",4096, NULL, 3, &rfid);
}