"C:\Program Files (x86)\Atollic\TrueSTUDIO for STM32 9.0.1\ARMTools\bin\arm-atollic-eabi-gcc.exe" -c -mthumb -mcpu=cortex-m3 -g -Wa,-adhln -x assembler-with-cpp -specs=nano.specs -o boot_block.o boot_block.s > boot_block.lst

"C:\Program Files (x86)\Atollic\TrueSTUDIO for STM32 9.0.1\ARMTools\bin\arm-atollic-eabi-objcopy.exe" -O binary boot_block.o boot_block.bin