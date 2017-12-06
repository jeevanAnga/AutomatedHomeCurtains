#ifndef BLUCHIP_BOARD_H__
#define BLUCHIP_BOARD_H__

#include <stdint.h>
#include <stdbool.h>

// J2 header (side w/ LED, on the right) 
#define J2_1 00
#define J2_2 04
#define J2_3 26
#define J2_4 21
#define J2_5 23
#define J2_6 27

// J1 header (programming side, on the left)
#define J1_1 03
#define J1_2 25

// Test Points
#define TP_1 17
#define TP_2 19

// J3 4-pin header
#define J3_1 01
#define J3_2 02
#define J3_3 06
#define J3_4 05

#define ONBOARD_LED 17	

// BluChip Explorer setup
//
// Pin P27, J2_1, is AIN1 
// (refer to nRF51822 Product Specification, section "Pin functions")
#define MAKERKIT_LIGHT_SENSOR_AIO LPCOMP_PSEL_PSEL_AnalogInput1
#define MAKERKIT_LIGHT_SENSOR_SRC J2_5
#define MAKERKIT_LED_1_SRC J1_1
#define MAKERKIT_LED_1_SNK J1_2
#define MAKERKIT_LED_2_SRC J2_1
#define MAKERKIT_LED_2_SNK J2_2
#define MAKERKIT_LIGHT_SENSOR_RESISTOR J2_6




typedef enum Light_Status_t {
	NEW_DARK = 0,
	DARK = 1,
	NEW_LIGHT = 2,
	LIGHT = 3	
} Light_Status_t;

#endif
