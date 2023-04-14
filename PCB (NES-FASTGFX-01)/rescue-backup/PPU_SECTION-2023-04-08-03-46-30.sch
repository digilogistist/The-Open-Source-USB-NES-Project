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
Sheet 2 2
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
L 32KB_SRAM IC4
U 1 1 5BC8E470
P 5450 5100
F 0 "IC4" H 5000 6200 50  0000 L CNN
F 1 "32KB_SRAM" H 5600 6200 50  0000 L CNN
F 2 "cart_interface:SOIC-28" H 5450 5100 50  0001 C CNN
F 3 "" H 5450 5100 50  0000 C CNN
	1    5450 5100
	-1   0    0    -1  
$EndComp
$Comp
L 74LS377 U15
U 1 1 5BC8E71B
P 9500 3350
F 0 "U15" H 9500 3350 50  0000 C CNN
F 1 "74LS377" H 9600 2950 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 9500 3350 50  0001 C CNN
F 3 "" H 9500 3350 50  0000 C CNN
	1    9500 3350
	-1   0    0    -1  
$EndComp
$Comp
L VCC #PWR043
U 1 1 5BC62042
P 5450 3900
F 0 "#PWR043" H 5450 3750 50  0001 C CNN
F 1 "VCC" H 5450 4050 50  0000 C CNN
F 2 "" H 5450 3900 50  0000 C CNN
F 3 "" H 5450 3900 50  0000 C CNN
	1    5450 3900
	1    0    0    -1  
$EndComp
$Comp
L 74LS157 U13
U 1 1 5BC69F38
P 7750 5350
F 0 "U13" H 7800 5500 50  0000 C CNN
F 1 "74LS157" H 7800 5200 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-16_4.4x5mm_Pitch0.65mm" H 7750 5350 50  0001 C CNN
F 3 "" H 7750 5350 50  0000 C CNN
	1    7750 5350
	-1   0    0    -1  
$EndComp
Text GLabel 3250 1750 0    60   Input ~ 0
~PPU_RD
Text GLabel 1850 2050 0    60   Input ~ 0
PPU_A3
Text GLabel 1850 1850 0    60   Input ~ 0
~PPU_A13
Text GLabel 4450 1950 2    60   Input ~ 0
~NT_FETCH
$Comp
L 74HC00 U7
U 2 1 5BC7B24D
P 2450 1950
F 0 "U7" H 2450 2000 50  0000 C CNN
F 1 "74HC00" H 2450 1850 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-14_4.4x5mm_Pitch0.65mm" H 2450 1950 50  0001 C CNN
F 3 "" H 2450 1950 50  0000 C CNN
	2    2450 1950
	1    0    0    1   
$EndComp
$Comp
L 74HC00 U7
U 4 1 5BC7B2DF
P 7800 4350
F 0 "U7" H 7800 4400 50  0000 C CNN
F 1 "74HC00" H 7800 4250 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-14_4.4x5mm_Pitch0.65mm" H 7800 4350 50  0001 C CNN
F 3 "" H 7800 4350 50  0000 C CNN
	4    7800 4350
	-1   0    0    -1  
$EndComp
$Comp
L 74HC00 U7
U 3 1 5BC7B4DF
P 5450 3250
F 0 "U7" H 5450 3300 50  0000 C CNN
F 1 "74HC00" H 5450 3150 50  0000 C CNN
F 2 "Housings_SSOP:TSSOP-14_4.4x5mm_Pitch0.65mm" H 5450 3250 50  0001 C CNN
F 3 "" H 5450 3250 50  0000 C CNN
	3    5450 3250
	-1   0    0    -1  
$EndComp
$Comp
L C C17
U 1 1 5BC7D5F1
P 2900 6800
F 0 "C17" H 2925 6900 50  0000 L CNN
F 1 "10nF" H 2925 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2938 6650 50  0001 C CNN
F 3 "" H 2900 6800 50  0000 C CNN
	1    2900 6800
	1    0    0    -1  
$EndComp
$Comp
L C C16
U 1 1 5BC7D69A
P 2650 6800
F 0 "C16" H 2675 6900 50  0000 L CNN
F 1 "10nF" H 2675 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2688 6650 50  0001 C CNN
F 3 "" H 2650 6800 50  0000 C CNN
	1    2650 6800
	1    0    0    -1  
$EndComp
$Comp
L C C15
U 1 1 5BC7D6EE
P 2400 6800
F 0 "C15" H 2425 6900 50  0000 L CNN
F 1 "10nF" H 2425 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2438 6650 50  0001 C CNN
F 3 "" H 2400 6800 50  0000 C CNN
	1    2400 6800
	1    0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 5BC7D6F4
