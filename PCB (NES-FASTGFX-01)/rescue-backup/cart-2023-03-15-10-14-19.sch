EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:cart-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L NES-CART J1
U 1 1 5BBB307F
P 1950 2650
F 0 "J1" H 1650 4700 60  0000 C CNN
F 1 "NES-CART" H 1850 500 60  0000 C CNN
F 2 "cart_interface:NES_CART_FINGERS" H 1950 2850 60  0001 C CNN
F 3 "" H 1950 2850 60  0000 C CNN
	1    1950 2650
	1    0    0    -1  
$EndComp
Text GLabel 1400 850  0    60   Input ~ 0
CPU_A11
Text GLabel 1400 950  0    60   Input ~ 0
CPU_A10
Text GLabel 1400 1050 0    60   Input ~ 0
CPU_A9
Text GLabel 1400 1150 0    60   Input ~ 0
CPU_A8
Text GLabel 1400 1250 0    60   Input ~ 0
CPU_A7
Text GLabel 1400 1350 0    60   Input ~ 0
CPU_A6
Text GLabel 1400 1450 0    60   Input ~ 0
CPU_A5
Text GLabel 1400 1550 0    60   Input ~ 0
CPU_A4
Text GLabel 1400 1650 0    60   Input ~ 0
CPU_A3
Text GLabel 1400 1750 0    60   Input ~ 0
CPU_A2
Text GLabel 1400 1850 0    60   Input ~ 0
CPU_A1
Text GLabel 1400 1950 0    60   Input ~ 0
CPU_A0
Text GLabel 1400 2050 0    60   Input ~ 0
CPU_R/W
NoConn ~ 1400 2350
NoConn ~ 1400 2450
NoConn ~ 1400 2550
NoConn ~ 1400 2650
NoConn ~ 1400 2750
NoConn ~ 2500 2350
NoConn ~ 2500 2650
NoConn ~ 2500 2750
NoConn ~ 1400 2150
NoConn ~ 2500 750 
Text GLabel 1400 3050 0    60   Input ~ 0
~PPU_RD
Text GLabel 1400 3150 0    60   Input ~ 0
NTRAM_A10
Text GLabel 1400 3250 0    60   Input ~ 0
PPU_A6
Text GLabel 1400 3350 0    60   Input ~ 0
PPU_A5
Text GLabel 1400 3450 0    60   Input ~ 0
PPU_A4
Text GLabel 1400 3550 0    60   Input ~ 0
PPU_A3
Text GLabel 1400 3650 0    60   Input ~ 0
PPU_A2
Text GLabel 1400 3750 0    60   Input ~ 0
PPU_A1
Text GLabel 1400 3850 0    60   Input ~ 0
PPU_A0
Text GLabel 1400 3950 0    60   Input ~ 0
PPU_D0
Text GLabel 1400 4050 0    60   Input ~ 0
PPU_D1
Text GLabel 1400 4150 0    60   Input ~ 0
PPU_D2
Text GLabel 1400 4250 0    60   Input ~ 0
PPU_D3
Text GLabel 2500 850  2    60   Input ~ 0
M2
Text GLabel 2500 950  2    60   Input ~ 0
CPU_A12
Text GLabel 2500 1050 2    60   Input ~ 0
CPU_A13
Text GLabel 2500 1150 2    60   Input ~ 0
CPU_A14
Text GLabel 2500 1250 2    60   Input ~ 0
CPU_D7
Text GLabel 2500 1350 2    60   Input ~ 0
CPU_D6
Text GLabel 2500 1450 2    60   Input ~ 0
CPU_D5
Text GLabel 2500 1550 2    60   Input ~ 0
CPU_D4
Text GLabel 2500 1650 2    60   Input ~ 0
CPU_D3
Text GLabel 2500 1750 2    60   Input ~ 0
CPU_D2
Text GLabel 2500 1850 2    60   Input ~ 0
CPU_D1
Text GLabel 2500 1950 2    60   Input ~ 0
CPU_D0
Text GLabel 2500 2050 2    60   Input ~ 0
/ROMSEL
Text GLabel 2500 2950 2    60   Input ~ 0
~PPU_WR
Text GLabel 2500 3250 2    60   Input ~ 0
PPU_A7
Text GLabel 2500 3350 2    60   Input ~ 0
PPU_A8
Text GLabel 2500 3450 2    60   Input ~ 0
PPU_A9
Text GLabel 2500 3550 2    60   Input ~ 0
PPU_A11
Text GLabel 2500 3650 2    60   Input ~ 0
PPU_A10
Text GLabel 2500 3750 2    60   Input ~ 0
PPU_A12
Text GLabel 2500 3950 2    60   Input ~ 0
PPU_D7
Text GLabel 2500 4050 2    60   Input ~ 0
PPU_D6
Text GLabel 2500 4150 2    60   Input ~ 0
PPU_D5
Text GLabel 2500 4250 2    60   Input ~ 0
PPU_D4
$Comp
L GND #PWR01
U 1 1 5BBB3C7B
P 2500 4650
F 0 "#PWR01" H 2500 4400 50  0001 C CNN
F 1 "GND" H 2500 4500 50  0000 C CNN
F 2 "" H 2500 4650 50  0000 C CNN
F 3 "" H 2500 4650 50  0000 C CNN
	1    2500 4650
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR02
U 1 1 5BBB3D48
P 1400 4650
F 0 "#PWR02" H 1400 4500 50  0001 C CNN
F 1 "VCC" H 1400 4800 50  0000 C CNN
F 2 "" H 1400 4650 50  0000 C CNN
F 3 "" H 1400 4650 50  0000 C CNN
	1    1400 4650
	-1   0    0    1   
