################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mainL00801.c 

OBJS += \
./mainL00801.o 

C_DEPS += \
./mainL00801.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -Dassert_param(x) -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Core" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\Startup" -I"D:\git\STM32f4Discovery\Lesson00801(WS2801)\SPL\Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


