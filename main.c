#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio_driver.h"
#include "systimer.h"


#define BAG_OPENED_BUTTON_PIN      (0)
#define BAG_LIGHT_EN_PIN           (1)
#define BAG_HAND_OPEN_BUTTON_PIN   (4)
#define BAG_SERVO_PIN              (3)


typedef enum {
    BAG_LIGHT_STATE_BAG_CLOSED = 0,
    BAG_LIGHT_STATE_BUTTON_DOWN_DEBOUNCE,
    BAG_LIGHT_STATE_BAG_OPENED_LIGHT_ON,
    BAG_LIGHT_STATE_BUTTON_UP_DEBOUNCE,
    BAG_LIGHT_STATE_BAG_OPENED_LIGHT_OFF
} bag_light_state_t;
bag_light_state_t bag_light_state = BAG_LIGHT_STATE_BAG_CLOSED;
timer_t opened_button_db_timer, light_timer;

typedef enum {
    BAG_SERVO_STATE_BUTTON_CLICK_WAITING = 0,
    BAG_SERVO_STATE_BUTTON_DOWN_DEBOUNCE,
    BAG_SERVO_STATE_OPEN_PROCESS,
    BAG_SERVO_STATE_HOLD_PROCESS
} bag_servo_state_t;
bag_servo_state_t bag_servo_state = BAG_SERVO_STATE_BUTTON_CLICK_WAITING;
timer_t bag_servo_timer;


int main(void) {
    // Tim 0 init for systimer
    TCCR0A = 0;
    TCCR0B = (1 << CS00);  // Clock Select: 0x00-0x05 -> 0/1/8/32/64/128/256/1024
    TIMSK0 = (1 << TOIE0);  // TOIE0 irq en 

    sei();   // global IRQ enable
    gpio_init();


    while(1) {
        switch (bag_light_state) {
            case BAG_LIGHT_STATE_BAG_CLOSED:
                if ((GPIOB_GET(BAG_OPENED_BUTTON_PIN)) == 0) {
                    opened_button_db_timer = systimer_set_ms(1200);
                    bag_light_state = BAG_LIGHT_STATE_BUTTON_DOWN_DEBOUNCE;
                }
                break;

            case BAG_LIGHT_STATE_BUTTON_DOWN_DEBOUNCE:
                if (systimer_triggered_ms(opened_button_db_timer)) {
                    if ((GPIOB_GET(BAG_OPENED_BUTTON_PIN)) == 0) {
                        GPIOB_SET(BAG_LIGHT_EN_PIN);
                        light_timer = systimer_set_ms(305000);   // 5 minutes
                        bag_light_state = BAG_LIGHT_STATE_BAG_OPENED_LIGHT_ON;
                    }
                    else {
                        bag_light_state = BAG_LIGHT_STATE_BAG_CLOSED;
                    }
                }
                break;

            case BAG_LIGHT_STATE_BAG_OPENED_LIGHT_ON:
                if ((GPIOB_GET(BAG_OPENED_BUTTON_PIN)) == 1) {
                    opened_button_db_timer = systimer_set_ms(1200);
                    bag_light_state = BAG_LIGHT_STATE_BUTTON_UP_DEBOUNCE;
                }
                else if (systimer_triggered_ms(light_timer)) {
                    GPIOB_RESET(BAG_LIGHT_EN_PIN);
                    bag_light_state = BAG_LIGHT_STATE_BAG_OPENED_LIGHT_OFF;
                }
                break;

            case BAG_LIGHT_STATE_BUTTON_UP_DEBOUNCE:
                if (systimer_triggered_ms(opened_button_db_timer)) {
                    if ((GPIOB_GET(BAG_OPENED_BUTTON_PIN)) == 1) {
                        GPIOB_RESET(BAG_LIGHT_EN_PIN);
                        bag_light_state = BAG_LIGHT_STATE_BAG_CLOSED;
                    }
                    else {
                        bag_light_state = BAG_LIGHT_STATE_BAG_OPENED_LIGHT_ON;
                    }
                }
                break;

            case BAG_LIGHT_STATE_BAG_OPENED_LIGHT_OFF:
                if ((GPIOB_GET(BAG_OPENED_BUTTON_PIN)) == 1) {
                    opened_button_db_timer = systimer_set_ms(1200);
                    bag_light_state = BAG_LIGHT_STATE_BUTTON_UP_DEBOUNCE;
                }
                break;
        }


        switch (bag_servo_state) {
            case BAG_SERVO_STATE_BUTTON_CLICK_WAITING:
                if ((GPIOB_GET(BAG_HAND_OPEN_BUTTON_PIN)) == 0) {
                    bag_servo_timer = systimer_set_ms(2200);
                    bag_servo_state = BAG_SERVO_STATE_BUTTON_DOWN_DEBOUNCE;
                }
                break;

            case BAG_SERVO_STATE_BUTTON_DOWN_DEBOUNCE:
                if (systimer_triggered_ms(bag_servo_timer)) {
                    if ((GPIOB_GET(BAG_HAND_OPEN_BUTTON_PIN)) == 0) {
                        GPIOB_SET(BAG_SERVO_PIN);
                        bag_servo_timer = systimer_set_ms(1100);
                        bag_servo_state = BAG_SERVO_STATE_OPEN_PROCESS;
                    }
                    else {
                        bag_servo_state = BAG_SERVO_STATE_BUTTON_CLICK_WAITING;
                    }
                }
                break;

            case BAG_SERVO_STATE_OPEN_PROCESS:
                if (systimer_triggered_ms(bag_servo_timer)) {
                    GPIOB_RESET(BAG_SERVO_PIN);
                    bag_servo_timer = systimer_set_ms(5200);
                    bag_servo_state = BAG_SERVO_STATE_HOLD_PROCESS;
                }
                break;

            case BAG_SERVO_STATE_HOLD_PROCESS:
                if (systimer_triggered_ms(bag_servo_timer)) {
                    if ((GPIOB_GET(BAG_HAND_OPEN_BUTTON_PIN)) == 1) bag_servo_state = BAG_SERVO_STATE_BUTTON_CLICK_WAITING;
                    else bag_servo_timer = systimer_set_ms(1200);
                }
                break;
        }

    }

}



ISR(TIM0_OVF_vect) {
    systimer_process();
}
