#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;
const int LED_PIN_R = 4;
const int LED_PIN_G = 6;
volatile int flag_r = 0;
volatile int flag_g = 0;
volatile int timer_0 = 0;
volatile int timer_1 = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_callback_0(repeating_timer_t *rt){
    timer_0 = 1;
    return true;
}

bool timer_callback_1(repeating_timer_t *rt){
    timer_1 = 1;
    return true;
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int ms_0 = 500;
    int ms_1 = 250;
    repeating_timer_t timer_0;
    repeating_timer_t timer_1;
    bool pisca_0 = false;
    bool pisca_1 = false;

    if (!add_repeating_timer_ms(ms_0, timer_callback_0, NULL, &timer_0)){
        printf("Erro\n");
    }
    if (!add_repeating_timer_ms(ms_1, timer_callback_1, NULL, &timer_1)){
        printf("Erro\n");
    }

    while (true) {
        if (flag_r) {
            flag_r = 0; 
            pisca_0 = !pisca_0; 
            if (!pisca_0) {
                gpio_put(LED_PIN_R, 0);
            }
        }

        if (pisca_0 && timer_0) {
            timer_0 = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
        }

        if (flag_g) {
            flag_g = 0; 
            pisca_1 = !pisca_1; 
            if (!pisca_1) {
                gpio_put(LED_PIN_G, 0);
            }
        }

        if (pisca_1 && timer_1) {
            timer_1 = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G)); 
        }
    }
}