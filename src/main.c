#include <stdint.h>
#include <stdio.h>
#include "peripherals.h"

// GPIO Macros
#define pin1    1
#define pin5    5
#define MODER   2
#define OUTPUT  1
#define ANALOG  3
#define RESET_MODER 3
#define GPIOA_EN 1
// ADC Macros
#define CH1 1
#define ADC1    8
#define LENGTH_SEQ 20
#define CONV_START 30
#define EOC_FLAG 1
#define EOC_INTERRUPT 5
#define ADC_IRQ 18
#define CONT_MODE 1

ADC_t   * const ADC    = (ADC_t    *)  0x40012000;
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

void _global_enable_NVIC(void){
    uint32_t priMASK = 0;
    __asm volatile("MSR primask, %0"::"r"(priMASK):"memory");
}

void _global_disable_NVIC(void){
    uint32_t priMASK = 1;
    __asm volatile("MSR primask, %0"::"r"(priMASK):"memory");
}


void ADC_handler(void){

    // Read ADC value | EOC is reset automatically
    uint16_t result = ADC->ADC_DR;
    printf("Value %u:\r\n", (uint16_t)result);
}

void _configure_ADC1_interrupt(uint32_t analog_PIN, uint32_t channel){

    // SET global disable 
    _global_disable_NVIC();

    // Enable and configure PA1 as Analog Pin 
    RCC->RCC_AHB1ENR |= GPIOA_EN;
    // Set GPIOA1 as Analog
    GPIOA->GPIOx_MODER &= ~(RESET_MODER     << (analog_PIN * MODER));
    GPIOA->GPIOx_MODER |=  (ANALOG          << (analog_PIN * MODER));

    //* Configure ADC1 *//
    RCC->RCC_APB2ENR |= (1 << ADC1);
    // Turn off ADC for configuration
    ADC->ADC_CR2 &= ~1;
    // Set Total number of convers to 1 | setting to 0 = 1 conversion
    ADC->ADC_SQR1 &= ~(0xF << LENGTH_SEQ);
    // Set first conversion in SQR to be from CH1
    ADC->ADC_SQR3 &= ~0x1F;
    ADC->ADC_SQR3 |= channel;
    // Enable interrupt for EOC FLAG
    ADC->ADC_CR1 |= (1 << EOC_INTERRUPT);
    // Enable NVIC IRQ18
    NVIC->ISER[ADC_IRQ / 32] |= (1 << (ADC_IRQ % 32));
    // global enable NVIC MASK
    _global_enable_NVIC();
    // Turn on ADC conversion
    ADC->ADC_CR2 |= 1;
    // Enable Continous Conversion Mode
    ADC->ADC_CR2 |= (1 << CONT_MODE);
    // Start the conversion
    ADC->ADC_CR2 |= (1 << CONV_START);
}
int main(void){

    initialise_monitor_handles();

    _configure_ADC1_interrupt(pin1, CH1);

    for(;;);
}
