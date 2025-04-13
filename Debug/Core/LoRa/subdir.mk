################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/LoRa/LoRa.c 

OBJS += \
./Core/LoRa/LoRa.o 

C_DEPS += \
./Core/LoRa/LoRa.d 


# Each subdirectory must supply rules for building sources it contributes
Core/LoRa/%.o Core/LoRa/%.su Core/LoRa/%.cyclo: ../Core/LoRa/%.c Core/LoRa/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-LoRa

clean-Core-2f-LoRa:
	-$(RM) ./Core/LoRa/LoRa.cyclo ./Core/LoRa/LoRa.d ./Core/LoRa/LoRa.o ./Core/LoRa/LoRa.su

.PHONY: clean-Core-2f-LoRa

