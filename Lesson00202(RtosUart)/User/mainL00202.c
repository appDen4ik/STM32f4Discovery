#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


void Led1( void *pvParameters );
void Led2( void *pvParameters );
void Led3( void *pvParameters );
void Led4( void *pvParameters );
void uartTask1( void *pvParameters );
void uartTask2( void *pvParameters );
void initPeriph( void );
static inline void sendDataToUart( uint8_t *data )  __attribute__((always_inline));


void vApplicationIdleHook( void )
{
}



/*******************************************************************/
void vApplicationMallocFailedHook( void )
{
    for( ;; );
}



/*******************************************************************/
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    for( ;; );
}



/*******************************************************************/
void vApplicationTickHook( void )
{
}

SemaphoreHandle_t mMutex;

int main(void) {


	initPeriph();

	 mMutex = xSemaphoreCreateMutex();

	if( mMutex != NULL ){

	    xTaskCreate( Led1, "vTask1", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	    xTaskCreate( Led2, "vTask2", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	    xTaskCreate( Led3, "vTask3", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	    xTaskCreate( Led4, "vTask4", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	    xTaskCreate( uartTask1, "vTask5", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	    xTaskCreate( uartTask2, "vTask6", configMINIMAL_STACK_SIZE, NULL, 1,  NULL );
	}
	    vTaskStartScheduler();

	//если все ок то сюда никогда не попадем
	while(1);

    return 0;
}



void Led1( void *pvParameters ){

	uint32_t i;

	  while(1) {

	    	GPIO_ToggleBits( GPIOD, GPIO_Pin_12 );
	    	for ( i = 0xAffff; i > 0; i-- ){

	    	}
	  }
}

void Led2( void *pvParameters ){

	uint32_t i;

	  while(1) {

	    	for ( i = 0x1b000f; i > 0; i-- ){

	    	}
	    	GPIO_ToggleBits( GPIOD, GPIO_Pin_13 );
	  }
}

void Led3( void *pvParameters ){
	uint32_t i;

		while(1) {

			 GPIO_ToggleBits( GPIOD, GPIO_Pin_14 );
			 for ( i = 0x3000ff; i > 0; i-- ){

			 }

		}

}

void Led4( void *pvParameters ){

	uint32_t i;

		while(1) {

			GPIO_ToggleBits( GPIOD, GPIO_Pin_15 );
			for ( i = 0xbfffff; i > 0; i-- ){

			}
		}

}



void uartTask1( void *pvParameters ) {
	uint32_t i;

	while( 1 ){
		for ( i = 0xfffff; i > 0; i-- ){

		}
		xSemaphoreTake( mMutex, portMAX_DELAY );
			sendDataToUart( (uint8_t *)"Hello World 1" );
		xSemaphoreGive( mMutex );

	}

}

void uartTask2( void *pvParameters ) {
	uint32_t i;

	while( 1 ){
		for ( i = 0xfffff; i > 0; i-- ){

		}
		xSemaphoreTake( mMutex, portMAX_DELAY );
			sendDataToUart( (uint8_t *)"Hello World 2" );
		xSemaphoreGive( mMutex );

	}

}

void initPeriph(void){

	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;


	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);


	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);


	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  USART_StructInit(&USART_InitStructure);
	  USART_InitStructure.USART_BaudRate = (uint32_t)115200;
	  USART_Init(USART2, &USART_InitStructure);
	  USART_Cmd(USART2, ENABLE);

}

void sendDataToUart(uint8_t *data){
	while (*data != '\0') {
		 while(!(USART2->SR & USART_SR_TC));
		 USART2->DR=*data++;
	}
}
















