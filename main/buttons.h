#ifndef BUTTONSH_H
#define BUTTONSH_H

#include "freertos/queue.h"

#define GPIO_INPUT_PIN_SEL ((1ULL<<UP_B) | (1ULL<<LEFT_B) | (1ULL<<DOWN_B) | (1ULL<<RIGHT_B) | (1ULL<<SELECT_B) | (1ULL<<START_B) | (1ULL<<A_B) | (1ULL<<B_B))
#define ESP_INTR_FLAG_DEFAULT 0

void set_button_queue(QueueHandle_t *button_queue);

void default_buttons_init(QueueHandle_t  *button_queue);

#endif