$EndComp
$Comp
L VCC #PWR03
U 1 1 5BBB4124
P 7350 700
F 0 "#PWR03" H 7350 550 50  0001 C CNN
F 1 "VCC" H 7350 850 50  0000 C CNN
F 2 "" H 7350 700 50  0000 C CNN
F 3 "" H 7350 700 50  0000 C CNN
	1    7350 700 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5BBB4198
P 7350 3100
F 0 "#PWR04" H 7350 2850 50  0001 C CNN
F 1 "GND" H 7350 2950 50  0000 C CNN
F 2 "" H 7350 3100 50  0000 C CNN
F 3 "" H 7350 3100 50  0000 C CNN
	1    7350 3100
	1    0    0    -1  
$EndComp
Text GLabel 6750 1000 0    60   Input ~ 0
CPU_A0
Text GLabel 6750 1100 0    60   Input ~ 0
CPU_A1
Text GLabel 6750 1200 0    60   Input ~ 0
CPU_A2
Text GLabel 6750 1300 0    60   Input ~ 0
CPU_A3
Text GLabel 6750 1400 0    60   Input ~ 0
CPU_A4
Text GLabel 6750 1500 0    60   Input ~ 0
CPU_A5
Text GLabel 6750 1600 0    60   Input ~ 0
CPU_A6
Text GLabel 6750 1700 0    60   Input ~ 0
CPU_A7
Text GLabel 6750 1800 0    60   Input ~ 0
CPU_A8
Text GLabel 6750 1900 0    60   Input ~ 0
CPU_A9
Text GLabel 6750 2000 0    60   Input ~ 0
CPU_A10
Text GLabel 6750 2100 0    60   Input ~ 0
CPU_A11
Text GLabel 6750 2200 0    60   Input ~ 0
CPU_A12
Text GLabel 6750 2300 0    60   Input ~ 0
PRG_RAM_A13
Text GLabel 6750 2400 0    60   Input ~ 0
PRG_RAM_A14
Text GLabel 6750 2600 0    60   Input ~ 0
/PRG_RAM_CS
Text GLabel 6750 2800 0    60   Input ~ 0
CPU_R/W
$Comp
L GND #PWR05
U 1 1 5BBB4411
P 6200 2850
F 0 "#PWR05" H 6200 2600 50  0001 C CNN
F 1 "GND" H 6200 2700 50  0000 C CNN
F 2 "" H 6200 2850 50  0000 C CNN
F 3 "" H 6200 2850 50  0000 C CNN
	1    6200 2850
	1    0    0    -1  
$EndComp
Text GLabel 7950 1000 2    60   Input ~ 0
CPU_D0
Text GLabel 7950 1100 2    60   Input ~ 0
CPU_D1
Text GLabel 7950 1200 2    60   Input ~ 0
CPU_D2
Text GLabel 7950 1300 2    60   Input ~ 0
CPU_D3
Text GLabel 7950 1400 2    60   Input ~ 0
CPU_D4
Text GLabel 7950 1500 2    60   Input ~ 0
CPU_D5
Text GLabel 7950 1600 2    60   Input ~ 0
CPU_D6
Text GLabel 7950 1700 2    60   Input ~ 0
CPU_D7
$Comp
L 74HC590 U4
U 1 1 5BBB5C47
P 4600 1200
F 0 "U4" H 4750 1800 50  0000 C CNN
F 1 "74HC590" H 4600 600 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 4600 1200 50  0001 C CNN
F 3 "" H 4600 1200 50  0000 C CNN
	1    4600 1200
	1    0    0    -1  
$EndComp
$Comp
L 74HC590 U5
U 1 1 5BBB5C8C
P 4600 2600
F 0 "U5" H 4750 3200 50  0000 C CNN
F 1 "74HC590" H 4600 2000 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 4600 2600 50  0001 C CNN
F 3 "" H 4600 2600 50  0000 C CNN
	1    4600 2600
	1    0    0    -1  
$EndComp
Text GLabel 5300 750  2    60   Input ~ 0
CPU_D0
Text GLabel 5300 850  2    60   Input ~ 0
CPU_D1
Text GLabel 5300 950  2    60   Input ~ 0
CPU_D2
Text GLabel 5300 1050 2    60   Input ~ 0
CPU_D3
Text GLabel 5300 1150 2    60   Input ~ 0
CPU_D4
Text GLabel 5300 1250 2    60   Input ~ 0
CPU_D5
Text GLabel 5300 1350 2    60   Input ~ 0
CPU_D6
Text GLabel 5300 1450 2    60   Input ~ 0
CPU_D7
Text GLabel 5300 2150 2    60   Input ~ 0
CPU_D0
Text GLabel 5300 2250 2    60   Input ~ 0
CPU_D1
Text GLabel 5300 2350 2    60   Input ~ 0
CPU_D2
Text GLabel 5300 2450 2    60   Input ~ 0
CPU_D3
Text GLabel 5300 2550 2    60   Input ~ 0
CPU_D4
Text GLabel 5300 2650 2    60   Input ~ 0
CPU_D5
Text GLabel 5300 2750 2    60   Input ~ 0
CPU_D6
Text GLabel 5300 2850 2    60   Input ~ 0
CPU_D7
Text GLabel 3600 750  0    60   Input ~ 0
/TSC0_OS
Text GLabel 3600 2150 0    60   Input ~ 0
/TSC1_OS
Text GLabel 3600 1450 0    60   Input ~ 0
~PPU_RD
Text GLabel 3600 2850 0    60   Input ~ 0
~PPU_RD
Text GLabel 3600 1350 0    60   Input ~ 0
M2
Text GLabel 3600 2750 0    60   Input ~ 0
M2
$Comp
L GND #PWR06
U 1 1 5BBB5F75
P 3900 1250
F 0 "#PWR06" H 3900 1000 50  0001 C CNN
F 1 "GND" H 3900 1100 50  0000 C CNN
F 2 "" H 3900 1250 50  0000 C CNN
F 3 "" H 3900 1250 50  0000 C CNN
	1    3900 1250
	-1   0    0    1   
