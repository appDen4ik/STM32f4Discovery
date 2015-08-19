/**
 ***************************************************************************
  * @file    oled128x64.c
  * @author  Denis
  * @version V0.1
  * @date    19-Aug-2015
  * @brief   Драйвер экранчика для стм32f407. Раилозована только функция
  * 		 записи байта и инициализации i2c
 ***************************************************************************
*/
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "oled128x64.h"


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
	I2C1->DR = ( OLED_ADRESS );

// ожидаем ACK от слейва
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}

// чтобы сбосить I2C_SR1_ADDR необходимо дополнительно прочитать I2C1->SR2
	if ( I2C1->SR2 ){

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

void initI2C( void ){
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

}
