#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int timer = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = 1;
    } else if (events == 0x8) { // rise edge
    }
}

bool timer_callback(repeating_timer_t *rt){
    timer = 1;
    return true;
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    int timer_ms = 500; 
    repeating_timer_t timer;
    bool piscaled = false; 

    if (!add_repeating_timer_ms(timer_ms, timer_callback, NULL, &timer)){
        printf("Erro para adicionar o timer\n");
    }
    while (true) {
        if (flag_f_r) { 
            flag_f_r = 0; 
            piscaled = !piscaled; 
            if (!piscaled) {
                gpio_put(LED_PIN_R, 0);
            }
        }
        if (piscaled && timer) {
            timer = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
        }
    }
}