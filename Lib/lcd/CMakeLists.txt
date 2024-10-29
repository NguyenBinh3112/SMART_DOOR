set(pri_req nvs_flash driver)

idf_component_register(SRCS "lcd.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})