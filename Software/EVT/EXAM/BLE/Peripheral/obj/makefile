################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/15"}
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include Profile/subdir.mk
-include HAL/subdir.mk
-include APP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Peripheral.hex \

SECONDARY_LIST += \
Peripheral.lst \

SECONDARY_SIZE += \
Peripheral.siz \


# All Target
all: Peripheral.elf secondary-outputs

# Tool invocations
Peripheral.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -T "D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\BLE\LIB" -L"D:\MyGithubProject\HID-Consumer-Keypad\Software\EVT\EXAM\SRC\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"Peripheral.map" --specs=nano.specs --specs=nosys.specs -o "Peripheral.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
Peripheral.hex: Peripheral.elf
	@	riscv-none-embed-objcopy -O ihex "Peripheral.elf"  "Peripheral.hex"
	@	@
Peripheral.lst: Peripheral.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "Peripheral.elf" > "Peripheral.lst"
	@	@
Peripheral.siz: Peripheral.elf
	@	riscv-none-embed-size --format=berkeley "Peripheral.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Peripheral.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