P 2150 6800
F 0 "C14" H 2175 6900 50  0000 L CNN
F 1 "10nF" H 2175 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 2188 6650 50  0001 C CNN
F 3 "" H 2150 6800 50  0000 C CNN
	1    2150 6800
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 5BC7D7D2
P 1900 6800
F 0 "C13" H 1925 6900 50  0000 L CNN
F 1 ".1uF" H 1925 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1938 6650 50  0001 C CNN
F 3 "" H 1900 6800 50  0000 C CNN
	1    1900 6800
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 5BC7D7D8
P 1650 6800
F 0 "C12" H 1675 6900 50  0000 L CNN
F 1 ".1uF" H 1675 6700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 1688 6650 50  0001 C CNN
F 3 "" H 1650 6800 50  0000 C CNN
	1    1650 6800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR044
U 1 1 5BC7DA50
P 2900 7050
F 0 "#PWR044" H 2900 6800 50  0001 C CNN
F 1 "GND" H 2900 6900 50  0000 C CNN
F 2 "" H 2900 7050 50  0000 C CNN
F 3 "" H 2900 7050 50  0000 C CNN
	1    2900 7050
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR045
U 1 1 5BC7E431
P 2900 6550
F 0 "#PWR045" H 2900 6400 50  0001 C CNN
F 1 "VCC" H 2900 6700 50  0000 C CNN
F 2 "" H 2900 6550 50  0000 C CNN
F 3 "" H 2900 6550 50  0000 C CNN
	1    2900 6550
	1    0    0    -1  
$EndComp
Text Notes 8250 7650 0    60   ~ 0
PPU SECTION
$Comp
L GND #PWR046
U 1 1 5BC6201C
P 5450 6300
F 0 "#PWR046" H 5450 6050 50  0001 C CNN
F 1 "GND" H 5450 6150 50  0000 C CNN
F 2 "" H 5450 6300 50  0000 C CNN
F 3 "" H 5450 6300 50  0000 C CNN
	1    5450 6300
	1    0    0    -1  
$EndComp
$Comp
L 29W040 U14
U 1 1 5BCAF640
P 7850 2400
F 0 "U14" H 8120 3350 50  0000 C CNN
F 1 "29W040" H 8150 1150 50  0000 C CNN
F 2 "cart_interface:SOIC-32" H 7850 2400 50  0001 C CNN
F 3 "" H 7850 2400 50  0000 C CNN
	1    7850 2400
	-1   0    0    -1  
$EndComp
$Comp
L 74LS245 U11
U 1 1 5BCAF6A1
P 2550 4700
F 0 "U11" H 2650 5275 50  0000 L BNN
F 1 "74LS245" H 2600 4125 50  0000 L TNN
F 2 "Housings_SSOP:TSSOP-20_4.4x6.5mm_Pitch0.65mm" H 2550 4700 50  0001 C CNN
F 3 "" H 2550 4700 50  0000 C CNN
	1    2550 4700
	1    0    0    -1  
