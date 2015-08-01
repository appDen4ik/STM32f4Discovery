
FLAGS = -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections

PROJECT_LOC = D:\git\STM32f4Discovery\Lesson00301(firsMakeFile)

all: sec.hex


main.o: main.c
	arm-none-eabi-gcc $(FLAGS) -I"$(PROJECT_LOC)" -c -o "$(PROJECT_LOC)\main.o"  "$(PROJECT_LOC)\main.c" 

system_stm32f4xx.o: system_stm32f4xx.c
	arm-none-eabi-gcc $(FLAGS) -I"${PROJECT_LOC}" -c -o "${PROJECT_LOC}\system_stm32f4xx.o"  "${PROJECT_LOC}\system_stm32f4xx.c" 

startup_stm32f40xx.o: startup_stm32f40xx.S
	arm-none-eabi-gcc $(FLAGS) -I"$(PROJECT_LOC)" -c -o "$(PROJECT_LOC)\startup_stm32f40xx.o"  "$(PROJECT_LOC)\startup_stm32f40xx.S" 

sec.elf: main.o system_stm32f4xx.o startup_stm32f40xx.o
	arm-none-eabi-gcc $(FLAGS) -T "${PROJECT_LOC}\STM32F417IG_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"sec.map" -o "sec.elf" "${PROJECT_LOC}\main.o" "${PROJECT_LOC}\system_stm32f4xx.o" "${PROJECT_LOC}\startup_stm32f40xx.o"

sec.hex: sec.elf
	arm-none-eabi-objcopy -O ihex "sec.elf"  "sec.hex"

clean:
	-rm -rf sec.hex