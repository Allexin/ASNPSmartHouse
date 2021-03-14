EESchema Schematic File Version 4
EELAYER 30 0
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
L SmartHouseDINSwitchController-rescue:STM32DISCOVERY-SmartHouseDINDMXDimmer-rescue U1
U 1 1 5B0C4D35
P 2850 3950
F 0 "U1" H 3300 3200 60  0000 C CNN
F 1 "STM32DISCOVERY" H 2900 4850 60  0000 C CNN
F 2 "SmartHouseLib:STM32_DISCOVERY" H 3300 3200 60  0001 C CNN
F 3 "" H 3300 3200 60  0001 C CNN
	1    2850 3950
	1    0    0    -1  
$EndComp
$Comp
L SmartHouseDINSwitchController-rescue:CONN_01X02-SmartHouseDINDMXDimmer-rescue P2
U 1 1 5B0C654C
P 5000 4300
F 0 "P2" H 5000 4450 50  0000 C CNN
F 1 "UART" V 5100 4300 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 5000 4300 50  0000 C CNN
F 3 "" H 5000 4300 50  0000 C CNN
	1    5000 4300
	1    0    0    -1  
$EndComp
$Comp
L SmartHouseDINSwitchController-rescue:CONN_01X02-SmartHouseDINDMXDimmer-rescue P1
U 1 1 5B0C6718
P 3550 1700
F 0 "P1" H 3550 1850 50  0000 C CNN
F 1 "POWER" V 3650 1700 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 3550 1700 50  0000 C CNN
F 3 "" H 3550 1700 50  0000 C CNN
	1    3550 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4800 4250 4450 4250
Wire Wire Line
	4450 4350 4800 4350
Wire Wire Line
	3500 1900 3500 3150
Wire Wire Line
	3600 3050 4450 3050
Wire Wire Line
	3500 3150 4450 3150
Wire Wire Line
	3600 1900 3600 3050
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 603FEBEC
P 650 3350
F 0 "J1" H 542 3025 50  0000 C CNN
F 1 "Conn_01x02_Female" H 542 3116 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 650 3350 50  0001 C CNN
F 3 "~" H 650 3350 50  0001 C CNN
	1    650  3350
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J2
U 1 1 603FFC1C
P 650 3550
F 0 "J2" H 542 3225 50  0000 C CNN
F 1 "Conn_01x02_Female" H 542 3316 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 650 3550 50  0001 C CNN
F 3 "~" H 650 3550 50  0001 C CNN
	1    650  3550
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 60400520
P 650 3750
F 0 "J3" H 542 3425 50  0000 C CNN
F 1 "Conn_01x02_Female" H 542 3516 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 650 3750 50  0001 C CNN
F 3 "~" H 650 3750 50  0001 C CNN
	1    650  3750
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 60400A56
P 650 3950
F 0 "J4" H 542 3625 50  0000 C CNN
F 1 "Conn_01x02_Female" H 542 3716 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 650 3950 50  0001 C CNN
F 3 "~" H 650 3950 50  0001 C CNN
	1    650  3950
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 60400DAF
P 650 4150
F 0 "J5" H 542 3825 50  0000 C CNN
F 1 "Conn_01x02_Female" H 542 3916 50  0000 C CNN
F 2 "SmartHouseLib:CONN_2_3.5mm" H 650 4150 50  0001 C CNN
F 3 "~" H 650 4150 50  0001 C CNN
	1    650  4150
	-1   0    0    1   
$EndComp
Connection ~ 3600 3050
Wire Wire Line
	850  3350 1400 3350
Wire Wire Line
	1400 3450 1200 3450
Wire Wire Line
	850  3550 1150 3550
Wire Wire Line
	1400 3650 1100 3650
Wire Wire Line
	850  3750 1050 3750
Wire Wire Line
	850  3950 950  3950
Wire Wire Line
	1400 4050 900  4050
Wire Wire Line
	850  4150 1400 4150
$Comp
L Device:R R6
U 1 1 60402FA9
P 1300 2500
F 0 "R6" H 1370 2546 50  0000 L CNN
F 1 "R" H 1370 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1230 2500 50  0001 C CNN
F 3 "~" H 1300 2500 50  0001 C CNN
	1    1300 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 60403744
P 1200 2500
F 0 "R5" H 1270 2546 50  0000 L CNN
F 1 "R" H 1270 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1130 2500 50  0001 C CNN
F 3 "~" H 1200 2500 50  0001 C CNN
	1    1200 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 60403906
P 1100 2500
F 0 "R4" H 1170 2546 50  0000 L CNN
F 1 "R" H 1170 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1030 2500 50  0001 C CNN
F 3 "~" H 1100 2500 50  0001 C CNN
	1    1100 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 60403A16
