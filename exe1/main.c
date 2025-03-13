#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_PIN_0  2
#define BTN_PIN_0  3


volatile int pisca_0 = 0;

// "timer_0" será a FLAG para saber se o timer disparou (1) ou não (0)
volatile int timer_0 = 0;

// Estrutura do SDK p/ o timer (renomeada para evitar conflito)
repeating_timer_t timer_0_struct; 

// Callback do timer: marca a FLAG
bool timer_callback_0(repeating_timer_t *rt) {
    timer_0 = 1;  
    return true; 
}

// Callback do botão: inverte o estado de "pisca_0"
void btn_callback_0(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        pisca_0 = !pisca_0;
    }
}

int main() {
    stdio_init_all();

    // Configura LED
    gpio_init(LED_PIN_0);
    gpio_set_dir(LED_PIN_0, GPIO_OUT);
    gpio_put(LED_PIN_0, 0);

    // Configura botão
    gpio_init(BTN_PIN_0);
    gpio_set_dir(BTN_PIN_0, GPIO_IN);
    gpio_pull_up(BTN_PIN_0);

    // Interrupção do botão
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_0,
        GPIO_IRQ_EDGE_FALL,
        true,
        &btn_callback_0
    );

    // Inicia o timer (observe que agora passamos &timer_0_struct)
    if (!add_repeating_timer_ms(500, timer_callback_0, NULL, &timer_0_struct)) {
        printf("Falha ao adicionar timer_0\n");
    }

    while (true) {
        // Se é para piscar e o timer "disparou" (timer_0 == 1)
        if (pisca_0 && timer_0) {
            // Reseta a flag
            timer_0 = 0;
            // Inverte LED
            gpio_put(LED_PIN_0, !gpio_get(LED_PIN_0));
        }
        else if (!pisca_0) {
            // Se não for piscar, manter LED apagado
            gpio_put(LED_PIN_0, 0);
        }

        sleep_ms(10);
    }
    return 0;
}
