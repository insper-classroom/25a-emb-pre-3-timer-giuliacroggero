#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;
volatile int flag_f_r = 0;
volatile int flag_r_r = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // Fall edge
        if (gpio == BTN_PIN_R)
            flag_f_r = 1;
    } else if (events == 0x8) { // Rise edge
        if (gpio == BTN_PIN_R)
            flag_r_r = 1;
    }
}

int main() {
    absolute_time_t start; 
    bool pressed = false; 
    const uint32_t LONG_PRESS_TIME_US = 500000;
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0;
            start = get_absolute_time();
            pressed = true;
        }else if (flag_r_r){
            flag_r_r = 0;
            if (pressed){
                uint32_t duracao = absolute_time_diff_us(start, get_absolute_time());
                if (duracao >= LONG_PRESS_TIME_US){
                    gpio_put(LED_PIN_R, (!gpio_get(LED_PIN_R)));
                }
            }
        }
    }
}