#ifndef AHC_H__
#define AHC_H__

#include "ble_ahc_service.h"	// BLE Service library.

//define system components
#define AHC_SERVICE
#define STEPPER
#define LIMIT_SWITCHES
#undef LDR

/*
//define application mode flags
#define AUTOMATED_HOME_CURTAIN 0x01	// Normal operation.
#define SUNRISE_SUNSET 0x02	// Closes & Opens the curtain based on the value of the LDR(ambient light sensor).
#define TUG_CONTROL 0x04	// Opens & Closes the curtain when tugged enough to cause limit switches to release.

//set system mode
#define MODE_ AUTOMATED_HOME_CURTAIN // | SUNRISE_SUNSET | TUG_CONTROL
*/

#ifdef AHC_SERVICE
	static ble_ahc_service_t    					 m_ahc_service; 
#endif

//pin configuration

#define STATUS_LED_SOURCE_PIN 00							// Curtain status LED.
#define AMBIENT_LIGHT_SENSOR_AIN_PIN 27				// Ambient brightness LDR.

#define STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A 23	// Stepper IN 1A.
#define STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A 21	// Stepper IN 2A.
#define STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A 26	// Stepper IN 3A.
#define STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A 04	// Stepper IN 4A.

#define OPENED_LIMIT_SWITCH_NO_INPUT_PIN 03	// Opened(left) limit switch.
#define CLOSED_LIMIT_SWITCH_NO_INPUT_PIN 25 // Closed(right) limit switch.


//Stepper

#define CURTAIN_OPEN_STEPS 12000	// When limit switch not used, motor is turned (12000 Steps / 200 Steps per Revolution) = 60 Revolutions.
#define CURTAIN_CLOSE_STEPS 12000	// When limit switch not used, motor is turned (12000 Steps / 200 Steps per Revolution) = 60 Revolutions.

volatile uint32_t STEPPER_MOTOR_STEP_DELAY = 2500;	// (microseconds).

void stepperMotorStep1() {
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);
  nrf_delay_us(STEPPER_MOTOR_STEP_DELAY);
}
void stepperMotorStep2() {
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);
  nrf_delay_us(STEPPER_MOTOR_STEP_DELAY);
}
void stepperMotorStep3() {
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);
  nrf_delay_us(STEPPER_MOTOR_STEP_DELAY);
}
void stepperMotorStep4() {
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);
  nrf_gpio_pin_set(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);
  nrf_delay_us(STEPPER_MOTOR_STEP_DELAY);
}

void motorOff() {
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);
  nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);
}

void stepCounterClockwise()
{
  stepperMotorStep1();
  stepperMotorStep2();
  stepperMotorStep3();
  stepperMotorStep4();
}

void stepClockwise()
{
  stepperMotorStep4();
  stepperMotorStep3();
  stepperMotorStep2();
  stepperMotorStep1();
}

void openCurtain()
{
#ifdef LIMIT_SWITCHES
	while(nrf_gpio_pin_read(OPENED_LIMIT_SWITCH_NO_INPUT_PIN) == 1)	// While opened limit switch released.
#else
	for (int x = 0; x < CURTAIN_OPEN_STEPS; x++)
#endif
  {
    stepClockwise();
  }
	motorOff();
}

void closeCurtain()
{
#ifdef LIMIT_SWITCHES
	while(nrf_gpio_pin_read(CLOSED_LIMIT_SWITCH_NO_INPUT_PIN) == 1)	// While closed limit switch released.
#else
	for (int x = 0; x < CURTAIN_CLOSE_STEPS; x++)
#endif
  {
    stepCounterClockwise();
  }
	motorOff();
}


//BLE AHC Service

static void curtain_write_handler(ble_ahc_service_t * p_nus, uint8_t * p_data, uint16_t length)
{
	 // Messages are expected in the format 
	 // 1-byte - Curtain direction (0-close/1-open)
	 
	if (length < 1) {
		// Not a valid message
		return;
	} 
	
	uint8_t direction = p_data[0];
	if (direction) {
		nrf_gpio_pin_set(STATUS_LED_SOURCE_PIN);	// Turn curtain status LED on.
		openCurtain();
		nrf_gpio_pin_clear(STATUS_LED_SOURCE_PIN);	// Turn curtain status LED off.
	} else {
		nrf_gpio_pin_set(STATUS_LED_SOURCE_PIN);	// Turn curtain status LED on.
		closeCurtain();
		nrf_gpio_pin_clear(STATUS_LED_SOURCE_PIN);	// Turn curtain status LED off.
	}
	
}
 
static void ahc_service_init()
{
    uint32_t       err_code;
	  ble_ahc_service_init_t ahc_service_init; 
	
    ahc_service_init.curtain_write_handler = curtain_write_handler; 
    
	  err_code = ble_ahc_service_init(&m_ahc_service, &ahc_service_init);
    APP_ERROR_CHECK(err_code);
	
}

void	add_ahc_service_event_handler_to_application_event_dispatcher(ble_evt_t * p_ble_evt){
	
	ble_ahc_service_on_ble_evt(&m_ahc_service, p_ble_evt);

}

//MAIN

void ahc_init() {
	
//ONBOARD_LED_EN
	nrf_gpio_cfg_output(ONBOARD_LED);	// Set Pin017 as output.
	nrf_gpio_pin_clear(ONBOARD_LED);	// Drive Pin017 LOW (Onboard LED tied to Pin017 is active low, this turns it on).
	
//LDR
	nrf_gpio_cfg_input(J2_6, NRF_GPIO_PIN_PULLDOWN);	// Set Pin027 as input with internal pulldown resistor.
	
//CURTAIN_STATUS_LED
	
	nrf_gpio_cfg_output(STATUS_LED_SOURCE_PIN);	// Configure motor status LED source pin as output(Set Pin00 as output).
	nrf_gpio_pin_clear(STATUS_LED_SOURCE_PIN);	// Turn motor status LED off(Drive Pin00 LOW).
		
//STEPPER

  nrf_gpio_cfg_output(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);	// Set Pin023(1A) as output.
  nrf_gpio_cfg_output(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);	// Set Pin021(2A) as output.
  nrf_gpio_cfg_output(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);	// Set Pin026(3A) as output.
  nrf_gpio_cfg_output(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);	// Set Pin04(4A) as output.
	nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_1A);		// Drive Pin023(1A) LOW.
	nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_2A);		// Drive Pin021(2A) LOW.
	nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_3A);		// Drive Pin026(3A) LOW.
	nrf_gpio_pin_clear(STEPPER_MOTOR_DRIVER_INPUT_SOURCE_PIN_4A);		// Drive Pin04(4A) LOW.
	
//LIMIT_SWITCHES

	nrf_gpio_cfg_input(OPENED_LIMIT_SWITCH_NO_INPUT_PIN, NRF_GPIO_PIN_PULLUP);	// Set Pin03(Opened) as input with internal pullup resistor.
	nrf_gpio_cfg_input(CLOSED_LIMIT_SWITCH_NO_INPUT_PIN, NRF_GPIO_PIN_PULLUP);	// Set Pin025(Closed) as input with internal pullup resistor.
	
}

#endif // AHC_H__
/** @} */
