################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.c 

OBJS += \
./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.o 

C_DEPS += \
./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mx25l6433f/ospi/%.o Drivers/BSP/Components/mx25l6433f/ospi/%.su Drivers/BSP/Components/mx25l6433f/ospi/%.cyclo: ../Drivers/BSP/Components/mx25l6433f/ospi/%.c Drivers/BSP/Components/mx25l6433f/ospi/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/Fonts" -I"C:/Users/royce/Downloads/en.X-CUBE-DISPLAY.3.0.0/Utilities/lcd" -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../DISPLAY/App -I../DISPLAY/Target -I../Drivers/BSP/Components/mx25l6433f -I../Drivers/BSP/Components/Common -I../Drivers/BSP/Components/st7789v -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-mx25l6433f-2f-ospi

clean-Drivers-2f-BSP-2f-Components-2f-mx25l6433f-2f-ospi:
	-$(RM) ./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.cyclo ./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.d ./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.o ./Drivers/BSP/Components/mx25l6433f/ospi/mx25l6433f.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-mx25l6433f-2f-ospi

