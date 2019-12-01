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
LIBS:SmartHouseComponents
LIBS:SmartHouseRelayGateController-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L STM32DISCOVERY U1
U 1 1 5B0C4D35
P 2850 3950
F 0 "U1" H 3300 3200 60  0000 C CNN
F 1 "STM32DISCOVERY" H 2900 4850 60  0000 C CNN
F 2 "SmartHouseLib:STM32_DISCOVERY" H 3300 3200 60  0001 C CNN
F 3 "" H 3300 3200 60  0001 C CNN
	1    2850 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 4250 5750 4250
$Comp
L CONN_01X04 P3
U 1 1 5DE0F684
P 5500 3400
F 0 "P3" H 5500 3650 50  0000 C CNN
F 1 "UPDOWN_ONOFF_5V_GND" V 5600 3400 50  0000 C CNN
F 2 "SmartHouseLib:CONN_4_5mm" H 5500 3400 50  0000 C CNN
F 3 "" H 5500 3400 50  0000 C CNN
	1    5500 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3750 5300 3750
Wire Wire Line
	5300 3750 5300 3550
Wire Wire Line
	4450 3650 5200 3650
Wire Wire Line
	5200 3650 5200 3450
Wire Wire Line
	5200 3450 5300 3450
Wire Wire Line
	4950 3150 4950 3350
Wire Wire Line
	4950 3350 5300 3350
Wire Wire Line
	5300 3050 5300 3250
$Comp
L CONN_01X04 P1
U 1 1 5DE10C6F
P 5800 1300
F 0 "P1" H 5800 1550 50  0000 C CNN
F 1 "TX_RX_5V_GND" V 5900 1300 50  0000 C CNN
F 2 "SmartHouseLib:CONN_4_5mm" H 5800 1300 50  0000 C CNN
F 3 "" H 5800 1300 50  0000 C CNN
	1    5800 1300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4450 4350 5650 4350
Wire Wire Line
	5650 4350 5650 1500
Wire Wire Line
	5750 4250 5750 1500
Wire Wire Line
	4450 3050 5950 3050
Wire Wire Line
	5950 3050 5950 1500
Connection ~ 5300 3050
Wire Wire Line
	4450 3150 5850 3150
Wire Wire Line
	5850 3150 5850 1500
Connection ~ 4950 3150
$EndSCHEMATC
