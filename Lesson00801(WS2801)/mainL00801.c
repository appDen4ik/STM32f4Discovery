// APA102

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


void deley(void);
void del(void);
void setColor(uint32_t color);
void setnull(void);

void main(void) {
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);



	/*  GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	  GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	  GPIO_SetBits(GPIOD, GPIO_Pin_13);
	  del(); */
	  setColor(0xffffffff);
while (1){

	  setnull();

	  setColor(0xE0000000);
	  	  setColor(0xE0000000);
	  	 setColor(0xffffffff);
	  	 del();



	  	 setnull();
	  setColor(0xff8B2202);
	  setColor(0xff8B2202);
	  setColor(0xffffffff);


	  del();

	  setnull();
	  setColor(0xffE0CA00 );


	  setColor(0xffD540D7);

	  setColor(0xffffffff);

	  del();

	  setnull();
	  setColor(0xff471B50);
	  setColor(0xE0000000);
	  setColor(0xffffffff);
	  del();
	  GPIO_SetBits(GPIOD, GPIO_Pin_15);
}




}
void setnull(void){
	int8_t shift = 31;
	uint32_t color = 0x00000000;
//	GPIO_SetBits(GPIOD, GPIO_Pin_13);

	for ( ;shift >= 0 ;shift-- ) {

		if ( (color >> shift) & 1 ){
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		}


		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	deley();
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		deley();

		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//		GPIO_ResetBits(GPIOD, GPIO_Pin_14);

	}


	GPIO_SetBits(GPIOD, GPIO_Pin_13);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);

}



//pd13 clock, pd14 data
void setColor(uint32_t color){
	int8_t shift = 31;
//	GPIO_SetBits(GPIOD, GPIO_Pin_13);

	for ( ;shift >= 0 ;shift-- ) {

		if ( (color >> shift) & 1 ){
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		}


		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	deley();
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		deley();

		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//		GPIO_ResetBits(GPIOD, GPIO_Pin_14);

	}

	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);

}

void deley(void){
	uint32_t i = 100;
	for ( ;i-- > 0 ; ) {

	}

}

void del(void){
	uint32_t i = 0xffffff;
	for ( ;i-- > 0 ; ) {

	}
}




