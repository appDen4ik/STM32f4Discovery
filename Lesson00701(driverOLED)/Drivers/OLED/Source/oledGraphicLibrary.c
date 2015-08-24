/**
 ***************************************************************************
  * @file    oled1GraphicLibrary.c
  * @author  Denis
  * @version V0.1
  * @date    19-Aug-2015
  * @brief   Библиотечка для работы с экранчиком oled128x64
  * 		 реализованы функции:
  * 		  - инициализация экранчика
  * 		  - установка произвольной точки на экране
  * 		  - очистка экранчика
 ***************************************************************************
*/

#include "oled128x64.h"
#include "oledGraphicLibrary.h"

extern uint8_t oledDRAM[8][128];

void initOLED( void ) {

	initI2C();

	writeOneByte( 0b0, 0xAE );//Display ON

	writeOneByte( 0b0, 0xA8 );//Set MUX
	writeOneByte( 0b0, 0x3F );

	writeOneByte( 0b0, 0xD3 );//Set Display Offset
	writeOneByte( 0b0, 0x0 );

	writeOneByte( 0b0, 0x40 );//Set display start adress

	writeOneByte( 0b0, 0xA1 );//Set segment re-map

	writeOneByte( 0b0, 0xc0 );//Set COM output scan direction c8

	writeOneByte( 0b0, 0xda );//Set Com Pin hardware configuration 02
	writeOneByte( 0b0, 18 );

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

	clearOLED();


	writeOneByte( 0b0, 0x22 );//Set Page Address
	writeOneByte( 0b0, 0x00 );
	writeOneByte( 0b0, 0x07 );

	writeOneByte( 0b0, 0x21 );//Set Column Address
	writeOneByte( 0b0, 0x00 );
	writeOneByte( 0b0, 127 );
}

void setDot( uint8_t x, uint8_t y ){
	//x - 1..128, y - 1..64
	//сначала нужно определить какая страница

	uint8_t byte = 0x01;


	x--;
	y--;

	if (x < 128 && y < 64){

		//налаживаем маску и записываем сначала в отпечаток
		oledDRAM[y/8][x] |= byte << y%8;

		//затем в физ. экранчик
		writeOneByte( 0b0, 0x22 );//( Page Address )
		writeOneByte( 0b0, y/8 );
		writeOneByte( 0b0, 0x07 );

		writeOneByte( 0b0, 0x21 );//Set X ( Column Address )
		writeOneByte( 0b0, x );
		writeOneByte( 0b0, 127 );

		writeOneByte( 0b01000000,   oledDRAM[y/8][x] );
	}
}


void clearOLED( void ){
	uint8_t column;
	uint8_t page;
	const uint16_t  sizeimprint = sizeof(oledDRAM);
	uint8_t *point = oledDRAM;

	uint32_t i;

	writeOneByte( 0b0, 0x22 );//Set Page Address
	writeOneByte( 0b0, 0x00 );
	writeOneByte( 0b0, 0x07 );

	writeOneByte( 0b0, 0x21 );//Set Column Address
	writeOneByte( 0b0, 0x00 );
	writeOneByte( 0b0, 127 );

	for ( page = 0; page < 8; page++ ){
		for (column = 0; column < 128; column++) {
				writeOneByte( 0b01000000, 0x0 );
			}
	}
	//затем чистим отпечаток
	for ( i = 0; i < sizeimprint; i++ ) {
		*point++ = 0;
	}

}


void del(uint64_t i){
	for (; i > 0; --i);
}

/* Функция печатает точки, определяющие окружность */
static void plot_circle(int16_t x, int16_t y, int16_t x_center, int16_t y_center){
  int16_t  x1, y1;

  for (x1=x;x1<x+1;++x1){
	  del(200000);
	  setDot(x1+x_center,y+y_center);
	  del(200000);
	  setDot(x1+x_center,y_center-y);
	  del(200000);
	  setDot(x_center-x1,y+y_center);
	  del(200000);
	  setDot(x_center-x1,y_center-y);
	  del(200000);
  }
  for (y1=y;y1<y+1;++y1){
	  del(200000);
	  setDot(y1+x_center,x+y_center);
	  del(200000);
	  setDot(y1+x_center,y_center-x);
	  del(200000);
	  setDot(x_center-y1,x+y_center);
	  del(200000);
	  setDot(x_center-y1,y_center-x);
	  del(200000);
  }
}

void circle(int16_t x_center, int16_t y_center, int16_t radius){
	int16_t x,y,delta;

  y=radius;
  delta=3-2*radius;

  for ( x=0; x<y; ){
		plot_circle(x,y,x_center,y_center);
		if (delta<0)
			delta+=4*x+6;
		else {
			delta+=4*(x-y)+10;
			y--;
		}
		x++;
  }
  x=y;
  if (y) plot_circle(x,y,x_center,y_center);
}

/* Вычерчивание линии заданного цвета с использованием
   алгоритма Брезенхама */
 void line(int16_t startx, int16_t starty, int16_t endx, int16_t endy){
   uint32_t t, distance;
   int32_t xerr=0, yerr=0, delta_x, delta_y;
   int32_t incx, incy;

 /* вычисление расстояния в обоих направлениях  */
   delta_x=endx-startx;
   delta_y=endy-starty;

 /* определение направления шага,
    шаг вычисляется либо по вертикальной, либо горизонтальной
    линии   */
    if (delta_x>0) incx=1;
    else  if (delta_x==0) incx=0;
    else  incx= -1;
    if (delta_y>0) incy=1;
    else  if (delta_y==0) incy=0;
    else  incy= -1;

  /* определение какое расстояние больше */
//          delta_x = abs(delta_x);
//          delta_y=abs(delta_y);
    if (delta_x>delta_y) distance=delta_x;
    else distance=delta_y;

  /* вычерчивание линии */
    for (t=0; t <= distance + 1; t++) {
    	setDot(startx,starty);
       xerr+=delta_x;
       yerr+=delta_y;
       if (xerr>distance) {
          xerr-=distance;
          startx+=incx;
       }
       if (yerr>distance) {
          yerr-=distance;
          starty+=incy;
       }
    }
 }
