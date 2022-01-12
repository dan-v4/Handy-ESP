#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"
#include "sys_constants.h"
#include "buttons.h"

static void IRAM_ATTR up_isr_handler(void* arg);
static void IRAM_ATTR left_isr_handler(void* arg);
static void IRAM_ATTR down_isr_handler(void* arg);
static void IRAM_ATTR right_isr_handler(void* arg);
static void IRAM_ATTR select_isr_handler(void* arg);
static void IRAM_ATTR start_isr_handler(void* arg);
static void IRAM_ATTR a_isr_handler(void* arg);
static void IRAM_ATTR b_isr_handler(void* arg);

QueueHandle_t *gpio_evt_queue;
uint32_t DEBOUNCE_TIME = 20;

static void IRAM_ATTR up_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR left_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR down_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR right_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR select_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR start_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR a_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

static void IRAM_ATTR b_isr_handler(void* arg){
    static uint64_t previous;
    uint64_t current = xTaskGetTickCount();
    if ((current - previous) > DEBOUNCE_TIME) {
        previous = current;
        uint32_t gpio_num = (uint32_t) arg;
        xQueueSendFromISR(*gpio_evt_queue, &gpio_num, NULL);
    }
}

void set_button_queue(QueueHandle_t *button_queue){
    gpio_evt_queue = button_queue;
}


void default_buttons_init(QueueHandle_t  *button_queue){
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    set_button_queue(button_queue);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(UP_B, up_isr_handler, (void*) UP_B);
    gpio_isr_handler_add(LEFT_B, left_isr_handler, (void*) LEFT_B);
    gpio_isr_handler_add(DOWN_B, down_isr_handler, (void*) DOWN_B);
    gpio_isr_handler_add(RIGHT_B, right_isr_handler, (void*) RIGHT_B);
    gpio_isr_handler_add(SELECT_B, select_isr_handler, (void*) SELECT_B);
    gpio_isr_handler_add(START_B, start_isr_handler, (void*) START_B);
    gpio_isr_handler_add(A_B, a_isr_handler, (void*) A_B);
    gpio_isr_handler_add(B_B, b_isr_handler, (void*) B_B);
}


