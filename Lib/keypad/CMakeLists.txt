set(pre_req nvs_flash input_iot output_iot driver)
idf_component_register(SRCS "key_pad"
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})
                    