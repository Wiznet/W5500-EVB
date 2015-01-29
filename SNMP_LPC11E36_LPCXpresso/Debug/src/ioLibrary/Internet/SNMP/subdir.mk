################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ioLibrary/Internet/SNMP/snmp.c \
../src/ioLibrary/Internet/SNMP/snmp_custom.c 

OBJS += \
./src/ioLibrary/Internet/SNMP/snmp.o \
./src/ioLibrary/Internet/SNMP/snmp_custom.o 

C_DEPS += \
./src/ioLibrary/Internet/SNMP/snmp.d \
./src/ioLibrary/Internet/SNMP/snmp_custom.d 


# Each subdirectory must supply rules for building sources it contributes
src/ioLibrary/Internet/SNMP/%.o: ../src/ioLibrary/Internet/SNMP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_ROMDIVIDE -D__USE_LPCOPEN -D__LPC11EXX__ -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\wiznet_evb_w5500evb_board\inc" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\lpc_chip_11exx\inc" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\ioLibrary\Ethernet" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\Application" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\ioLibrary\Ethernet\W5500" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\ioLibrary\Internet\DHCP" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\ioLibrary\Internet\DNS" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\ioLibrary\Internet\SNMP" -I"D:\WIZnet\__Purple\W5500_EVB\workspace\snmp\SNMP_LPC11E36_LPCXpresso\src\PlatformHandler" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