P 1000 2500
F 0 "R3" H 1070 2546 50  0000 L CNN
F 1 "R" H 1070 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 930 2500 50  0001 C CNN
F 3 "~" H 1000 2500 50  0001 C CNN
	1    1000 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 60403C01
P 900 2500
F 0 "R2" H 970 2546 50  0000 L CNN
F 1 "R" H 970 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 830 2500 50  0001 C CNN
F 3 "~" H 900 2500 50  0001 C CNN
	1    900  2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 60403F6A
P 800 2500
F 0 "R1" H 870 2546 50  0000 L CNN
F 1 "R" H 870 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 730 2500 50  0001 C CNN
F 3 "~" H 800 2500 50  0001 C CNN
	1    800  2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 60404202
P 1400 2500
F 0 "R7" H 1470 2546 50  0000 L CNN
F 1 "R" H 1470 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1330 2500 50  0001 C CNN
F 3 "~" H 1400 2500 50  0001 C CNN
	1    1400 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 604043C8
P 1500 2500
F 0 "R8" H 1570 2546 50  0000 L CNN
F 1 "R" H 1570 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1430 2500 50  0001 C CNN
F 3 "~" H 1500 2500 50  0001 C CNN
	1    1500 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 604045A4
P 1600 2500
F 0 "R9" H 1670 2546 50  0000 L CNN
F 1 "R" H 1670 2455 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 1530 2500 50  0001 C CNN
F 3 "~" H 1600 2500 50  0001 C CNN
	1    1600 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  3050 850  3250
Wire Wire Line
	850  4150 850  4250
Wire Wire Line
	850  4250 600  4250
Wire Wire Line
	600  4250 600  2650
Wire Wire Line
	600  2650 800  2650
Connection ~ 850  4150
Wire Wire Line
	900  2650 900  4050
Connection ~ 900  4050
Wire Wire Line
	900  4050 850  4050
Wire Wire Line
	1000 2650 950  2650
Wire Wire Line
	950  2650 950  3950
Connection ~ 950  3950
Wire Wire Line
	950  3950 1400 3950
Wire Wire Line
	1100 2650 1100 2700
Wire Wire Line
	1100 2700 1000 2700
Wire Wire Line
	1000 2700 1000 3850
Connection ~ 1000 3850
Wire Wire Line
	1000 3850 850  3850
Wire Wire Line
	1400 3850 1000 3850
Wire Wire Line
	1050 3750 1050 2750
Wire Wire Line
	1050 2750 1200 2750
Wire Wire Line
	1200 2750 1200 2650
Connection ~ 1050 3750
Wire Wire Line
	1050 3750 1400 3750
Wire Wire Line
	1100 3650 1100 2800
Wire Wire Line
	1100 2800 1300 2800
Wire Wire Line
	1300 2800 1300 2650
Connection ~ 1100 3650
Wire Wire Line
	1100 3650 850  3650
Wire Wire Line
	1150 3550 1150 2850
Wire Wire Line
	1150 2850 1400 2850
Wire Wire Line
	1400 2850 1400 2650
Connection ~ 1150 3550
Wire Wire Line
	1150 3550 1400 3550
Wire Wire Line
	1200 3450 1200 2900
Wire Wire Line
	1200 2900 1500 2900
Wire Wire Line
	1500 2900 1500 2650
Connection ~ 1200 3450
Wire Wire Line
	1200 3450 850  3450
Wire Wire Line
	1600 2650 1600 2800
Wire Wire Line
	1600 2800 1800 2800
Wire Wire Line
	1800 2800 1800 3350
Wire Wire Line
	1800 3350 1400 3350
Connection ~ 1400 3350
Wire Wire Line
	800  2350 900  2350
Connection ~ 900  2350
Wire Wire Line
	900  2350 1000 2350
Connection ~ 1000 2350
Wire Wire Line
	1000 2350 1100 2350
Connection ~ 1100 2350
Wire Wire Line
	1100 2350 1200 2350
Connection ~ 1200 2350
Wire Wire Line
	1200 2350 1300 2350
Connection ~ 1300 2350
Wire Wire Line
	1300 2350 1400 2350
Connection ~ 1400 2350
Wire Wire Line
	1400 2350 1500 2350
Connection ~ 1500 2350
Wire Wire Line
	1500 2350 1600 2350
Wire Wire Line
	1600 2350 3200 2350
Wire Wire Line
	3200 2350 3200 2950
Wire Wire Line
	3200 2950 4450 2950
Connection ~ 1600 2350
Wire Wire Line
	850  3050 3600 3050
$EndSCHEMATC