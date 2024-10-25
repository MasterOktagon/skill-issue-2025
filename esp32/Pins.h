#pragma once

//
// PINOUT HEADER
//
// for Robocore board. 
// NOTE: Version compability with older versions than the V3.2 of the RoboCore has been REMOVED!
//

#define BOARD_REVISION 3

///////////////////////Input////////////////////////
#define FLEX             14
#define VBAT             15
#define T_E              16 // Menu selection
#define MPU_INT          26 // Interrupt Pin!

#define MS               41
#define T_L              18
#define T_R              17

///////////////////Light Sensors////////////////////

// Front values
#define PT_L_1           2
#define PT_L_0           4
#define PT_R_0           5
#define PT_R_1           6

// Back values
#define PT_L_3           10
#define PT_L_2           11
#define PT_R_2           12
#define PT_R_3           13

// Ref values
#define PT_REF_L         1
#define PT_REF_R         7

// LEDs
#define PT_RGB           46
#define PT_WHITE_L       36
#define PT_WHITE_R       37
#define PT_WHITE_REF     38
#define PT_RED           39
#define PT_GREEN         40

///////////////////////PWM//////////////////////////
#define SERVO1           0
#define SERVO2           1
#define SERVO3           2
#define SERVO4           3
#define SERVO5           4

#define CH5              5
#define CH6              6
#define CH7              7

#define RED_L            8
#define GREEN_L          9
#define BLUE_L           10
#define RED_R            11
#define GREEN_R          12
#define BLUE_R           13

#define PWMA1            21
#define PWMB1            34
#define PWMA2            33
#define PWMB2            35

///////////////////Shiftregister////////////////////
#define DS               42
#define SHCP             48
#define STCP             47

// Shiftregister IC10
#define SR_AIN1_F        0
#define SR_AIN1_R        1
#define SR_BIN1_F        2
#define SR_BIN1_R        3
#define SR_AIN2_F        4
#define SR_AIN2_R        5
#define SR_BIN2_F        6
#define SR_BIN2_R        7

// Shiftregister IC11
#define SR_STBY1         8
#define SR_STBY2         9
#define SR_XSHT_1        10
#define SR_XSHT_2        11
#define SR_XSHT_3        12
#define SR_XSHT_4        13
#define SR_DE1           14 // External Digital Pin
#define SR_DE2           15 // External Digital Pin

#define SR_PORTS 16 // number of ports to evaluate

///////////////////////Other////////////////////////

#define MPU6050_ADRESS   0x51
#define OLED_ADRESS      0x3C
#define TOF_ADRESS       0x29
#define TOF_ADDRESS1     0x31
#define TOF_ADDRESS2     0x32
#define TOF_ADDRESS3     0x33
#define TOF_ADDRESS4     0x34
#define BUS_ADDRES       0x40