$EndComp
$Comp
L 29W040 U10
U 1 1 5BBB804B
P 9850 1900
F 0 "U10" H 10120 2850 50  0000 C CNN
F 1 "29W040" H 10150 650 50  0000 C CNN
F 2 "cart_interface:SOIC-32" H 9850 1900 50  0001 C CNN
F 3 "" H 9850 1900 50  0000 C CNN
	1    9850 1900
	1    0    0    -1  
$EndComp
Text GLabel 10450 1100 2    60   Input ~ 0
CPU_D0
Text GLabel 10450 1200 2    60   Input ~ 0
CPU_D1
Text GLabel 10450 1300 2    60   Input ~ 0
CPU_D2
Text GLabel 10450 1400 2    60   Input ~ 0
CPU_D3
Text GLabel 10450 1500 2    60   Input ~ 0
CPU_D4
Text GLabel 10450 1600 2    60   Input ~ 0
CPU_D5
Text GLabel 10450 1700 2    60   Input ~ 0
CPU_D6
Text GLabel 10450 1800 2    60   Input ~ 0
CPU_D7
$Comp
L GND #PWR07
U 1 1 5BBB8C56
P 9900 3350
F 0 "#PWR07" H 9900 3100 50  0001 C CNN
F 1 "GND" H 9900 3200 50  0000 C CNN
F 2 "" H 9900 3350 50  0000 C CNN
F 3 "" H 9900 3350 50  0000 C CNN
	1    9900 3350
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR08
U 1 1 5BBB8C76
P 9900 700
F 0 "#PWR08" H 9900 550 50  0001 C CNN
F 1 "VCC" H 9900 850 50  0000 C CNN
F 2 "" H 9900 700 50  0000 C CNN
F 3 "" H 9900 700 50  0000 C CNN
	1    9900 700 
	1    0    0    -1  
$EndComp
Text GLabel 9250 1100 0    60   Input ~ 0
CPU_A0
Text GLabel 9250 1200 0    60   Input ~ 0
CPU_A1
Text GLabel 9250 1300 0    60   Input ~ 0
CPU_A2
Text GLabel 9250 1400 0    60   Input ~ 0
CPU_A3
Text GLabel 9250 1500 0    60   Input ~ 0
CPU_A4
Text GLabel 9250 1600 0    60   Input ~ 0
CPU_A5
Text GLabel 9250 1700 0    60   Input ~ 0
CPU_A6
Text GLabel 9250 1800 0    60   Input ~ 0
CPU_A7
Text GLabel 9250 1900 0    60   Input ~ 0
CPU_A8
Text GLabel 9250 2000 0    60   Input ~ 0
CPU_A9
Text GLabel 9250 2100 0    60   Input ~ 0
CPU_A10
Text GLabel 9250 2200 0    60   Input ~ 0
CPU_A11
Text GLabel 9250 2300 0    60   Input ~ 0
CPU_A12
Text GLabel 9250 2400 0    60   Input ~ 0
CPU_A13
Text GLabel 9250 2500 0    60   Input ~ 0
CPU_A14
Text GLabel 9250 2600 0    60   Input ~ 0
PRG_ROM_A15
Text GLabel 9250 2700 0    60   Input ~ 0
PRG_ROM_A16
Text GLabel 9250 2800 0    60   Input ~ 0
PRG_ROM_A17
Text GLabel 9250 2900 0    60   Input ~ 0
PRG_ROM_A18
Text GLabel 10450 2000 2    60   Input ~ 0
/ROMSEL
Text GLabel 10450 2200 2    60   Input ~ 0
CPU_R/W
$Comp
L GND #PWR09
U 1 1 5BBB9E7A
P 11000 2250
F 0 "#PWR09" H 11000 2000 50  0001 C CNN
F 1 "GND" H 11000 2100 50  0000 C CNN
F 2 "" H 11000 2250 50  0000 C CNN
F 3 "" H 11000 2250 50  0000 C CNN
	1    11000 2250
	1    0    0    -1  
