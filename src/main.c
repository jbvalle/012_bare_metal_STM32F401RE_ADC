#include <stdint.h>
#include <stdio.h>
#include "peripherals.h"


RCC_t   * const RCC     = (RCC_t    *)  0x40023800;
GPIOx_t * const GPIOA   = (GPIOx_t  *)  0x40020000;
USART_t * const USART2  = (USART_t  *)  0x40004400;
NVIC_t  * const NVIC    = (NVIC_t   *)  0xE000E100;

void initialise_monitor_handles(void);

void wait_ms(int time){
    for(int i = 0; i < time; i++){
        for(int j = 0; j < 1600; j++);
    }
}
int main(void){

    initialise_monitor_handles();

    RCC->RCC_AHB1ENR |= 1;
    
    GPIOA->GPIOx_MODER &= ~(3 << (5 * 2));
    GPIOA->GPIOx_MODER |=  (1 << (5 * 2));

    int var = 120;

    for(;;){

        printf("%d\r\n", var);

        GPIOA->GPIOx_ODR ^= (1 << 5);
        wait_ms(100);
    }
}
