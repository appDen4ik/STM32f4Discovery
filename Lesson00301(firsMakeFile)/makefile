
FLAGS = -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections


all: sec.hex


main.o: main.c
	arm-none-eabi-gcc $(FLAGS) -I"E:\workspace\STM\Lesson00301(firsMakeFile)" -c -o "E:\workspace\STM\Lesson00301(firsMakeFile)\main.o"  "E:\workspace\STM\Lesson00301(firsMakeFile)\main.c" 

system_stm32f4xx.o: system_stm32f4xx.c
	arm-none-eabi-gcc $(FLAGS) -I"E:\workspace\STM\Lesson00301(firsMakeFile)" -c -o "E:\workspace\STM\Lesson00301(firsMakeFile)\system_stm32f4xx.o"  "E:\workspace\STM\Lesson00301(firsMakeFile)\system_stm32f4xx.c" 

startup_stm32f40_41xxx.o: startup_stm32f40xx.S
	arm-none-eabi-gcc $(FLAGS) -I"E:\workspace\STM\Lesson00301(firsMakeFile)" -c -o "E:\workspace\STM\Lesson00301(firsMakeFile)\startup_stm32f40xx.o"  "E:\workspace\STM\Lesson00301(firsMakeFile)\startup_stm32f40xx.S" 

sec.elf: main.o system_stm32f4xx.o startup_stm32f40xx.o
	arm-none-eabi-gcc $(FLAGS) -T "E:\workspace\STM\sec\src\STM32F417IG_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"sec.map" -o "sec.elf" "E:\workspace\STM\Lesson00301(firsMakeFile)\main.o" "E:\workspace\STM\Lesson00301(firsMakeFile)\system_stm32f4xx.o" "E:\workspace\STM\Lesson00301(firsMakeFile)\startup_stm32f40xx.o"

sec.hex: sec.elf
	arm-none-eabi-objcopy -O ihex "sec.elf"  "sec.hex"

clean:
	-rm -rf sec.hex