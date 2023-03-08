################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/MCU.c \
D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/RTC.c \
D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/SLEEP.c 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/MCU.o: D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/MCU.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DDEBUG=1 -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Startup" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\APP\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\Profile\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\HAL\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Ld" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\LIB" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/RTC.o: D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/RTC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DDEBUG=1 -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Startup" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\APP\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\Profile\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\HAL\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Ld" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\LIB" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/SLEEP.o: D:/MyGithubProject/HID-Consumer-Keypad/Software/EVT/EXAM/BLE/HAL/SLEEP.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DDEBUG=1 -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Startup" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\APP\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\Peripheral\Profile\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\HAL\include" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Ld" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\LIB" -I"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

