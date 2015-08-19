

#ifndef DRIVERS_OLED_INCLUDE_OLEDGRAPHICLIBRARY_H_
#define DRIVERS_OLED_INCLUDE_OLEDGRAPHICLIBRARY_H_

#include <stdint.h>

void initOLED( void );
void setDot( uint8_t x, uint8_t y);
void clearOLED( void );
void line(int16_t startx, int16_t starty, int16_t endx, int16_t endy );
void circle(int16_t x_center, int16_t y_center, int16_t radius );

#endif /* DRIVERS_OLED_INCLUDE_OLEDGRAPHICLIBRARY_H_ */
