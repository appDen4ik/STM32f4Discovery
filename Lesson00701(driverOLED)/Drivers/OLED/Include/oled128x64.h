
#ifndef DRIVERS_OLED_INCLUDE_OLED128X64_H_
#define DRIVERS_OLED_INCLUDE_OLED128X64_H_

#include <stdint.h>
#define OLED_ADRESS 0x78


void writeOneByte( uint8_t adress, uint8_t data );
void initI2C( void );

#endif /* DRIVERS_OLED_INCLUDE_OLED128X64_H_ */
