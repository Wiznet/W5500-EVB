################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: w5500evb_onboard_potentiometer.axf

# Tool invocations
w5500evb_onboard_potentiometer.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\WIZnet\__Purple\___LPCXpresso_Workspace\wiznet_evb_w5500evb_board\Debug" -L"D:\WIZnet\__Purple\___LPCXpresso_Workspace\lpc_chip_11exx\Debug" -L"D:\WIZnet\__Purple\___LPCXpresso_Workspace\ioLibrary\Debug" -Xlinker -Map="w5500evb_onboard_potentiometer.map" -Xlinker --gc-sections -Xlinker --allow-multiple-definition -mcpu=cortex-m0 -mthumb -T "w5500evb_onboard_potentiometer_Debug.ld" -o "w5500evb_onboard_potentiometer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) w5500evb_onboard_potentiometer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "w5500evb_onboard_potentiometer.axf" ; arm-none-eabi-objcopy -O ihex "w5500evb_onboard_potentiometer.axf" "w5500evb_onboard_potentiometer.hex" ; arm-none-eabi-objcopy -O binary "w5500evb_onboard_potentiometer.axf" "w5500evb_onboard_potentiometer.bin" ; checksum -p LPC11E36_501 -d "w5500evb_onboard_potentiometer.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
