################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc_1125.c \
../src/adc_11xx.c \
../src/chip_11xx.c \
../src/clock_11xx.c \
../src/gpio_11xx_1.c \
../src/gpio_11xx_2.c \
../src/gpiogroup_11xx.c \
../src/i2c_11xx.c \
../src/i2cm_11xx.c \
../src/iocon_11xx.c \
../src/pinint_11xx.c \
../src/pmu_11xx.c \
../src/ring_buffer.c \
../src/ssp_11xx.c \
../src/sysctl_11xx.c \
../src/sysinit_11xx.c \
../src/timer_11xx.c \
../src/uart_11xx.c \
../src/wwdt_11xx.c 

OBJS += \
./src/adc_1125.o \
./src/adc_11xx.o \
./src/chip_11xx.o \
./src/clock_11xx.o \
./src/gpio_11xx_1.o \
./src/gpio_11xx_2.o \
./src/gpiogroup_11xx.o \
./src/i2c_11xx.o \
./src/i2cm_11xx.o \
./src/iocon_11xx.o \
./src/pinint_11xx.o \
./src/pmu_11xx.o \
./src/ring_buffer.o \
./src/ssp_11xx.o \
./src/sysctl_11xx.o \
./src/sysinit_11xx.o \
./src/timer_11xx.o \
./src/uart_11xx.o \
./src/wwdt_11xx.o 

C_DEPS += \
./src/adc_1125.d \
./src/adc_11xx.d \
./src/chip_11xx.d \
./src/clock_11xx.d \
./src/gpio_11xx_1.d \
./src/gpio_11xx_2.d \
./src/gpiogroup_11xx.d \
./src/i2c_11xx.d \
./src/i2cm_11xx.d \
./src/iocon_11xx.d \
./src/pinint_11xx.d \
./src/pmu_11xx.d \
./src/ring_buffer.d \
./src/ssp_11xx.d \
./src/sysctl_11xx.d \
./src/sysinit_11xx.d \
./src/timer_11xx.d \
./src/uart_11xx.d \
./src/wwdt_11xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M0 -I"D:\WIZnet\__Purple\___LPCXpresso_Workspace\lpc_chip_11exx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