$EndComp
Text GLabel 3650 3500 0    60   Input ~ 0
CPU_A0
Text GLabel 3650 3600 0    60   Input ~ 0
CPU_A1
Text GLabel 3650 3700 0    60   Input ~ 0
CPU_A2
Text GLabel 3650 3800 0    60   Input ~ 0
CPU_A3
Text GLabel 3650 3900 0    60   Input ~ 0
CPU_A4
Text GLabel 3650 4000 0    60   Input ~ 0
CPU_A5
Text GLabel 3650 4100 0    60   Input ~ 0
CPU_A6
Text GLabel 3650 4200 0    60   Input ~ 0
CPU_A7
Text GLabel 3650 4400 0    60   Input ~ 0
CTRL_CLK
Text GLabel 5050 3500 2    60   Input ~ 0
PRG_ROM_A15
Text GLabel 5050 3600 2    60   Input ~ 0
PRG_ROM_A16
Text GLabel 5050 3700 2    60   Input ~ 0
PRG_ROM_A17
Text GLabel 5050 3800 2    60   Input ~ 0
PRG_ROM_A18
Text GLabel 5050 3900 2    60   Input ~ 0
PRG_RAM_A13
Text GLabel 5050 4000 2    60   Input ~ 0
PRG_RAM_A14
$Comp
L ATTINY13-P IC2
U 1 1 5BBBD5EF
P 9800 4000
F 0 "IC2" H 9000 4400 50  0000 C CNN
F 1 "ATTINY13-P" H 10450 3600 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 10450 4000 50  0000 C CIN
F 3 "" H 9000 4350 50  0000 C CNN
	1    9800 4000
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR010
U 1 1 5BBBD761
P 10800 3750
F 0 "#PWR010" H 10800 3600 50  0001 C CNN
F 1 "VCC" H 10800 3900 50  0000 C CNN
F 2 "" H 10800 3750 50  0000 C CNN
F 3 "" H 10800 3750 50  0000 C CNN
	1    10800 3750
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR011
U 1 1 5BBBD785
P 10800 4250
F 0 "#PWR011" H 10800 4000 50  0001 C CNN
F 1 "GND" H 10800 4100 50  0000 C CNN
F 2 "" H 10800 4250 50  0000 C CNN
F 3 "" H 10800 4250 50  0000 C CNN
	1    10800 4250
	1    0    0    -1  
$EndComp
$Comp
L 74LS139 U9
U 2 1 5BBC0701
P 9650 5950
F 0 "U9" H 9650 6050 50  0000 C CNN
F 1 "74LS139" H 9650 5850 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 9650 5950 50  0001 C CNN
F 3 "" H 9650 5950 50  0000 C CNN
	2    9650 5950
	1    0    0    1   
$EndComp
$Comp
L 74LS139 U9
U 1 1 5BBC0748
P 7400 5800
F 0 "U9" H 7400 5900 50  0000 C CNN
F 1 "74LS139" H 7400 5700 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 7400 5800 50  0001 C CNN
F 3 "" H 7400 5800 50  0000 C CNN
	1    7400 5800
	1    0    0    1   
$EndComp
$Comp
L 74HC00 U7
U 1 1 5BBC0CDE
P 7150 5050
F 0 "U7" H 7150 5100 50  0000 C CNN
F 1 "74HC00" H 7150 4950 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-14_4.4x5mm_Pitch0.65mm" H 7150 5050 50  0001 C CNN
F 3 "" H 7150 5050 50  0000 C CNN
	1    7150 5050
	-1   0    0    -1  
$EndComp
Text GLabel 8250 5500 2    60   Input ~ 0
/PRG_RAM_CS
NoConn ~ 8250 6100
NoConn ~ 8250 5900
NoConn ~ 10500 6250
Text GLabel 6550 6050 0    60   Input ~ 0
/ROMSEL
Text GLabel 6550 5900 0    60   Input ~ 0
CPU_A13
Text GLabel 7750 4950 2    60   Input ~ 0
CPU_A14
Text GLabel 7750 5150 2    60   Input ~ 0
M2
Text GLabel 8800 6050 0    60   Input ~ 0
CPU_A11
Text GLabel 8800 6200 0    60   Input ~ 0
CPU_A12
Text GLabel 10500 5650 2    60   Input ~ 0
/TSC1_OS
Text GLabel 10500 5850 2    60   Input ~ 0
/TSC0_OS
Text GLabel 10500 6050 2    60   Input ~ 0
CTRL_CLK
NoConn ~ 5300 3050
$Comp
L GND #PWR012
U 1 1 5BBD5E09
P 850 750
F 0 "#PWR012" H 850 500 50  0001 C CNN
F 1 "GND" H 850 600 50  0000 C CNN
F 2 "" H 850 750 50  0000 C CNN
F 3 "" H 850 750 50  0000 C CNN
	1    850  750 
	1    0    0    -1  
$EndComp
Text Notes 8250 7500 0    60   ~ 0
CPU SECTION
Text GLabel 8800 4050 0    60   Input ~ 0
CLK
Text GLabel 2500 4550 2    60   Input ~ 0
CLK
Text GLabel 2500 4450 2    60   Input ~ 0
S2
Text GLabel 8800 3950 0    60   Input ~ 0
S2
Text GLabel 8800 3850 0    60   Input ~ 0
S0
Text GLabel 1400 4450 0    60   Input ~ 0
S0
Text GLabel 1400 4550 0    60   Input ~ 0
S1
Text GLabel 8800 3750 0    60   Input ~ 0
S1
$Comp
L ATTINY13A-P IC3
U 1 1 5BBDD86B
P 9800 4800
F 0 "IC3" H 9000 5200 50  0000 C CNN
F 1 "ATTINY13A-P" H 10450 4400 50  0000 C CNN
F 2 "SMD_Packages:SOIC-8-N" H 10450 4800 50  0000 C CIN
F 3 "" H 9000 5150 50  0000 C CNN
	1    9800 4800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 5BBDD8E8
P 10800 5050
F 0 "#PWR013" H 10800 4800 50  0001 C CNN
F 1 "GND" H 10800 4900 50  0000 C CNN
F 2 "" H 10800 5050 50  0000 C CNN
F 3 "" H 10800 5050 50  0000 C CNN
	1    10800 5050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR014
