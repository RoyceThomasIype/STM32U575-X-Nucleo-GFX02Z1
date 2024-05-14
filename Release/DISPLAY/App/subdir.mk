################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DISPLAY/App/app_display.c \
../DISPLAY/App/stm32_lcd.c 

OBJS += \
./DISPLAY/App/app_display.o \
./DISPLAY/App/stm32_lcd.o 

C_DEPS += \
./DISPLAY/App/app_display.d \
./DISPLAY/App/stm32_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
DISPLAY/App/%.o DISPLAY/App/%.su DISPLAY/App/%.cyclo: ../DISPLAY/App/%.c DISPLAY/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/Fonts" -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/lcd" -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../DISPLAY/App -I../DISPLAY/Target -I../Drivers/BSP/Components/mx25l6433f -I../Drivers/BSP/Components/Common -I../Drivers/BSP/Components/st7789v -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DISPLAY-2f-App

clean-DISPLAY-2f-App:
	-$(RM) ./DISPLAY/App/app_display.cyclo ./DISPLAY/App/app_display.d ./DISPLAY/App/app_display.o ./DISPLAY/App/app_display.su ./DISPLAY/App/stm32_lcd.cyclo ./DISPLAY/App/stm32_lcd.d ./DISPLAY/App/stm32_lcd.o ./DISPLAY/App/stm32_lcd.su

.PHONY: clean-DISPLAY-2f-App

