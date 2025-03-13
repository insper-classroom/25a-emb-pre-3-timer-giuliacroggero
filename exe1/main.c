#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile bool piscaled = false; 
volatile bool active_timer = false;

bool timer_callback(repeating_timer_t *rt) {
    active_timer = true;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        piscaled = !piscaled;
    }
}

int main() {

    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    repeating_timer_t timer;

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R,
        GPIO_IRQ_EDGE_FALL,
        true,
        &btn_callback
    );

    if (!add_repeating_timer_ms(500, timer_callback, NULL, &timer)) {
        printf("Falha ao adicionar o timer\n");
    }
    while (true) {
        if (piscaled) {
            if (active_timer) {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
                active_timer = false;
            }
        } else {
            gpio_put(LED_PIN_R, 0);
        }
        sleep_ms(10); 
    }
}
