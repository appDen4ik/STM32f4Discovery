#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_usart.h"

#define oled_adress 0x78

void init( void );
void writeOneByte( uint8_t adress, uint8_t data );

uint8_t tmp;

uint8_t inf[128] = { 0xff, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff, 0xff };

uint8_t counter;

void main( void ) {

	init();
	GPIO_SetBits( GPIOD, GPIO_Pin_12 );

	writeOneByte( 0b0, 0xAE );//Display ON

	writeOneByte( 0b0, 0xA8 );//Set MUX
	writeOneByte( 0b0, 0x3F );

	writeOneByte( 0b0, 0xD3 );//Set Display Offset
	writeOneByte( 0b0, 0x0 );

	writeOneByte( 0b0, 0x40 );//Set display start adress

	writeOneByte( 0b0, 0xA1 );//Set segment re-map

	writeOneByte( 0b0, 0xc8 );//Set COM output scan direction

	writeOneByte( 0b0, 0xda );//Set Com Pin hardware configuration
	writeOneByte( 0b0, 0x02 );

	writeOneByte( 0b0, 0x81 );//Set Contrast Control
	writeOneByte( 0b0, 0xbf );


	writeOneByte( 0b0, 0xA4 );//Disable Entire Display On

	writeOneByte( 0b0, 0xA6 );//Set Normal Display

	writeOneByte( 0b0, 0xd5 );//Set Osc Frequency
	writeOneByte( 0b0, 0x80 );

	writeOneByte( 0b0, 0x8d );//Enable charge pump regulato
	writeOneByte( 0b0, 0x14 );

	writeOneByte( 0b0, 0xAF );//Display ON

	writeOneByte( 0b0, 0x20 );//Set Memory Addressing Mode
	writeOneByte( 0b0, 0x00 );

	GPIO_SetBits( GPIOD, GPIO_Pin_13 );



	while ( 1 ) {
		for (counter = 0; counter < 128; counter++) {
			writeOneByte( 0b01000000, inf[counter] );
		}
		GPIO_SetBits( GPIOD, GPIO_Pin_14 );
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

	}

	{//i2c1 PB6 - SCL PB7 - SDA
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

		// включаем тактирование порта gpiob и i2c

/*		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

		//настраиваем i2c, включаем тактирование
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	    // Reset
	    I2C1->CR1 = I2C_CR1_SWRST;*/


	}

}

void writeOneByte( uint8_t adress, uint8_t data ) {

// сначала проверяем свободна ли линия ( SDA и SCL - high )
	while ( ( I2C1->SR2 & I2C_SR2_BUSY ) != 0 ) {

	}

// далее отправляем START бит
	I2C1->CR1 |= I2C_CR1_START;

// проверяем что все ок ( старт сформировался )
	while ( 0 == ( I2C1->SR1 & I2C_SR1_SB )  ) {

	}

// записываем адрес slave + 0 бит направления ( 1 - чтение, 0 - запись )
// 0bit = 0
	I2C1->DR = ( ( oled_adress ) );

// ожидаем ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	tmp = I2C1->SR2;

// устанавливаем адрес startAdress
	I2C1->DR = adress;

// ожидаем пока data register станет empty
	while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

	}

// пишем данные
	I2C1->DR = data;

// ожидаем пока data register станет empty
    while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

	}

//формируем стоп
    I2C1->CR1 |=  ( I2C_CR1_STOP );
//ждем пока линия освободится
    while( I2C1->SR2 & I2C_SR2_BUSY ) {

    }

}
