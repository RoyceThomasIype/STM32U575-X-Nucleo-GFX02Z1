################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DISPLAY/App/Fonts/font12.c \
../DISPLAY/App/Fonts/font16.c \
../DISPLAY/App/Fonts/font20.c \
../DISPLAY/App/Fonts/font24.c \
../DISPLAY/App/Fonts/font8.c 

OBJS += \
./DISPLAY/App/Fonts/font12.o \
./DISPLAY/App/Fonts/font16.o \
./DISPLAY/App/Fonts/font20.o \
./DISPLAY/App/Fonts/font24.o \
./DISPLAY/App/Fonts/font8.o 

C_DEPS += \
./DISPLAY/App/Fonts/font12.d \
./DISPLAY/App/Fonts/font16.d \
./DISPLAY/App/Fonts/font20.d \
./DISPLAY/App/Fonts/font24.d \
./DISPLAY/App/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
DISPLAY/App/Fonts/%.o DISPLAY/App/Fonts/%.su DISPLAY/App/Fonts/%.cyclo: ../DISPLAY/App/Fonts/%.c DISPLAY/App/Fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../DISPLAY/App -I../DISPLAY/Target -I../Drivers/BSP/Components/mx25l6433f -I../Drivers/BSP/Components/Common -I../Drivers/BSP/Components/ili9341 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DISPLAY-2f-App-2f-Fonts

clean-DISPLAY-2f-App-2f-Fonts:
	-$(RM) ./DISPLAY/App/Fonts/font12.cyclo ./DISPLAY/App/Fonts/font12.d ./DISPLAY/App/Fonts/font12.o ./DISPLAY/App/Fonts/font12.su ./DISPLAY/App/Fonts/font16.cyclo ./DISPLAY/App/Fonts/font16.d ./DISPLAY/App/Fonts/font16.o ./DISPLAY/App/Fonts/font16.su ./DISPLAY/App/Fonts/font20.cyclo ./DISPLAY/App/Fonts/font20.d ./DISPLAY/App/Fonts/font20.o ./DISPLAY/App/Fonts/font20.su ./DISPLAY/App/Fonts/font24.cyclo ./DISPLAY/App/Fonts/font24.d ./DISPLAY/App/Fonts/font24.o ./DISPLAY/App/Fonts/font24.su ./DISPLAY/App/Fonts/font8.cyclo ./DISPLAY/App/Fonts/font8.d ./DISPLAY/App/Fonts/font8.o ./DISPLAY/App/Fonts/font8.su

.PHONY: clean-DISPLAY-2f-App-2f-Fonts

