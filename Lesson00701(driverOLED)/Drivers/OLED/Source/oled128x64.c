/**
 ***************************************************************************
  * @file    oled128x64.c
  * @author  Denis
  * @version V0.1
  * @date    19-Aug-2015
  * @brief   ������� ��������� ��� ���32f407. ����������� ������ �������
  * 		 ������ ����� � ������������� i2c
 ***************************************************************************
*/
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "oled128x64.h"


void writeOneByte( uint8_t adress, uint8_t data ) {

// ������� ��������� �������� �� ����� ( SDA � SCL - high )
	while ( ( I2C1->SR2 & I2C_SR2_BUSY ) != 0 ) {

	}

// ����� ���������� START ���
	I2C1->CR1 |= I2C_CR1_START;

// ��������� ��� ��� �� ( ����� ������������� )
	while ( 0 == ( I2C1->SR1 & I2C_SR1_SB )  ) {

	}

// ���������� ����� slave + 0 ��� ����������� ( 1 - ������, 0 - ������ )
// 0bit = 0
	I2C1->DR = ( OLED_ADRESS );

// ������� ACK �� ������
	while ( ( I2C1->SR1 & I2C_SR1_ADDR ) == 0 ) {

	}

// ����� ������� I2C_SR1_ADDR ���������� ������������� ��������� I2C1->SR2
	if ( I2C1->SR2 ){

	}

// ������������� ����� startAdress
	I2C1->DR = adress;

// ������� ���� data register ������ empty
	while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

	}

// ����� ������
	I2C1->DR = data;

// ������� ���� data register ������ empty
    while ( ( I2C1->SR1 & I2C_SR1_TXE ) == 0 ) {

	}

//��������� ����
    I2C1->CR1 |=  ( I2C_CR1_STOP );
//���� ���� ����� �����������
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
