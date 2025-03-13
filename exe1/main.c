#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define LED_PIN_0 2
#define BTN_PIN_0 3

volatile bool pisca_0 = false;     // Flag que indica se deve piscar ou não
volatile bool timer_0_ativo = false; // Flag que indica que o timer “disparou”

bool timer_callback_0(repeating_timer_t *rt) {
    timer_0_ativo = true; 
    return true; 
}

void btn_callback_0(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        // Inverte a flag
        pisca_0 = !pisca_0;
    }
}

int main() {
    stdio_init_all();

    // Configura GPIO
    gpio_init(LED_PIN_0);
    gpio_set_dir(LED_PIN_0, GPIO_OUT);
    gpio_put(LED_PIN_0, 0);

    gpio_init(BTN_PIN_0);
    gpio_set_dir(BTN_PIN_0, GPIO_IN);
    gpio_pull_up(BTN_PIN_0);

    // Ativa interrupção do botão
    gpio_set_irq_enabled_with_callback(
        BTN_PIN_0,
        GPIO_IRQ_EDGE_FALL,
        true,
        &btn_callback_0
    );

    // Timer struct (não confundir com a flag booleana)
    repeating_timer_t timer_0;

    // Inicia timer que dispara a cada 500ms
    if (!add_repeating_timer_ms(500, timer_callback_0, NULL, &timer_0)) {
        printf("Falha ao iniciar timer_0\n");
    }

    while (true) {
        if (pisca_0) {                       // Se precisamos piscar
            if (timer_0_ativo) {            // E se o timer disparou
                gpio_put(LED_PIN_0, !gpio_get(LED_PIN_0));
                timer_0_ativo = false;      // Reseta a flag
            }
        } else {
            gpio_put(LED_PIN_0, 0);         // Fica apagado
        }

        sleep_ms(10);
    }

    return 0;
}
