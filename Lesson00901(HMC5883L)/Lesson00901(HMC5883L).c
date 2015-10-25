#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_i2c.h"
#include "misc.h"

#define HMC5883L_ADRESS 0x1E



static void initUSART( void )  __attribute__((always_inline));
static void initI2C(void) __attribute__((always_inline));
static uint8_t readOneByte( uint8_t adress );
static void writeOneByte( uint8_t adress, uint8_t data );
static uint8_t * ShortIntToString(int16_t data, uint8_t* adressDestenation);
static uint8_t StringToChar( uint8_t* data );
static uint8_t* append(uint8_t* destination, uint8_t* string);
static void sendStringToUART( uint8_t * data ); //ноль терминальная строка

static uint8_t tmp[200];
static uint8_t buf[10];
static uint8_t *p;



void main( void ) {

	uint32_t i;
	int16_t xValue;
	int16_t yValue;
	int16_t zValue;

	initUSART();
	initI2C();

	writeOneByte( 2, 0 );

	ShortIntToString(-2111, buf);
	sendStringToUART( buf );

	sendStringToUART( "Start OK" );

	while ( 1 ) {


		//p = append(tmp, (uint8_t *)"Не пересчитанные значения:      ");



		xValue = (int16_t)readOneByte( 0x03 );
		xValue = xValue << 8;
		xValue |= (int16_t)readOneByte( 0x04 );

	    ShortIntToString(xValue, buf);


		p = append( tmp, "X = " );
		p = append( p, buf );


		yValue = (int16_t)readOneByte( 0x07 ) << 8;
		yValue = yValue << 8;
		yValue |= (int16_t)readOneByte( 0x08 );

		ShortIntToString(yValue, buf);
		p = append( p, "    Y = " );
		p = append( p, buf );


		zValue = (int16_t)readOneByte( 0x05 ) << 8;
		zValue = zValue << 8;
		zValue |= (int16_t)readOneByte( 0x06 );

		ShortIntToString(zValue, buf);
		p = append( p, "    Z = " );
		p = append( p, buf );


		sendStringToUART( tmp );

		for ( i = 0x1fffff; i > 0; i-- );
	}
/*
		while ( ! ( ( USART2 -> SR ) & (USART_SR_RXNE) ) ) {
			for ( i = 0xff; i > 0; i-- );
		}
		tmp = USART2 -> DR;
		USART2 -> DR = tmp;*/

}


static void initUSART( void ) {
// usart 2
// pa3 - usart_rx, pa2 - usart_tx
//сначала включаем тактирование gpioa, usart2

	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

// настройка пинов gpio
// pa2 - tx
// pa3 - rx

	GPIOA -> MODER = ( ( GPIO_Mode_AF << ( GPIO_PinSource2 * 2 ) )   | ( GPIO_Mode_AF << ( GPIO_PinSource3 * 2 ) ) );
	GPIOA -> OTYPER  ;                                                                                        // no change, reset value output push pull
	GPIOA -> OSPEEDR = ( ( GPIO_Pin_2 << GPIO_Medium_Speed ) | ( GPIO_Pin_3 << GPIO_Medium_Speed ) );		  // no change, reset value low level
	GPIOA -> PUPDR = ( GPIO_PuPd_UP << ( GPIO_PinSource2 * 2 ) );                                  // tx - pull UP, rx - no change, reset value - NO PULL
	GPIOA -> LCKR ; 																					// no change, reset value - no lock state
	GPIOA -> AFR[0] = ( GPIO_AF_USART2 << ( GPIO_PinSource2 * 4 ) | GPIO_AF_USART2 << ( GPIO_PinSource3 * 4 ) );
	GPIOA -> AFR[1]; // no change, reset value 0x00

// настройка usart
//baudrate 115200

#define BAUDRATE        115200
#define APB1_presc		8
#define APB1_freq ( SystemCoreClock / APB1_presc )

// mantissa = 11
// fraction = 0.3932 * 16 = 6,2912 = 6

	uint8_t fraction =       6;  //APB1_freq % ( 16 * BAUDRATE );
	uint16_t mantissa =     11;//APB1_freq / ( 16 * BAUDRATE );


	USART2 -> BRR = (mantissa << 4) | fraction;
	USART2 -> CR1 = USART_WordLength_8b | USART_CR1_UE | USART_CR1_RE | USART_CR1_TE ;
}


void sendStringToUART( uint8_t * data) {

	while ( 1 ) {
		while ( ! ( ( USART2 -> SR ) & (USART_SR_TC) ) );
		USART2 -> DR = *data++;
		if ( *data == '\0' ) {
			return;
		}
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
	I2C1->DR = ( HMC5883L_ADRESS << 1);

// ожидаем ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}


// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	if ( I2C1->SR2 ) {

	}

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
    I2C1->DR = ( ( HMC5883L_ADRESS << 1 ) | 1 );


// ожидаем ответа ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}


// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	if( I2C1->SR2 ) {

	}

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
	I2C1->DR = ( HMC5883L_ADRESS << 1 );

// ожидаем ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	if ( I2C1->SR2 ) {

	}

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

void initI2C(void) {

//i2c1 PB6 - SCL PB7 - SDA
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

// Перевод последовательности ASCII в число
uint8_t StringToChar(uint8_t* data) {
	uint8_t returnedValue = 0;
	for (;*data >= '0' && *data <= '9'; data++)
	returnedValue = 10 * returnedValue + (*data - '0');
	return returnedValue;
}

// Перевод числа в последовательность ASCII
uint8_t * ShortIntToString(int16_t data, uint8_t* adressDestenation) {

	uint8_t *startAdressDestenation = adressDestenation;
	uint8_t *endAdressDestenation;
	uint8_t buff;

	uint8_t *copyAdr = adressDestenation;
	uint8_t sign = 0;


	if ( data < 0 ) {

		sign = '-';

		data = data * -1;
	}

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



	if ( sign == '-' ) {

		adressDestenation = endAdressDestenation;

		for ( adressDestenation--; adressDestenation >=  copyAdr; adressDestenation-- ){
			*(adressDestenation + 1) = *adressDestenation;
		}
		adressDestenation++;
		*adressDestenation = '-';


		endAdressDestenation++;
	}

	*endAdressDestenation = '\0';

	return endAdressDestenation ;
}

uint8_t* append(uint8_t* destination, uint8_t* string) {
	while(*string != '\0'){
		*destination++ = *string++;
	}
	*destination = '\0';
	return destination;
}



