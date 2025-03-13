#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile bool pisca_r = false;
volatile bool pisca_g = false;

volatile bool timer_r_flag = false;
volatile bool timer_g_flag = false;

repeating_timer_t timer_r;
repeating_timer_t timer_g;

bool timer_callback_r(repeating_timer_t *rt) {
    timer_r_flag = true;       
    return true;              
}


bool timer_callback_g(repeating_timer_t *rt) {
    timer_g_flag = true;     
    return true;               
}


void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  
        if (gpio == BTN_PIN_R) {
            flag_r = 1;                
        } else if (gpio == BTN_PIN_G) {
            flag_g = 1;                
        }
    }
}

int main() {
    stdio_init_all(); 
    
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); 

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0); 


    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    if (!add_repeating_timer_ms(500, timer_callback_r, NULL, &timer_r)) {
        printf("Falha ao adicionar timer do LED vermelho\n");
    }

    if (!add_repeating_timer_ms(250, timer_callback_g, NULL, &timer_g)) {
        printf("Falha ao adicionar timer do LED verde\n");
    }

    while (true) {
        if (flag_r) {
            flag_r = 0;                
            pisca_r = !pisca_r;        
            if (!pisca_r) {          
                gpio_put(LED_PIN_R, 0);
            }
        }

        if (flag_g) {
            flag_g = 0;                
            pisca_g = !pisca_g;     
            if (!pisca_g) {           
                gpio_put(LED_PIN_G, 0);
            }
        }

        if (pisca_r && timer_r_flag) {
            timer_r_flag = false; 
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        } 
        else if (!pisca_r) {
            gpio_put(LED_PIN_R, 0);
        }

        if (pisca_g && timer_g_flag) {
            timer_g_flag = false; 

            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        } 

        else if (!pisca_g) {
            gpio_put(LED_PIN_G, 0);
        }

        sleep_ms(10); 
    }

    return 0;
}
