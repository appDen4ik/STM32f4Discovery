/**
 ***************************************************************************
  * @file    mainL00701.c
  * @author  Denis
  * @version V1.
  * @date    19-Aug-2015
  * @brief   В данной программе рассматривается работа OLED 128x64 i2c
 ***************************************************************************
*/

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "oledGraphicLibrary.h"



void main( void) {

	GPIO_InitTypeDef GPIO_InitStructure;
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);



	initOLED();

	setDot(1, 1);
	setDot(128, 1);
	setDot(1, 64);
	setDot(128, 64);
	line(10,10,120,60);
	circle( 100, 20, 10 );
	GPIO_SetBits( GPIOD, GPIO_Pin_15 );
	while(1){

	}
}