$EndComp
Text GLabel 3250 4900 2    60   Input ~ 0
CHR_D0
Text GLabel 3250 4800 2    60   Input ~ 0
CHR_D1
Text GLabel 3250 4700 2    60   Input ~ 0
CHR_D2
Text GLabel 3250 4600 2    60   Input ~ 0
CHR_D3
Text GLabel 3250 4500 2    60   Input ~ 0
CHR_D4
Text GLabel 3250 4400 2    60   Input ~ 0
CHR_D5
Text GLabel 3250 4300 2    60   Input ~ 0
CHR_D6
Text GLabel 3250 4200 2    60   Input ~ 0
CHR_D7
Text GLabel 1850 4900 0    60   Input ~ 0
PPU_D0
Text GLabel 1850 4800 0    60   Input ~ 0
PPU_D1
Text GLabel 1850 4700 0    60   Input ~ 0
PPU_D2
Text GLabel 1850 4600 0    60   Input ~ 0
PPU_D3
Text GLabel 1850 4500 0    60   Input ~ 0
PPU_D4
Text GLabel 1850 4400 0    60   Input ~ 0
PPU_D5
Text GLabel 1850 4300 0    60   Input ~ 0
PPU_D6
Text GLabel 1850 4200 0    60   Input ~ 0
PPU_D7
Text GLabel 1850 5100 0    60   Input ~ 0
~PPU_RD
Text GLabel 1850 5200 0    60   Input ~ 0
PPU_A13
Text GLabel 7250 1600 0    60   Input ~ 0
CHR_D0
Text GLabel 7250 1700 0    60   Input ~ 0
CHR_D1
Text GLabel 7250 1800 0    60   Input ~ 0
CHR_D2
Text GLabel 7250 1900 0    60   Input ~ 0
CHR_D3
Text GLabel 7250 2000 0    60   Input ~ 0
CHR_D4
Text GLabel 7250 2100 0    60   Input ~ 0
CHR_D5
Text GLabel 7250 2200 0    60   Input ~ 0
CHR_D6
Text GLabel 7250 2300 0    60   Input ~ 0
CHR_D7
Text GLabel 4850 4200 0    60   Input ~ 0
CHR_D0
Text GLabel 4850 4300 0    60   Input ~ 0
CHR_D1
Text GLabel 4850 4400 0    60   Input ~ 0
CHR_D2
Text GLabel 4850 4500 0    60   Input ~ 0
CHR_D3
Text GLabel 4850 4600 0    60   Input ~ 0
CHR_D4
Text GLabel 4850 4700 0    60   Input ~ 0
CHR_D5
Text GLabel 4850 4800 0    60   Input ~ 0
CHR_D6
Text GLabel 4850 4900 0    60   Input ~ 0
CHR_D7
Text GLabel 10200 2850 2    60   Input ~ 0
CHR_D0
Text GLabel 10200 2950 2    60   Input ~ 0
CHR_D1
Text GLabel 10200 3050 2    60   Input ~ 0
CHR_D2
Text GLabel 10200 3150 2    60   Input ~ 0
CHR_D3
Text GLabel 10200 3250 2    60   Input ~ 0
CHR_D4
Text GLabel 10200 3350 2    60   Input ~ 0
CHR_D5
Text GLabel 10200 3450 2    60   Input ~ 0
CHR_D6
Text GLabel 10200 3550 2    60   Input ~ 0
CHR_D7
Text GLabel 8450 1600 2    60   Input ~ 0
PPU_A0
Text GLabel 8450 1700 2    60   Input ~ 0
PPU_A1
Text GLabel 8450 1800 2    60   Input ~ 0
PPU_A2
Text GLabel 8450 1900 2    60   Input ~ 0
PPU_A3
Text GLabel 8450 2000 2    60   Input ~ 0
PPU_A4
Text GLabel 8450 2100 2    60   Input ~ 0
PPU_A5
Text GLabel 8450 2200 2    60   Input ~ 0
PPU_A6
Text GLabel 8450 2300 2    60   Input ~ 0
PPU_A7
Text GLabel 8450 2400 2    60   Input ~ 0
PPU_A8
Text GLabel 8450 2500 2    60   Input ~ 0
PPU_A9
Text GLabel 8450 2600 2    60   Input ~ 0
PPU_A10
Text GLabel 8450 2700 2    60   Input ~ 0
PPU_A11
Text GLabel 7250 2600 0    60   Input ~ 0
~PPU_RD
Text GLabel 7250 2700 0    60   Input ~ 0
~PPU_WR
Text GLabel 6050 5900 2    60   Input ~ 0
~PPU_RD
Text GLabel 6050 6000 2    60   Input ~ 0
~PPU_WR
Text GLabel 6050 4200 2    60   Input ~ 0
PPU_A0
Text GLabel 6050 4300 2    60   Input ~ 0
PPU_A1
Text GLabel 6050 4400 2    60   Input ~ 0
PPU_A2
Text GLabel 6050 4500 2    60   Input ~ 0
PPU_A3
Text GLabel 6050 4600 2    60   Input ~ 0
PPU_A4
Text GLabel 6050 4700 2    60   Input ~ 0
PPU_A5
Text GLabel 6050 4800 2    60   Input ~ 0
PPU_A6
Text GLabel 6050 4900 2    60   Input ~ 0
PPU_A7
Text GLabel 6050 5000 2    60   Input ~ 0
PPU_A8
Text GLabel 6050 5100 2    60   Input ~ 0
PPU_A9
Text GLabel 6050 5200 2    60   Input ~ 0
CHR_RAM_A10
Text GLabel 10200 3750 2    60   Input ~ 0
~PPU_RD
Text GLabel 10200 3850 2    60   Input ~ 0
~NT_FETCH
$Comp
L GND #PWR047
U 1 1 5BCB45DA
P 7800 3850
F 0 "#PWR047" H 7800 3600 50  0001 C CNN
F 1 "GND" H 7800 3700 50  0000 C CNN
F 2 "" H 7800 3850 50  0000 C CNN
F 3 "" H 7800 3850 50  0000 C CNN
	1    7800 3850
	-1   0    0    -1  
$EndComp
$Comp
L VCC #PWR048
U 1 1 5BCB4882
P 7800 1200
F 0 "#PWR048" H 7800 1050 50  0001 C CNN
F 1 "VCC" H 7800 1350 50  0000 C CNN
F 2 "" H 7800 1200 50  0000 C CNN
F 3 "" H 7800 1200 50  0000 C CNN
	1    7800 1200
	-1   0    0    -1  
