################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SPL/Source/subdir.mk
-include CMSIS/Startup/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Lesson00801(WS2801).hex \

SECONDARY_SIZE += \
Lesson00801(WS2801).siz \


# All Target
all: Lesson00801(WS2801).elf secondary-outputs

# Tool invocations
Lesson00801(WS2801).elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "D:\git\STM32f4Discovery\Lesson00801(WS2801)\CMSIS\STM32F417IG_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"Lesson00801(WS2801).map" -o "Lesson00801(WS2801).elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Lesson00801(WS2801).hex: Lesson00801(WS2801).elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "Lesson00801(WS2801).elf"  "Lesson00801(WS2801).hex"
	@echo 'Finished building: $@'
	@echo ' '

Lesson00801(WS2801).siz: Lesson00801(WS2801).elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "Lesson00801(WS2801).elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Lesson00801(WS2801).elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
