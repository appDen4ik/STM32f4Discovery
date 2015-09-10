################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_stm32f4xx.c 

OBJS += \
./CMSIS/system_stm32f4xx.o 

C_DEPS += \
./CMSIS/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Dassert_param(x) -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Core" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Startup" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\SPL\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