$EndComp
Text GLabel 8800 4800 2    60   Input ~ 0
PPU_A11
Text GLabel 8800 4900 2    60   Input ~ 0
PPU_A11
Text GLabel 8800 5050 2    60   Input ~ 0
OBJ_A12
Text GLabel 8800 5300 2    60   Input ~ 0
OBJ_A13
Text GLabel 8800 5550 2    60   Input ~ 0
OBJ_A14
Text GLabel 8800 5800 2    60   Input ~ 0
PPU_A12
Text GLabel 8800 5900 2    60   Input ~ 0
PPU_A13
Wire Wire Line
	3250 1950 3050 1950
Wire Wire Line
	2900 6950 2900 7050
Wire Wire Line
	1650 6950 1650 7000
Wire Wire Line
	1650 7000 2900 7000
Connection ~ 2900 7000
Wire Wire Line
	1900 6950 1900 7000
Connection ~ 1900 7000
Wire Wire Line
	2150 6950 2150 7000
Connection ~ 2150 7000
Wire Wire Line
	2400 6950 2400 7000
Connection ~ 2400 7000
Wire Wire Line
	2650 6950 2650 7000
Connection ~ 2650 7000
Wire Wire Line
	1650 6650 1650 6600
Wire Wire Line
	1650 6600 2900 6600
Wire Wire Line
	2900 6550 2900 6650
Wire Wire Line
	2650 6650 2650 6600
Connection ~ 2650 6600
Wire Wire Line
	2400 6650 2400 6600
Connection ~ 2400 6600
Wire Wire Line
	2150 6650 2150 6600
Connection ~ 2150 6600
Wire Wire Line
	1900 6650 1900 6600
Connection ~ 1900 6600
Connection ~ 2900 6600
Wire Wire Line
	7000 5600 6050 5600
Wire Wire Line
	6050 5500 7000 5500
Wire Wire Line
	7000 5500 7000 5350
Wire Wire Line
	6050 5400 6900 5400
Wire Wire Line
	6900 5400 6900 5100
Wire Wire Line
	6900 5100 7000 5100
Wire Wire Line
	7000 4850 6800 4850
Wire Wire Line
	6800 4850 6800 5300
Wire Wire Line
	6800 5300 6050 5300
Wire Wire Line
	8450 2800 8800 2800
Wire Wire Line
	8450 2900 8800 2900
Wire Wire Line
	8450 3000 8800 3000
Wire Wire Line
	8450 3100 8800 3100
Wire Wire Line
	8450 3200 8800 3200
Wire Wire Line
	8450 3300 8800 3300
Wire Wire Line
	8450 3400 8800 3400
Wire Wire Line
	7250 2500 6750 2500
Wire Wire Line
	6750 2500 6750 4350
Wire Wire Line
	6750 4350 7200 4350
Wire Wire Line
	6050 3350 6750 3350
Connection ~ 6750 3350
Wire Wire Line
	6050 5800 6550 5800
Wire Wire Line
	6550 5800 6550 6550
Wire Wire Line
	6550 6550 4300 6550
Wire Wire Line
	4300 6550 4300 3250
Wire Wire Line
	4300 3250 4850 3250
Wire Wire Line
	8500 5400 8600 5400
Wire Wire Line
	8600 5400 8600 2900
Connection ~ 8600 2900
Wire Wire Line
	8500 5650 8700 5650
Wire Wire Line
	8700 5650 8700 3000
Connection ~ 8700 3000
Wire Wire Line
	8500 4800 8800 4800
Wire Wire Line
	8500 4900 8800 4900
Wire Wire Line
	8500 5050 8800 5050
Wire Wire Line
	8500 5300 8800 5300
Wire Wire Line
	8500 5550 8800 5550
Wire Wire Line
	8500 5800 8800 5800
Wire Wire Line
	8500 5900 8800 5900
Wire Wire Line
	8500 5150 8500 2800
Connection ~ 8500 2800
Text GLabel 8800 4450 2    60   Input ~ 0
PPU_A12
Wire Wire Line
	8400 4450 8800 4450
$Comp
L 74AHCT1G79 U12
U 1 1 5BCC0115
P 3850 1750
F 0 "U12" H 4000 2050 50  0000 C CNN
F 1 "74AHCT1G79" H 4150 1455 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-353_SC-70-5" H 3850 1750 50  0001 C CNN
F 3 "" H 3850 1750 50  0000 C CNN
	1    3850 1750
	1    0    0    1   
$EndComp
Wire Wire Line
	8800 3500 8800 4250
Wire Wire Line
	8800 4250 8400 4250
Text GLabel 6050 3150 2    60   Input ~ 0
EN
$EndSCHEMATC