U 1 1 5BBDD924
P 10800 4550
F 0 "#PWR014" H 10800 4400 50  0001 C CNN
F 1 "VCC" H 10800 4700 50  0000 C CNN
F 2 "" H 10800 4550 50  0000 C CNN
F 3 "" H 10800 4550 50  0000 C CNN
	1    10800 4550
	-1   0    0    1   
$EndComp
Text GLabel 8800 4550 0    60   Input ~ 0
S1
Text GLabel 8800 4650 0    60   Input ~ 0
S0
Text GLabel 8800 4750 0    60   Input ~ 0
S2
Text GLabel 8800 4850 0    60   Input ~ 0
CLK
$Comp
L C C1
U 1 1 5BBCB1F2
P 900 7100
F 0 "C1" H 925 7200 50  0000 L CNN
F 1 "10nF" H 925 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 938 6950 50  0001 C CNN
F 3 "" H 900 7100 50  0000 C CNN
	1    900  7100
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 5BBCB968
P 1150 7100
F 0 "C2" H 1175 7200 50  0000 L CNN
F 1 "10nF" H 1175 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 1188 6950 50  0001 C CNN
F 3 "" H 1150 7100 50  0000 C CNN
	1    1150 7100
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 5BBCB9DB
P 1400 7100
F 0 "C3" H 1425 7200 50  0000 L CNN
F 1 "10nF" H 1425 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 1438 6950 50  0001 C CNN
F 3 "" H 1400 7100 50  0000 C CNN
	1    1400 7100
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 5BBCB9E1
P 1650 7100
F 0 "C4" H 1675 7200 50  0000 L CNN
F 1 "10nF" H 1675 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 1688 6950 50  0001 C CNN
F 3 "" H 1650 7100 50  0000 C CNN
	1    1650 7100
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 5BBCBA63
P 1900 7100
F 0 "C5" H 1925 7200 50  0000 L CNN
F 1 "10nF" H 1925 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 1938 6950 50  0001 C CNN
F 3 "" H 1900 7100 50  0000 C CNN
	1    1900 7100
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5BBCBA69
P 2150 7100
F 0 "C6" H 2175 7200 50  0000 L CNN
F 1 "10nF" H 2175 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2188 6950 50  0001 C CNN
F 3 "" H 2150 7100 50  0000 C CNN
	1    2150 7100
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 5BBCBA6F
P 2400 7100
F 0 "C7" H 2425 7200 50  0000 L CNN
F 1 "10nF" H 2425 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2438 6950 50  0001 C CNN
F 3 "" H 2400 7100 50  0000 C CNN
	1    2400 7100
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 5BBCBA75
P 2650 7100
F 0 "C8" H 2675 7200 50  0000 L CNN
F 1 ".1uF" H 2675 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2688 6950 50  0001 C CNN
F 3 "" H 2650 7100 50  0000 C CNN
	1    2650 7100
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 5BBCBAEC
P 2900 7100
F 0 "C9" H 2925 7200 50  0000 L CNN
F 1 ".1uF" H 2925 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 2938 6950 50  0001 C CNN
F 3 "" H 2900 7100 50  0000 C CNN
	1    2900 7100
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 5BBCBAF2
P 3150 7100
F 0 "C10" H 3175 7200 50  0000 L CNN
F 1 ".1uF" H 3175 7000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3188 6950 50  0001 C CNN
F 3 "" H 3150 7100 50  0000 C CNN
	1    3150 7100
	1    0    0    -1  
$EndComp
$Comp
L CP1 C11
U 1 1 5BBCE00E
P 3400 7100
F 0 "C11" H 3425 7200 50  0000 L CNN
F 1 "47uF" H 3425 7000 50  0000 L CNN
F 2 "Capacitors_SMD:CP_Elec_4x5.3" H 3400 7100 50  0001 C CNN
F 3 "" H 3400 7100 50  0000 C CNN
	1    3400 7100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 5BBD24E3
P 900 7250
F 0 "#PWR015" H 900 7000 50  0001 C CNN
F 1 "GND" H 900 7100 50  0000 C CNN
F 2 "" H 900 7250 50  0000 C CNN
F 3 "" H 900 7250 50  0000 C CNN
	1    900  7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 5BBD2537
P 1150 7250
F 0 "#PWR016" H 1150 7000 50  0001 C CNN
F 1 "GND" H 1150 7100 50  0000 C CNN
F 2 "" H 1150 7250 50  0000 C CNN
F 3 "" H 1150 7250 50  0000 C CNN
	1    1150 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 5BBD258B
P 1400 7250
F 0 "#PWR017" H 1400 7000 50  0001 C CNN
F 1 "GND" H 1400 7100 50  0000 C CNN
F 2 "" H 1400 7250 50  0000 C CNN
F 3 "" H 1400 7250 50  0000 C CNN
	1    1400 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 5BBD25DF
P 1650 7250
F 0 "#PWR018" H 1650 7000 50  0001 C CNN
F 1 "GND" H 1650 7100 50  0000 C CNN
F 2 "" H 1650 7250 50  0000 C CNN
F 3 "" H 1650 7250 50  0000 C CNN
	1    1650 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 5BBD2633
P 1900 7250
F 0 "#PWR019" H 1900 7000 50  0001 C CNN
F 1 "GND" H 1900 7100 50  0000 C CNN
F 2 "" H 1900 7250 50  0000 C CNN
F 3 "" H 1900 7250 50  0000 C CNN
	1    1900 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR020
