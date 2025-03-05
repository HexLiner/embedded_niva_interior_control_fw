#include "gpio_driver.h"
#include <avr/io.h>


#define GPIO_INPUT  (0)
#define GPIO_OUTPUT (1)


void gpio_init(void) {
    // Port B initialization
    DDRB = (GPIO_INPUT  << DDB5) |
           (GPIO_INPUT  << DDB4) |
           (GPIO_OUTPUT << DDB3) |
           (GPIO_INPUT  << DDB2) |
           (GPIO_OUTPUT << DDB1) |
           (GPIO_INPUT  << DDB0);

    PORTB = (0 << PORTB5) |
            (0 << PORTB4) |
            (0 << PORTB3) |
            (0 << PORTB2) |
            (0 << PORTB1) |
            (0 << PORTB0);
}
