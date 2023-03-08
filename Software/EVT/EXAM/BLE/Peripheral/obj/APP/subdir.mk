################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/battery.c \
../APP/encoder.c \
../APP/hidconsumer.c \
../APP/peripheral.c \
../APP/peripheral_main.c \
../APP/usb_hid.c \
../APP/user_hid_task.c \
../APP/user_task.c \
../APP/vibrator.c \
../APP/ws2812.c 

OBJS += \
./APP/battery.o \
./APP/encoder.o \
./APP/hidconsumer.o \
./APP/peripheral.o \
./APP/peripheral_main.o \
./APP/usb_hid.o \
./APP/user_hid_task.o \
./APP/user_task.o \
./APP/vibrator.o \
./APP/ws2812.o 

C_DEPS += \
./APP/battery.d \
./APP/encoder.d \
./APP/hidconsumer.d \
./APP/peripheral.d \
./APP/peripheral_main.d \
./APP/usb_hid.d \
./APP/user_hid_task.d \
./APP/user_task.d \
./APP/vibrator.d \
./APP/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DDEBUG=1 -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Startup" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\APP\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\Profile\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\HAL\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Ld" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\LIB" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

