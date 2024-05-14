################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DISPLAY/Target/key_io.c \
../DISPLAY/Target/lcd_io.c \
../DISPLAY/Target/lcd_os.c \
../DISPLAY/Target/mem_io.c \
../DISPLAY/Target/mem_os.c 

OBJS += \
./DISPLAY/Target/key_io.o \
./DISPLAY/Target/lcd_io.o \
./DISPLAY/Target/lcd_os.o \
./DISPLAY/Target/mem_io.o \
./DISPLAY/Target/mem_os.o 

C_DEPS += \
./DISPLAY/Target/key_io.d \
./DISPLAY/Target/lcd_io.d \
./DISPLAY/Target/lcd_os.d \
./DISPLAY/Target/mem_io.d \
./DISPLAY/Target/mem_os.d 


# Each subdirectory must supply rules for building sources it contributes
DISPLAY/Target/%.o DISPLAY/Target/%.su DISPLAY/Target/%.cyclo: ../DISPLAY/Target/%.c DISPLAY/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/Fonts" -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/lcd" -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../DISPLAY/App -I../DISPLAY/Target -I../Drivers/BSP/Components/mx25l6433f -I../Drivers/BSP/Components/Common -I../Drivers/BSP/Components/st7789v -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DISPLAY-2f-Target

clean-DISPLAY-2f-Target:
	-$(RM) ./DISPLAY/Target/key_io.cyclo ./DISPLAY/Target/key_io.d ./DISPLAY/Target/key_io.o ./DISPLAY/Target/key_io.su ./DISPLAY/Target/lcd_io.cyclo ./DISPLAY/Target/lcd_io.d ./DISPLAY/Target/lcd_io.o ./DISPLAY/Target/lcd_io.su ./DISPLAY/Target/lcd_os.cyclo ./DISPLAY/Target/lcd_os.d ./DISPLAY/Target/lcd_os.o ./DISPLAY/Target/lcd_os.su ./DISPLAY/Target/mem_io.cyclo ./DISPLAY/Target/mem_io.d ./DISPLAY/Target/mem_io.o ./DISPLAY/Target/mem_io.su ./DISPLAY/Target/mem_os.cyclo ./DISPLAY/Target/mem_os.d ./DISPLAY/Target/mem_os.o ./DISPLAY/Target/mem_os.su

.PHONY: clean-DISPLAY-2f-Target

