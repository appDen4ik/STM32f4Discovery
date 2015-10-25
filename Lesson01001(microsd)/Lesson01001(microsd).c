
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

static void initSpi( void );
static void initMicrosd(void);
static void initUsart( void );
static void sendStringToUART( uint8_t * data);
static uint8_t writeData(uint8_t data);
static uint8_t sendCmd(uint8_t cmd, uint32_t argument);
static void deselect(void);
static int select(void);
static int wait_ready(uint16_t wt );			/* Timeout [ms] */



/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


#define LED_PORT GPIOD
#define LED_GREEN (1<<12)
#define LED_RED (1<<14)


void main() {

	int i;

	initSpi();
	initUsart();


	sendStringToUART ( "USART Init");
	for ( i = 0; i < 10000000; i++ ) {

	}

	initMicrosd();


	while ( 1 ) {


	}

}


static
int wait_ready (	/* 1:Ready, 0:Timeout */
	uint16_t wt			/* Timeout [ms] */
)
{
	uint8_t d;


	uint16_t Timer2 = wt;
	do {
		d = writeData(0xFF);
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
	} while (d != 0xFF /*&& Timer2*/);	/* Wait for card goes ready or timeout */

	return (d == 0xFF) ? 1 : 0;
}

static int select (void)	/* 1:OK, 0:Timeout */
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);		/* Set CS# low */
	writeData(0xFF);	/* Dummy clock (force DO enabled) */
	if (wait_ready(500)) {

		return 1;	/* Wait for card ready */
	}
	deselect();
	return 0;	/* Timeout */
}

static void deselect(void) {
	GPIO_SetBits(GPIOD,GPIO_Pin_12);	/* Set CS# high */
	writeData(0xFF);	                /* Dummy clock (force DO hi-z for multiple slave SPI) */

}



uint8_t sendCmd( uint8_t cmd, uint32_t arg ) {

	uint8_t n, res;

	if (cmd != CMD12) {

			deselect();
			if (!select()) {
				sendStringToUART ( "select error");
				return 0xFF;
			}
//			sendStringToUART ( "select check point");

	}

	/* Send command packet */
	writeData(0x40 | cmd);				/* Start + command index */
	writeData((uint8_t)(arg >> 24));		/* Argument[31..24] */
	writeData((uint8_t)(arg >> 16));		/* Argument[23..16] */
	writeData((uint8_t)(arg >> 8));			/* Argument[15..8] */
	writeData((uint8_t)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	writeData(n);

	n = 10;								/* Wait for response (10 bytes max) */

	do{
		res = writeData(0xFF);
//
	while ( ! ( ( USART2 -> SR ) & (USART_SR_TC) ) );
	USART2 -> DR = res;
//
	}while ((res & 0x80) && --n);

	return res;
}



void initSpi( void ) {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);



/*	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);*/

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);



/*	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);*/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);



	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOD, &GPIO_InitStructure);


		GPIO_SetBits(GPIOD,GPIO_Pin_12);




	//Заполняем структуру с параметрами SPI модуля
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полный дуплекс
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // передаем по 8 бит
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // Полярность и
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // фаза тактового сигнала
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // Управлять состоянием сигнала NSS программно
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // Предделитель SCK
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Первым отправляется старший бит
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // Режим - мастер
	SPI_Init(SPI1, &SPI_InitStructure); //Настраиваем SPI1
	SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1....

	// Поскольку сигнал NSS контролируется программно, установим его в единицу
	// Если сбросить его в ноль, то наш SPI модуль подумает, что
	// у нас мультимастерная топология и его лишили полномочий мастера.
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

}



void initMicrosd(void) {

	sendStringToUART ( "microsd start init" );
	int i;

	GPIO_SetBits(GPIOD,GPIO_Pin_12);
	//сначала посылаем 80 импульсов на clk
	for ( i = 0; i < 10; i++ ) {
		while ( ! ( ( USART2 -> SR ) & (USART_SR_TC) ) );
		USART2 -> DR = writeData(0xff);
	}


	while ( ! ( ( USART2 -> SR ) & (USART_SR_TC) ) );//
	if ( (USART2 -> DR = sendCmd(CMD0, 0) ) == 1 ) {

		sendStringToUART ( "my first command" );

	}

	sendStringToUART ( "end initMicrosd");

}



void initUsart( void ) {

// usart 2
// pa3 - usart_rx, pa2 - usart_tx
//сначала включаем тактирование gpioa, usart2

		RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
		RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

// настройка пинов gpio
// pa2 - tx
// pa3 - rx

		GPIOA -> MODER |= ( ( GPIO_Mode_AF << ( GPIO_PinSource2 * 2 ) ) | ( GPIO_Mode_AF << ( GPIO_PinSource3 * 2 ) ) );
		GPIOA -> OTYPER  ;                                                                                        // no change, reset value output push pull
		GPIOA -> OSPEEDR|= ( ( GPIO_Pin_2 << GPIO_Medium_Speed ) | ( GPIO_Pin_3 << GPIO_Medium_Speed ) );		  // no change, reset value low level
		GPIOA -> PUPDR  |= ( GPIO_PuPd_UP << ( GPIO_PinSource2 * 2 ) );                                  // tx - pull UP, rx - no change, reset value - NO PULL
		GPIOA -> LCKR  ; 																					// no change, reset value - no lock state
		GPIOA -> AFR[0] |= ( GPIO_AF_USART2 << ( GPIO_PinSource2 * 4 ) | GPIO_AF_USART2 << ( GPIO_PinSource3 * 4 ) );
		GPIOA -> AFR[1] ; // no change, reset value 0x00

// настройка usart
//baudrate 115200

#define BAUDRATE        115200
#define APB1_presc		8
#define APB1_freq ( SystemCoreClock / APB1_presc )

// mantissa = 11
// fraction = 0.3932 * 16 = 6,2912 = 6

		uint8_t fraction =       6;  //APB1_freq % ( 16 * BAUDRATE );
		uint16_t mantissa =     11;  //APB1_freq / ( 16 * BAUDRATE );


		USART2 -> BRR = (mantissa << 4) | fraction;
		USART2 -> CR1 = USART_WordLength_8b | USART_CR1_UE | USART_CR1_RE | USART_CR1_TE ;

}




void sendStringToUART( uint8_t * data) {

	while ( 1 ) {
		while ( ! ( ( USART2 -> SR ) & (USART_SR_TC) ) );
		if ( *data == '\0' ) {
			return;
		}
		USART2 -> DR = *data++;
	}


}


uint8_t writeData(uint8_t data) {

	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {

	}

    SPI_I2S_SendData(SPI1, data);

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) {

    }
    return SPI_I2S_ReceiveData(SPI1);
}
