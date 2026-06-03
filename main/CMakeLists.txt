idf_component_register(SRCS "ble_func.c" "tasks.c" "main.c" "twai.c" "ble_func.c" "utils.c"
                    INCLUDE_DIRS "." "headers"
                    REQUIRES bt nvs_flash driver)