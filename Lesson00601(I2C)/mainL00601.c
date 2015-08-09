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
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Read( void );
__attribute__( ( always_inline ) ) __STATIC_INLINE  void ds3231Write( void );
static uint8_t StringToChar( uint8_t* data );
static void sendDataToUart( uint8_t *data );
static uint8_t * ShortIntToString( uint16_t data, uint8_t* adressDestenation );
__attribute__( ( always_inline ) ) __STATIC_INLINE uint8_t DicToBinDic( uint8_t data );

static uint8_t readOneByte( uint8_t adress );
static void readBytes( uint8_t *destination, uint8_t startAdress, uint8_t howMatch );

static void writeOneByte( uint8_t adress, uint8_t data );
static void writeBytes( uint8_t startAdress, uint8_t *data, uint8_t howMatch );

//---------------------------------------------------------------------------------------------
typedef enum {
				NONE,
				UART_RECEIVED,
				DS3231_WRITE,
				DS3231_READ
								} status;

status currentStatus = NONE;
//---------------------------------------------------------------------------------------------
uint8_t buffUART[100] = { 0 };
uint8_t counterBuffUart = 0;
uint8_t sec = 0;
uint8_t hour = 0;
uint8_t min = 0;
uint8_t tmp = 0;
uint8_t secMinHour[3] = {0};
uint8_t all[12] = {0};
const uint8_t ds3231_adress = 0b1101000;
//---------------------------------------------------------------------------------------------

void USART2_IRQHandler(void) {
	buffUART[counterBuffUart++] = (uint16_t)(USART2->DR & (uint16_t)0x01FF);

	if ( buffUART[counterBuffUart - 1] == '!' ) {
		currentStatus = UART_RECEIVED;
	//	NVIC_DisableIRQ(USART2_IRQn);
	}

// выключаем прерывания до тех пор пока не обработаем текущую команду
}

void main( void ) {

	init();
	GPIO_SetBits( GPIOD, GPIO_Pin_12 );
	while ( 1 ) {

		if ( currentStatus == UART_RECEIVED ) {
			uartReceived();
		} else if ( currentStatus ==  DS3231_WRITE ) {
			ds3231Write();
			currentStatus = NONE;
			counterBuffUart = 0;
			NVIC_EnableIRQ(USART2_IRQn);
		} else if ( currentStatus ==  DS3231_READ ) {
			ds3231Read();
			currentStatus = NONE;
			counterBuffUart = 0;
		}

	}

}

void uartReceived( void ) {

	// формат команд
	// set HH:MM:SS!
	// get!

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
/*		while(!(USART2->SR & USART_SR_TC));
			USART2->DR=sec;   */
			counterBuffUart = 0;
	} else if ( buffUART[0] == 'g' && buffUART[1] == 'e' && buffUART[2] == 't' ) {
		currentStatus = DS3231_READ;
		GPIO_ToggleBits( GPIOD, GPIO_Pin_13 );
	}

}


void ds3231Read( void ) {

	readBytes( all, 0, 12 );

	for ( tmp = 0; tmp < 12;  ) {
	    while(!(USART2->SR & USART_SR_TC));
	    USART2->DR = all[tmp++];
	}

/*	while(!(USART2->SR & USART_SR_TC));
	USART2->DR = readOneByte( 0 );*/



   GPIO_SetBits( GPIOD, GPIO_Pin_15 );
}

void ds3231Write( void ) {

/*	writeOneByte( 0, sec);
	writeOneByte( 1, min );
	writeOneByte( 2, hour);*/

	secMinHour[0] = sec;
	secMinHour[1] = min;
	secMinHour[2] = hour;

	writeBytes( 0, secMinHour, 3 );

    GPIO_SetBits( GPIOD, GPIO_Pin_15 );

}


