idf_component_register(SRCS "utils.c" "ble_func.c" "tasks.c" "main.c" "twai.c" "ble_func.c"
                    INCLUDE_DIRS "." "headers"
                    REQUIRES bt nvs_flash driver)