U 1 1 5BBD2687
P 2150 7250
F 0 "#PWR020" H 2150 7000 50  0001 C CNN
F 1 "GND" H 2150 7100 50  0000 C CNN
F 2 "" H 2150 7250 50  0000 C CNN
F 3 "" H 2150 7250 50  0000 C CNN
	1    2150 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 5BBD26DB
P 2400 7250
F 0 "#PWR021" H 2400 7000 50  0001 C CNN
F 1 "GND" H 2400 7100 50  0000 C CNN
F 2 "" H 2400 7250 50  0000 C CNN
F 3 "" H 2400 7250 50  0000 C CNN
	1    2400 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 5BBD272F
P 2650 7250
F 0 "#PWR022" H 2650 7000 50  0001 C CNN
F 1 "GND" H 2650 7100 50  0000 C CNN
F 2 "" H 2650 7250 50  0000 C CNN
F 3 "" H 2650 7250 50  0000 C CNN
	1    2650 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 5BBD2783
P 2900 7250
F 0 "#PWR023" H 2900 7000 50  0001 C CNN
F 1 "GND" H 2900 7100 50  0000 C CNN
F 2 "" H 2900 7250 50  0000 C CNN
F 3 "" H 2900 7250 50  0000 C CNN
	1    2900 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 5BBD27D7
P 3150 7250
F 0 "#PWR024" H 3150 7000 50  0001 C CNN
F 1 "GND" H 3150 7100 50  0000 C CNN
F 2 "" H 3150 7250 50  0000 C CNN
F 3 "" H 3150 7250 50  0000 C CNN
	1    3150 7250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 5BBD287F
P 3400 7250
F 0 "#PWR025" H 3400 7000 50  0001 C CNN
F 1 "GND" H 3400 7100 50  0000 C CNN
F 2 "" H 3400 7250 50  0000 C CNN
F 3 "" H 3400 7250 50  0000 C CNN
	1    3400 7250
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR026
U 1 1 5BBD28D3
P 900 6950
F 0 "#PWR026" H 900 6800 50  0001 C CNN
F 1 "VCC" H 900 7100 50  0000 C CNN
F 2 "" H 900 6950 50  0000 C CNN
F 3 "" H 900 6950 50  0000 C CNN
	1    900  6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR027
U 1 1 5BBD2927
P 1150 6950
F 0 "#PWR027" H 1150 6800 50  0001 C CNN
F 1 "VCC" H 1150 7100 50  0000 C CNN
F 2 "" H 1150 6950 50  0000 C CNN
F 3 "" H 1150 6950 50  0000 C CNN
	1    1150 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR028
U 1 1 5BBD297B
P 1400 6950
F 0 "#PWR028" H 1400 6800 50  0001 C CNN
F 1 "VCC" H 1400 7100 50  0000 C CNN
F 2 "" H 1400 6950 50  0000 C CNN
F 3 "" H 1400 6950 50  0000 C CNN
	1    1400 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR029
U 1 1 5BBD29CF
P 1650 6950
F 0 "#PWR029" H 1650 6800 50  0001 C CNN
F 1 "VCC" H 1650 7100 50  0000 C CNN
F 2 "" H 1650 6950 50  0000 C CNN
F 3 "" H 1650 6950 50  0000 C CNN
	1    1650 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR030
U 1 1 5BBD2A23
P 1900 6950
F 0 "#PWR030" H 1900 6800 50  0001 C CNN
F 1 "VCC" H 1900 7100 50  0000 C CNN
F 2 "" H 1900 6950 50  0000 C CNN
F 3 "" H 1900 6950 50  0000 C CNN
	1    1900 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR031
U 1 1 5BBD2A77
P 2150 6950
F 0 "#PWR031" H 2150 6800 50  0001 C CNN
F 1 "VCC" H 2150 7100 50  0000 C CNN
F 2 "" H 2150 6950 50  0000 C CNN
F 3 "" H 2150 6950 50  0000 C CNN
	1    2150 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR032
U 1 1 5BBD2ACB
P 2400 6950
F 0 "#PWR032" H 2400 6800 50  0001 C CNN
F 1 "VCC" H 2400 7100 50  0000 C CNN
F 2 "" H 2400 6950 50  0000 C CNN
F 3 "" H 2400 6950 50  0000 C CNN
	1    2400 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR033
U 1 1 5BBD2B1F
P 2650 6950
F 0 "#PWR033" H 2650 6800 50  0001 C CNN
F 1 "VCC" H 2650 7100 50  0000 C CNN
F 2 "" H 2650 6950 50  0000 C CNN
F 3 "" H 2650 6950 50  0000 C CNN
	1    2650 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR034
U 1 1 5BBD2B73
P 2900 6950
F 0 "#PWR034" H 2900 6800 50  0001 C CNN
F 1 "VCC" H 2900 7100 50  0000 C CNN
F 2 "" H 2900 6950 50  0000 C CNN
F 3 "" H 2900 6950 50  0000 C CNN
	1    2900 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR035
U 1 1 5BBD2C07
P 3150 6950
F 0 "#PWR035" H 3150 6800 50  0001 C CNN
F 1 "VCC" H 3150 7100 50  0000 C CNN
F 2 "" H 3150 6950 50  0000 C CNN
F 3 "" H 3150 6950 50  0000 C CNN
	1    3150 6950
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR036
U 1 1 5BBD2CAF
P 3400 6950
F 0 "#PWR036" H 3400 6800 50  0001 C CNN
F 1 "VCC" H 3400 7100 50  0000 C CNN
F 2 "" H 3400 6950 50  0000 C CNN
F 3 "" H 3400 6950 50  0000 C CNN
	1    3400 6950
	1    0    0    -1  
