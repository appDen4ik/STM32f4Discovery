#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"


void Led1( void *pvParameters );
void Led2( void *pvParameters );
void Led3( void *pvParameters );
void Led4( void *pvParameters );


void initPeriph(void);

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


int main(void) {

	uint32_t i=0;

	initPeriph();

	xTaskCreate( Led1, "vTask1", configMINIMAL_STACK_SIZE, NULL, 1,  NULL);
	xTaskCreate( Led2, "vTask2", configMINIMAL_STACK_SIZE, NULL, 1,  NULL);
	xTaskCreate( Led3, "vTask3", configMINIMAL_STACK_SIZE, NULL, 1,  NULL);
	xTaskCreate( Led4, "vTask4", configMINIMAL_STACK_SIZE, NULL, 1,  NULL);

	vTaskStartScheduler();

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



void initPeriph(void){

	  GPIO_InitTypeDef GPIO_InitStructure;


	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);


	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

}