void readBytes( uint8_t *destination, uint8_t startAdress, uint8_t howMatch ) {

// сначала проверяем свободна ли линия ( SDA и SCL - high )
    while ( ( I2C1->SR2 & I2C_SR2_BUSY ) != 0 ) {

    }

// далее отправляем START бит (после отправки стартового бита модуль
// автоматически становится мастером)
    I2C1->CR1 |= I2C_CR1_START;

// проверяем что все ок ( старт сформировался )
    while ( 0 == ( I2C1->SR1 & I2C_SR1_SB ) ) {

    }

// записываем адрес slave + 0 бит направления ( 1 - чтение, 0 - запись )
// 0bit = 0
    I2C1->DR = ( ( ds3231_adress << 1 ) );

// ожидаем ACK от слейва
    while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

    }

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
    tmp = I2C1->SR2;

// устанавливаем адрес startAdress
    I2C1->DR = startAdress;

// ожидаем пока data register станет empty
    while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

    }

// отправляем повторный СТАРТ
    I2C1->CR1 |= I2C_CR1_START;

// проверяем что все ок ( старт сформировался )
    while ( ( I2C1->SR1 & I2C_SR1_SB ) == 0 ) {

    }

// записываем адрес slave + 0 бит направления ( 1 - чтение, 0 - запись )
// 0bit = 1 чтение
    I2C1->DR = ( ( ds3231_adress << 1 ) + 1 );

// ожидаем ответа ACK от слейва
    while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

    }

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
    tmp = I2C1->SR2;

//перед тем как забрать байты, укажем что будем ждать еще ACK
    I2C1->CR1 |=  I2C_CR1_ACK;

    for (; howMatch-- > 1;) {
    // ожидаем флаг что данные пришли
        while( ( I2C1->SR1 & I2C_SR1_RXNE ) == 0 ) {

        }

    // забираем данные
        *destination++ = I2C1->DR;
    }

//перед тем как забрать этот байт, укажем что больше байтов не ждем
    I2C1->CR1 &=  ( ~I2C_CR1_ACK );

// ожидаем флаг что данные пришли
    while( ( I2C1->SR1 & I2C_SR1_RXNE ) == 0 ) {

    }

// забираем данные
    *destination = I2C1->DR;

//формируем стоп
    I2C1->CR1 |=  ( I2C_CR1_STOP );
//ждем пока линия освободится
    while( I2C1->SR2 & I2C_SR2_BUSY ) {

    }

}

uint8_t readOneByte( uint8_t adress ) {

	uint8_t buf;

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
	I2C1->DR = ( ( ds3231_adress << 1 ) );

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

// отправляем повторный СТАРТ
	I2C1->CR1 |= I2C_CR1_START;

// проверяем что все ок ( старт сформировался )
    while ( ( I2C1->SR1 & I2C_SR1_SB ) == 0 ) {

	}

// записываем адрес slave + 0 бит направления ( 1 - чтение, 0 - запись )
// 0bit = 1 чтение
    I2C1->DR = ( ( ds3231_adress << 1 ) + 1 );


// ожидаем ответа ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}


// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	tmp = I2C1->SR2;

//перед тем как забрать этот байт, укажем что больше байтов не ждем
    I2C1->CR1 &=  ~I2C_CR1_ACK;
// ожидаем флаг что данные пришли
	while( ( I2C1->SR1 & I2C_SR1_RXNE ) == 0 ) {

	}

// забираем данные
    buf = I2C1->DR;

//формируем стоп
	I2C1->CR1 |= ( I2C_CR1_STOP );
//ждем пока линия освободится
	while( I2C1->SR2 & I2C_SR2_BUSY ) {

	}

	return buf;
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
	I2C1->DR = ( ( ds3231_adress << 1 ) );

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

void writeBytes( uint8_t startAdress, uint8_t *data, uint8_t howMatch ) {

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
	I2C1->DR = ( ( ds3231_adress << 1 ) );

// ожидаем ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	tmp = I2C1->SR2;

// устанавливаем адрес startAdress
	I2C1->DR = startAdress;

// ожидаем пока data register станет empty
	while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

	}


	for (; howMatch-- > 0 ;){
	// пишем данные
		I2C1->DR = *data++;

	// ожидаем пока data register станет empty
		while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

		}
	}


//формируем стоп
	I2C1->CR1 |=  ( I2C_CR1_STOP );
//ждем пока линия освободится
	while( I2C1->SR2 & I2C_SR2_BUSY ) {

	}

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


