/**
 ***************************************************************************
  * @file    mainL00601.c
  * @author  Denis
  * @version V1.
  * @date    5-Aug-2015
  * @brief   В данной программе рассматривается работа i2c на примере ds3231
 ***************************************************************************
 * Краткое описание:
 * 	-	Использование двух команд get и set. Вводятся через терминал
 * 	-	При вводе команды get читается текущее время из ds3231 и выводится
 * 		в терминал в формате HH:MM:SS
 * 	-	Ввод  set  HH:MM:SS  устанавливает текущее время
 ****************************************************************************
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

__attribute__( ( always_inline ) ) __STATIC_INLINE  void init( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void uartReceived( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void uartTransm( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Received( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Transm( void );
static void getTime( void );
static void setTime( void );

typedef enum{
				NONE,
				UART_RECEIVED,
				UART_TRANSMIT,
				DS3231_TRANSMIT,
				DS3231_RECEIVED
								} status;

status currentStatus = NONE;

void USART2_IRQHandler(void) {
	USART_SendData(USART2, '@');
}


void main( void ) {

	init();

	while ( 1 ) {

		if ( currentStatus == UART_RECEIVED ) {
			uartReceived();
		} else if ( currentStatus ==  UART_TRANSMIT ) {
			uartTransm();
		} else if ( currentStatus ==  DS3231_TRANSMIT ) {
			ds3231Transm();
		} else if ( currentStatus ==  DS3231_RECEIVED ) {
			ds3231Received();
		}

	}

}

void uartReceived( void ) {

}

void uartTransm( void ) {

}

void ds3231Received( void ) {

}

void ds3231Transm( void ) {

}



void init( void ) {


	{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //TX
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //RX
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	  	GPIO_Init(GPIOA, &GPIO_InitStructure);


	  	USART_StructInit(&USART_InitStructure);

	  	USART_Init(USART2, &USART_InitStructure);
	  	USART_Cmd(USART2, ENABLE);

	    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	    NVIC_Init(&NVIC_InitStructure);

	    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}

	{
		GPIO_InitTypeDef gpio;
		I2C_InitTypeDef i2c;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);


		i2c.I2C_ClockSpeed = 100000;
		i2c.I2C_Mode = I2C_Mode_I2C;
		i2c.I2C_DutyCycle = I2C_DutyCycle_2;
		i2c.I2C_OwnAddress1 = 0x15;
		i2c.I2C_Ack = I2C_Ack_Disable;
		i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_Init(I2C1, &i2c);


		gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_OType = GPIO_OType_OD;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &gpio);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

		I2C_Cmd(I2C1, ENABLE);
	}



	__enable_irq();

}

void setTime( void ) {

}

void getTime( void ) {

}

