#include <stdint.h>
#include <stdio.h>
#include "peripherals.h"
#define pin1    1
#define pin5    5

#define MODER   2
#define OUTPUT  1
#define ANALOG  3
#define ADC1    8
#define RESET_MODER 3
#define GPIOA_EN 1

RCC_t   * const RCC     = (RCC_t    *)  0x40023800;
GPIOx_t * const GPIOA   = (GPIOx_t  *)  0x40020000;
USART_t * const USART2  = (USART_t  *)  0x40004400;
NVIC_t  * const NVIC    = (NVIC_t   *)  0xE000E100;
ADC_t   * const ADC1     = (ADC_t    *)  0x40012000;

void initialise_monitor_handles(void);

void wait_ms(int time){
    for(int i = 0; i < time; i++){
        for(int j = 0; j < 1600; j++);
    }
}

int main(void){

    initialise_monitor_handles();


    // Enable and Configure GPIOA P5 
    RCC->RCC_AHB1ENR |= GPIOA_EN;
    
    GPIOA->GPIOx_MODER &= ~(RESET_MODER     << (pin5 * MODER));
    GPIOA->GPIOx_MODER |=  (OUTPUT          << (pin5 * MODER));

    // Enable and configure PA1 as Analog Pin 
    RCC->RCC_AHB1ENR |= GPIOA_EN;

    GPIOA->GPIOx_MODER &= ~(RESET_MODER     << (pin1 * MODER));
    GPIOA->GPIOx_MODER |=  (ANALOG          << (pin1 * MODER));

    //* Configure ADC1 *//
    RCC->RCC_APB2ENR |= (1 << ADC1);
    // Turn off ADC for configuration




    for(;;){

        GPIOA->GPIOx_ODR ^= (1 << 5);
        wait_ms(100);
    }
}
