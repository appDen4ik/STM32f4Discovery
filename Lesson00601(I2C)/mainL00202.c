/**
 ***************************************************************************
  * @file    mainL00601.c
  * @author  Denis
  * @version V1.
  * @date    5-Aug-2015
  * @brief   � ������ ��������� ��������������� ������ i2c �� ������� ds3231
 ***************************************************************************
 * ������� ��������:
 * 	-	������������� ���� ������ get � set. �������� ����� ��������
 * 	-	��� ����� ������� get �������� ������� ����� �� ds3231 � ���������
 * 		� �������� � ������� HH:MM:SS
 * 	-	����  set  HH:MM:SS  ������������� ������� �����
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

	__enable_irq();

}

void setTime( void ) {

}

void getTime( void ) {

}