$EndComp
$Comp
L 74LS574 U2
U 1 1 5BBCF3F2
P 4350 4000
F 0 "U2" H 4350 4000 50  0000 C CNN
F 1 "74LS574" H 4400 3650 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 4350 4000 50  0001 C CNN
F 3 "" H 4350 4000 50  0000 C CNN
	1    4350 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR037
U 1 1 5BBCF509
P 3650 4500
F 0 "#PWR037" H 3650 4250 50  0001 C CNN
F 1 "GND" H 3650 4350 50  0000 C CNN
F 2 "" H 3650 4500 50  0000 C CNN
F 3 "" H 3650 4500 50  0000 C CNN
	1    3650 4500
	1    0    0    -1  
$EndComp
$Comp
L 32KB_SRAM IC1
U 1 1 5BBDC121
P 7350 1900
F 0 "IC1" H 6900 3000 50  0000 L CNN
F 1 "32KB_SRAM" H 7500 3000 50  0000 L CNN
F 2 "cart_interface:SOIC-28" H 7350 1900 50  0001 C CNN
F 3 "" H 7350 1900 50  0000 C CNN
	1    7350 1900
	1    0    0    -1  
$EndComp
Text GLabel 2550 3100 2    60   Input ~ 0
~PPU_A13
Text GLabel 5050 4100 2    60   Input ~ 0
MIR_MODE
Text GLabel 5050 4200 2    60   Input ~ 0
MIR_TYPE
$Sheet
S 7200 6700 3800 250 
U 5BC9BF61
F0 "PPU_SECTION" 60
F1 "PPU_SECTION.sch" 60
$EndSheet
$Comp
L 74LS574 U8
U 1 1 5BCA0C41
P 7300 4000
F 0 "U8" H 7300 4000 50  0000 C CNN
F 1 "74LS574" H 7350 3650 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 7300 4000 50  0001 C CNN
F 3 "" H 7300 4000 50  0000 C CNN
	1    7300 4000
	1    0    0    -1  
$EndComp
Text GLabel 6600 4400 0    60   Input ~ 0
/TSC0_OS
Text GLabel 6600 3500 0    60   Input ~ 0
CPU_A0
Text GLabel 6600 3600 0    60   Input ~ 0
CPU_A1
Text GLabel 6600 3700 0    60   Input ~ 0
CPU_A2
$Comp
L GND #PWR038
U 1 1 5BCA162D
P 6600 4500
F 0 "#PWR038" H 6600 4250 50  0001 C CNN
F 1 "GND" H 6600 4350 50  0000 C CNN
F 2 "" H 6600 4500 50  0000 C CNN
F 3 "" H 6600 4500 50  0000 C CNN
	1    6600 4500
	1    0    0    -1  
$EndComp
Text GLabel 8000 3500 2    60   Input ~ 0
OBJ_A12
Text GLabel 8000 3600 2    60   Input ~ 0
OBJ_A13
Text GLabel 8000 3700 2    60   Input ~ 0
OBJ_A14
NoConn ~ 8000 3900
NoConn ~ 8000 4000
NoConn ~ 8000 4100
NoConn ~ 8000 4200
$Comp
L 74LS574 U3
U 1 1 5BCA511C
P 4350 5400
F 0 "U3" H 4350 5400 50  0000 C CNN
F 1 "74LS574" H 4400 5050 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 4350 5400 50  0001 C CNN
F 3 "" H 4350 5400 50  0000 C CNN
	1    4350 5400
	1    0    0    -1  
$EndComp
Text GLabel 3650 5600 0    60   Input ~ 0
CPU_A0
Text GLabel 3650 5500 0    60   Input ~ 0
CPU_A1
Text GLabel 3650 5400 0    60   Input ~ 0
CPU_A2
Text GLabel 3650 5300 0    60   Input ~ 0
CPU_A3
Text GLabel 3650 5200 0    60   Input ~ 0
CPU_A4
Text GLabel 3650 5100 0    60   Input ~ 0
CPU_A5
Text GLabel 3650 5000 0    60   Input ~ 0
CPU_A6
Text GLabel 3650 4900 0    60   Input ~ 0
CPU_A7
Text GLabel 3650 5800 0    60   Input ~ 0
/TSC1_OS
Text GLabel 5050 5600 2    60   Input ~ 0
CHR_D0
Text GLabel 5050 5500 2    60   Input ~ 0
CHR_D1
Text GLabel 5050 5400 2    60   Input ~ 0
CHR_D2
Text GLabel 5050 5300 2    60   Input ~ 0
CHR_D3
Text GLabel 5050 5200 2    60   Input ~ 0
CHR_D4
Text GLabel 5050 5100 2    60   Input ~ 0
CHR_D5
Text GLabel 5050 5000 2    60   Input ~ 0
CHR_D6
Text GLabel 5050 4900 2    60   Input ~ 0
CHR_D7
Text GLabel 2500 3850 2    60   Input ~ 0
PPU_A13
$Comp
L 4053 U6
U 1 1 5BCA6C38
P 5150 7000
F 0 "U6" H 5250 7000 50  0000 C CNN
F 1 "4053" H 5250 6800 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 5150 7000 60  0001 C CNN
F 3 "" H 5150 7000 60  0001 C CNN
	1    5150 7000
	1    0    0    -1  
