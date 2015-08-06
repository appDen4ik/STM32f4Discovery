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
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Read( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Write( void );
uint8_t StringToChar(uint8_t* data);
void sendDataToUart(uint8_t *data);
uint8_t * ShortIntToString(uint16_t data, uint8_t* adressDestenation);
__attribute__( ( always_inline ) ) __STATIC_INLINE uint8_t DicToBinDic(uint8_t data);
static void getTime( void );
static void setTime( void );

typedef enum {
				NONE,
				UART_RECEIVED,
				UART_TRANSMIT,
				DS3231_WRITE,
				DS3231_READ
								} status;

status currentStatus = NONE;
//---------------------------------------------------------------
uint8_t buffUART[100] = { 0 };
uint8_t counterBuffUart = 0;
uint8_t sec;
uint8_t hour;
uint8_t min;
uint8_t tmp;
const uint8_t ds3231_adress = 0b11010000;
//---------------------------------------------------------------

void USART2_IRQHandler(void) {
	buffUART[counterBuffUart++] = (uint16_t)(USART2->DR & (uint16_t)0x01FF);

	if ( buffUART[counterBuffUart - 1] == '!' ) {
		currentStatus = UART_RECEIVED;
		NVIC_DisableIRQ(USART2_IRQn);
	}

// выключаем прерывания до тех пор пока не обработаем текущую команду
}


void main( void ) {

	init();
	GPIO_SetBits( GPIOD, GPIO_Pin_12 );
	while ( 1 ) {

		if ( currentStatus == UART_RECEIVED ) {
			uartReceived();
		} else if ( currentStatus ==  UART_TRANSMIT ) {
			uartTransm();
			currentStatus = NONE;
			counterBuffUart = 0;
			NVIC_EnableIRQ(USART2_IRQn);
		} else if ( currentStatus ==  DS3231_WRITE ) {
			ds3231Write();
			currentStatus = NONE;
			NVIC_EnableIRQ(USART2_IRQn);
		} else if ( currentStatus ==  DS3231_READ ) {
			ds3231Read();
			currentStatus = UART_TRANSMIT;
		}

	}

}

void uartReceived( void ) {

	// формат команд
	// set HH:MM:SS\0
	// get\0

	if ( buffUART[0] == 's' && buffUART[1] == 'e' && buffUART[2] == 't' ) {
		currentStatus = DS3231_WRITE;
		tmp = StringToChar( &buffUART[4] );
		hour = DicToBinDic( tmp );
/*		while(!(USART2->SR & USART_SR_TC));    //
			USART2->DR=hour;                   //*/
		for ( counterBuffUart = 4; buffUART[counterBuffUart] != ':'; counterBuffUart++ );
		tmp = StringToChar( &buffUART[++counterBuffUart] );
		min = DicToBinDic( tmp );
/*		while(!(USART2->SR & USART_SR_TC));    //
			USART2->DR=min;                   //*/
		for ( ; buffUART[counterBuffUart] != ':'; counterBuffUart++ );
		tmp = StringToChar( &buffUART[++counterBuffUart] );
		sec = DicToBinDic( tmp );
		while(!(USART2->SR & USART_SR_TC));    //
			USART2->DR=sec;                   //
			counterBuffUart = 0;
	} else if ( buffUART[0] == 'g' && buffUART[1] == 'e' && buffUART[2] == 't' ) {
		currentStatus = DS3231_READ;
		GPIO_ToggleBits( GPIOD, GPIO_Pin_13 );
	}

}

void uartTransm( void ) {

}

void ds3231Read( void ) {

	while(!(USART2->SR & USART_SR_TC));    //
				USART2->DR='0';

	while(I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY) );
	I2C_GenerateSTART( I2C1, ENABLE );

	GPIO_ToggleBits( GPIOD, GPIO_Pin_14 );

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT) );

	I2C_Send7bitAddress( I2C1, ds3231_adress, I2C_Direction_Transmitter ); //I2C_Direction_Receiver

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) );

	I2C_SendData(I2C1, 0);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));





//	while(I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY) );
	I2C_GenerateSTART( I2C1, ENABLE );

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT) );

	I2C_Send7bitAddress( I2C1, ds3231_adress, I2C_Direction_Receiver );

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) );


	I2C_AcknowledgeConfig( I2C1, ENABLE );
	while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );


	sec = I2C_ReceiveData(I2C1);

	while( !(USART2->SR & USART_SR_TC) );    //
				USART2->DR=sec;


//	min = I2C_ReceiveData(I2C1);
	I2C_AcknowledgeConfig( I2C1, ENABLE );



	while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );



		min = I2C_ReceiveData(I2C1);

		while( !(USART2->SR & USART_SR_TC) );    //
					USART2->DR=min;


	I2C_AcknowledgeConfig( I2C1, DISABLE );
	while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );

	hour = I2C_ReceiveData(I2C1);

	while( !(USART2->SR & USART_SR_TC) );    //
				USART2->DR=hour;

				GPIO_ToggleBits( GPIOD, GPIO_Pin_15 );

	I2C_GenerateSTOP( I2C1, ENABLE );


}

void ds3231Write( void ) {


}

uint8_t DicToBinDic(uint8_t data) {

	return ( data % 10 ) | ( ( data / 10 ) << 4 );
}

void sendDataToUart(uint8_t *data) {
	while (*data != '\0') {
		 while(!(USART2->SR & USART_SR_TC));
		 USART2->DR=*data++;
	}
}

void init( void ) {

		GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);



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

// Перевод последовательности ASCII в число
uint8_t StringToChar(uint8_t* data) {
	uint8_t returnedValue = 0;
	for (;*data >= '0' && *data <= '9'; data++)
	returnedValue = 10 * returnedValue + (*data - '0');
	return returnedValue;
}


// Перевод числа в последовательность ASCII
uint8_t * ShortIntToString(uint16_t data, uint8_t* adressDestenation) {


	uint8_t *startAdressDestenation = adressDestenation;
	uint8_t *endAdressDestenation;
	uint8_t buff;

	do {// перевод входного значения в последовательность ASCII кодов
		// в обратном порядке
		*adressDestenation++ = data % 10 + '0';
	} while ((data /= 10) > 0);
	endAdressDestenation = adressDestenation ;

	// разворот последовательности
	for (adressDestenation--; startAdressDestenation < adressDestenation;startAdressDestenation++, adressDestenation--) {
		buff = *startAdressDestenation;
		*startAdressDestenation = *adressDestenation;
		*adressDestenation = buff;
	}
	return endAdressDestenation;
}

void setTime( void ) {

}

void getTime( void ) {

}

