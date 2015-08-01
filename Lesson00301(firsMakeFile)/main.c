#include "stm32f4xx_rcc.h"

int main(void)
{

volatile int i = 0;

	RCC->AHB1ENR  |= RCC_AHB1Periph_GPIOD;
	GPIOD -> MODER = 0x55000000;
	GPIOD -> OTYPER = 0;
	GPIOD -> OSPEEDR = 0;


    while(1)
    {
    	GPIOD -> ODR = 0xf000;
    	    	for ( i=0xfffff; i > 0; i-- ){

    	    	}


    	GPIOD -> ODR = 0x0000;
    	    	for ( i=0xfffff; i > 0; i-- ){

    	    	}
    }
    return 0;}
