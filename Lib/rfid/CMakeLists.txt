set(pre_req nvs_flash driver)
idf_component_register(SRCS "rfid_rc522.c" "spi.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})
                    