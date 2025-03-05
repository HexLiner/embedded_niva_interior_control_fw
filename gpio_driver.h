#include <avr/io.h>


#ifndef _GPIO_DRIVER_H_
#define _GPIO_DRIVER_H_


#define GPIOB_SET(pin_n) PORTB |= 1 << (pin_n)
#define GPIOB_SET_GR(pins_msk) PORTB |= (pins_msk)
#define GPIOB_RESET(pin_n) PORTB &= ~(1 << (pin_n))
#define GPIOB_RESET_GR(pins_msk) PORTB &= ~(pins_msk)
#define GPIOB_GET(pin_n) (PINB >> (pin_n)) & 0x01
#define GPIOB_GET_GR(pins_msk) PINB & (pins_msk)
#define GPIOB_MODE_OUTPUT(pin_n) DDRB |= (1 << pin_n)
#define GPIOB_MODE_INPUT(pin_n) DDRB &= ~(1 << pin_n)


extern void gpio_init(void);


#endif  // _GPIO_DRIVER_H_
