################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/ioLibrary/Internet/SNMP/subdir.mk
-include src/ioLibrary/Internet/DNS/subdir.mk
-include src/ioLibrary/Internet/DHCP/subdir.mk
-include src/ioLibrary/Ethernet/W5500/subdir.mk
-include src/ioLibrary/Ethernet/subdir.mk
-include src/PlatformHandler/subdir.mk
-include src/Application/subdir.mk
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
all: SNMP_LPC11E36_LPCXpresso.axf

# Tool invocations
SNMP_LPC11E36_LPCXpresso.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\wiznet_evb_w5500evb_board\Release" -L"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\lpc_chip_11exx\Release" -Xlinker -Map="SNMP_LPC11E36_LPCXpresso.map" -Xlinker --gc-sections -Xlinker --allow-multiple-definition -mcpu=cortex-m0 -mthumb -T "SNMP_LPC11E36_LPCXpresso_Release.ld" -o "SNMP_LPC11E36_LPCXpresso.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SNMP_LPC11E36_LPCXpresso.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "SNMP_LPC11E36_LPCXpresso.axf" ; arm-none-eabi-objcopy -O ihex "SNMP_LPC11E36_LPCXpresso.axf" "SNMP_LPC11E36_LPCXpresso.hex" ; arm-none-eabi-objcopy -v -O binary "SNMP_LPC11E36_LPCXpresso.axf" "SNMP_LPC11E36_LPCXpresso.bin" ; checksum -p LPC11E36_501 -d "SNMP_LPC11E36_LPCXpresso.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
