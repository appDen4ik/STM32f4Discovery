################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SPL/Source/stm32f4xx_gpio.c \
../SPL/Source/stm32f4xx_rcc.c 

OBJS += \
./SPL/Source/stm32f4xx_gpio.o \
./SPL/Source/stm32f4xx_rcc.o 

C_DEPS += \
./SPL/Source/stm32f4xx_gpio.d \
./SPL/Source/stm32f4xx_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
SPL/Source/%.o: ../SPL/Source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Dassert_param(x) -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Core" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Startup" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\SPL\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


