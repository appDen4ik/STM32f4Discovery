################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../CMSIS/Startup/startup_stm32f40xx.S 

OBJS += \
./CMSIS/Startup/startup_stm32f40xx.o 

S_UPPER_DEPS += \
./CMSIS/Startup/startup_stm32f40xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/Startup/%.o: ../CMSIS/Startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


