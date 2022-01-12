#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "menus.h"
#include "buttons.h"
#include "spi_sdcard.h"
#include "sys_constants.h"

typedef struct {
    //QueueHandle_t *queue; 
    void (*up_handler)();
    void (*left_handler)(); 
    void (*down_handler)();
    void (*right_handler)();
    void (*select_handler)(); 
    void (*start_handler)(); 
    void (*a_handler)(); 
    void (*b_handler)();
}driver_param;

QueueHandle_t button_queue;
TaskHandle_t mm_task;
driver_param *main_params;

void main_button_driver(driver_param *mbd_params){
    uint32_t io_num;
    while(1){
        if(xQueueReceive(button_queue, &io_num, portMAX_DELAY)){
            switch(io_num){
                case UP_B:
                    mbd_params->up_handler();
                    break;
                case LEFT_B:
                    mbd_params->left_handler();
                    break;
                case DOWN_B:
                    mbd_params->down_handler();
                    break;
                case RIGHT_B:
                    mbd_params->right_handler();
                    break;
                case SELECT_B:
                    mbd_params->select_handler();
                    break;
                case START_B:
                    mbd_params->start_handler();
                    break;
                case A_B:
                    mbd_params->a_handler();
                    break;
                case B_B:
                    mbd_params->b_handler();
                    break;
            }
        }
    }
}


void app_main(void){
    button_queue = xQueueCreate(2, sizeof(uint32_t));
    main_params = malloc(sizeof(driver_param));
    main_params->up_handler = menu_up_handler;
    main_params->left_handler = menu_left_handler;
    main_params->down_handler = menu_down_handler;
    main_params->right_handler = menu_right_handler;
    main_params->select_handler = menu_select_handler;
    main_params->start_handler = menu_start_handler;
    main_params->a_handler = menu_a_handler;
    main_params->b_handler = menu_b_handler;
    sd_init();
    check_new_init();
    displays_startup(background);
    default_buttons_init(&button_queue);
    xTaskCreate(main_button_driver, "Main Menu Button Task", 10000, (void *)main_params, 10, &mm_task);
    vTaskSuspend(mm_task);
    vTaskResume(mm_task);
}

