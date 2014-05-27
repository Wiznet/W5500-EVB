################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Internet/DNS/dns.c 

OBJS += \
./Internet/DNS/dns.o 

C_DEPS += \
./Internet/DNS/dns.d 


# Each subdirectory must supply rules for building sources it contributes
Internet/DNS/%.o: ../Internet/DNS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_LPCOPEN -D__LPC11EXX__ -I"D:\WIZnet\__Purple\___LPCXpresso_Workspace\wiznet_evb_w5500evb_board\inc" -I"D:\WIZnet\__Purple\___LPCXpresso_Workspace\lpc_chip_11exx\inc" -I"D:\WIZnet\__Purple\___LPCXpresso_Workspace\ioLibrary\Ethernet" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