$EndComp
Text GLabel 6000 7000 2    60   Input ~ 0
CHR_RAM_A10
Text GLabel 4450 7600 0    60   Input ~ 0
PPU_A13
Text GLabel 4450 7500 0    60   Input ~ 0
MIR_MODE
Text GLabel 4450 7400 0    60   Input ~ 0
MIR_TYPE
Text GLabel 4450 6700 0    60   Input ~ 0
MIR_TYPE
Text GLabel 4450 6400 0    60   Input ~ 0
PPU_A10
Text GLabel 4450 6500 0    60   Input ~ 0
PPU_A11
Text GLabel 6000 6700 2    60   Input ~ 0
NTRAM_A10
Text GLabel 4450 7000 0    60   Input ~ 0
PPU_A10
$Comp
L GND #PWR039
U 1 1 5BCA8D98
P 5850 7500
F 0 "#PWR039" H 5850 7250 50  0001 C CNN
F 1 "GND" H 5850 7350 50  0000 C CNN
F 2 "" H 5850 7500 50  0000 C CNN
F 3 "" H 5850 7500 50  0000 C CNN
	1    5850 7500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR040
U 1 1 5BCAA04E
P 3900 7550
F 0 "#PWR040" H 3900 7300 50  0001 C CNN
F 1 "GND" H 3900 7400 50  0000 C CNN
F 2 "" H 3900 7550 50  0000 C CNN
F 3 "" H 3900 7550 50  0000 C CNN
	1    3900 7550
	1    0    0    -1  
$EndComp
$Comp
L 74AHCT1G32 U1
U 1 1 5BCAE044
P 2600 5900
F 0 "U1" H 2600 5950 50  0000 C CNN
F 1 "74AHCT1G32" H 2600 5850 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-353_SC-70-5" H 2600 5900 50  0001 C CNN
F 3 "" H 2600 5900 50  0000 C CNN
	1    2600 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 5700 8800 5700
Wire Wire Line
	2500 3050 2500 3150
Wire Wire Line
	11000 2100 11000 2250
Wire Wire Line
	850  750  1400 750 
Wire Wire Line
	10450 2100 11000 2100
Connection ~ 3800 2150
Wire Wire Line
	3600 2850 3900 2850
Wire Wire Line
	3600 2750 3900 2750
Wire Wire Line
	3600 2150 3900 2150
Wire Wire Line
	3600 1450 3900 1450
Wire Wire Line
	3600 1350 3900 1350
Wire Wire Line
	3600 750  3900 750 
Wire Wire Line
	5300 1900 5300 1650
Wire Wire Line
	3700 1900 5300 1900
Wire Wire Line
	3700 2650 3700 1900
Wire Wire Line
	3900 2650 3700 2650
Wire Wire Line
	3800 2400 3900 2400
Wire Wire Line
	3800 1000 3800 2400
Wire Wire Line
	3900 1000 3800 1000
Wire Wire Line
	6200 2700 6200 2850
Wire Wire Line
	6750 2700 6200 2700
Connection ~ 2500 3100
Wire Wire Line
	2550 3100 2500 3100
Wire Wire Line
	6550 5050 6550 5550
Wire Wire Line
	5850 6400 5850 6150
Wire Wire Line
	5850 6150 3800 6150
Wire Wire Line
	3800 6150 3800 6800
Wire Wire Line
	3800 6800 4450 6800
Wire Wire Line
	5850 6700 6000 6700
Wire Wire Line
	5950 6700 5950 6250
Wire Wire Line
	5950 6250 3900 6250
Wire Wire Line
	3900 6250 3900 7100
Wire Wire Line
	3900 7100 4450 7100
Connection ~ 5950 6700
Wire Wire Line
	6000 7000 5850 7000
Wire Wire Line
	4450 7300 3900 7300
Wire Wire Line
	3900 7300 3900 7550
Wire Wire Line
	3650 5900 3200 5900
Text GLabel 2000 5800 0    60   Input ~ 0
~PPU_WR
Text GLabel 2000 6000 0    60   Input ~ 0
~PPU_A13
$Comp
L VCC #PWR041
U 1 1 5BCC2B52
P 5150 6350
F 0 "#PWR041" H 5150 6200 50  0001 C CNN
F 1 "VCC" H 5150 6500 50  0000 C CNN
F 2 "" H 5150 6350 50  0000 C CNN
F 3 "" H 5150 6350 50  0000 C CNN
	1    5150 6350
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR042
U 1 1 5BCC2B9A
P 5150 7650
F 0 "#PWR042" H 5150 7400 50  0001 C CNN
F 1 "GND" H 5150 7500 50  0000 C CNN
F 2 "" H 5150 7650 50  0000 C CNN
F 3 "" H 5150 7650 50  0000 C CNN
	1    5150 7650
	-1   0    0    1   
$EndComp
Text GLabel 8000 3800 2    60   Input ~ 0
EN
Text GLabel 6600 3800 0    60   Input ~ 0
CPU_A3
Text GLabel 6600 3900 0    60   Input ~ 0
CPU_A4
Text GLabel 6600 4000 0    60   Input ~ 0
CPU_A5
Text GLabel 6600 4100 0    60   Input ~ 0
CPU_A6
Text GLabel 6600 4200 0    60   Input ~ 0
CPU_A7
Text GLabel 2500 2550 2    60   Input ~ 0
EXP_7
Text GLabel 2500 2450 2    60   Input ~ 0
EXP_8
Text GLabel 8800 4150 0    60   Input ~ 0
EXP_7
Text GLabel 8800 4250 0    60   Input ~ 0
EXP_8
Text GLabel 8800 4950 0    60   Input ~ 0
EXP_7
Text GLabel 8800 5050 0    60   Input ~ 0
EXP_8
$EndSCHEMATC
