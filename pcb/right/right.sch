EESchema Schematic File Version 4
EELAYER 26 0
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
$Sheet
S 6100 3100 600  800 
U 59E157CB
F0 "KeyMatrix" 60
F1 "../shared/keymatrix.sch" 60
F2 "SDA" I L 6100 3700 60 
F3 "SCL" I L 6100 3500 60 
F4 "RESET" I L 6100 3300 60 
$EndSheet
NoConn ~ 5000 4300
NoConn ~ 5600 4300
$Comp
L power:+5V #PWR010
U 1 1 5A3F6A2D
P 4800 4000
F 0 "#PWR010" H 4800 3850 50  0001 C CNN
F 1 "+5V" H 4800 4140 50  0000 C CNN
F 2 "" H 4800 4000 50  0001 C CNN
F 3 "" H 4800 4000 50  0001 C CNN
	1    4800 4000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 5A3F6A6E
P 5800 4000
F 0 "#PWR011" H 5800 3750 50  0001 C CNN
F 1 "GND" H 5800 3850 50  0000 C CNN
F 2 "" H 5800 4000 50  0001 C CNN
F 3 "" H 5800 4000 50  0001 C CNN
	1    5800 4000
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG04
U 1 1 5A424761
P 4800 4000
F 0 "#FLG04" H 4800 4075 50  0001 C CNN
F 1 "PWR_FLAG" H 4800 4150 50  0000 C CNN
F 2 "" H 4800 4000 50  0001 C CNN
F 3 "" H 4800 4000 50  0001 C CNN
	1    4800 4000
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5A42418B
P 5800 4000
F 0 "#FLG02" H 5800 4075 50  0001 C CNN
F 1 "PWR_FLAG" H 5800 4150 50  0000 C CNN
F 2 "" H 5800 4000 50  0001 C CNN
F 3 "" H 5800 4000 50  0001 C CNN
	1    5800 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	5500 4000 5800 4000
Connection ~ 5800 4000
Wire Wire Line
	5500 4000 5500 4300
Wire Wire Line
	5400 4300 5400 3700
Wire Wire Line
	5400 3700 6100 3700
Wire Wire Line
	5300 3500 5300 4300
Wire Wire Line
	5300 3500 6100 3500
Wire Wire Line
	5100 4300 5100 4000
Wire Wire Line
	5100 4000 4800 4000
Connection ~ 4800 4000
NoConn ~ 5700 4300
$Comp
L Connector:RJ45 J?
U 1 1 5A4FD93E
P 5350 4750
F 0 "J?" H 4912 4698 50  0000 R CNN
F 1 "RJ45" H 4912 4789 50  0000 R CNN
F 2 "" H 5350 4750 50  0001 C CNN
F 3 "" H 5350 4750 50  0001 C CNN
	1    5350 4750
	-1   0    0    1   
$EndComp
Wire Wire Line
	5200 3300 5200 4300
Wire Wire Line
	5200 3300 6100 3300
NoConn ~ 4800 5100
$EndSCHEMATC
