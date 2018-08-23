//*************************************************************************************
/** @file motor_dr.cpp
 *    This file contains a very simple motor driver for the ME405 custom mechatronics
 *    board powered by a AVR microcontroller
 *
 *  Revisions:
 *    @li 1/19/2016 Original creation of the basic motor driver
 *
 */
//*************************************************************************************

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232int.h"                       // Include header for serial port class
#include "motor_dr.h"                       // Include header for the A/D class


//-------------------------------------------------------------------------------------
/** \brief This constructor sets up a Motor driver.
 *  \details Provides basic framework for initiating multiple motors
 *  @param ina_PORT pointer to a registry for output data of both INA and INB and also
 *          allows one to initiate the DDR register to configure these pins
 *  @param ina_pin variable to describe which pin INA and INB will be using
 *  @param diag_PORT pointer to data register for the diagnostics of the motor driver
 *          and allows set up for DDR of that port as well
 *  @param diag_pin variable to describe which pin the diagnostics are using
 *  @param pwm_PORT pointer to the data register the pwm control uses and also allows
 *          the set up of the DDR register
 *  @param pwm_pin variable describing which pin is used for the pwm control
 *  @param duty_OCR access to the register for the configuration of the timers
 *          comparator that is used for setting the duty cycle of the pwm
 */

Motor_driver::Motor_driver(emstream* p_serial_port,
			   volatile uint8_t* my_ina_PORT, uint8_t my_ina_pin,
			   volatile uint8_t* my_diag_PORT, uint8_t my_diag_pin,
			   volatile uint8_t* my_pwm_PORT, uint8_t my_pwm_pin,
			   volatile uint16_t* my_duty_OCR)
{
	ptr_to_serial = p_serial_port;

	ina_PORT = my_ina_PORT;
	ina_DDR = my_ina_PORT -1; //DDR register is one address below the data port
	ina_pin = my_ina_pin;
	diag_PORT = my_diag_PORT;
	diag_DDR = my_diag_PORT - 1; //DDR register is one address below the data port
	diag_pin = my_diag_pin;
	pwm_PORT = my_pwm_PORT;
	pwm_DDR = my_pwm_PORT - 1; //DDR register is one address below the data port
	pwm_pin = my_pwm_pin;
	duty_OCR = my_duty_OCR;

	DBG (ptr_to_serial, "Motor Driver constructor OK" << endl);
}
/** This method sets the power/speed of the motor. A positive number causes a clockwise
 *   torque and a negative number causes a counter clockwise torque. It also initiates
 *   and configures the proper data registers and pin-outs to control the motor
 *
 *  @param power_in signed variable that allows the motors speed/power to be set
 *
 */
void Motor_driver::set_power(int16_t power_in)
{
    *ina_DDR |= (1 << ina_pin) | (1 << (ina_pin + 1));
    //ina_DDR sets both the INA and INB pins as outputs
    *diag_DDR &= ~(1 << diag_pin);
    //clears the diagnostics DDR to ensure pin is an input
    *diag_PORT |= (1 << diag_pin);
    //sets the diagnostics pin pull up resistor
    *pwm_DDR |= (1 << pwm_pin);
    // sets the pwm pin as an output

    if(power_in >= 0)
    {
        *ina_PORT |= (1 << ina_pin);
        //sets INA to HIGH
        *ina_PORT &= ~(1 << (ina_pin +1));
        //sets INB to LOW
        *duty_OCR = abs(power_in);
        // places power in data into the comparators config register for the duty

    }
    else if(power_in < 0)
    {
        *ina_PORT |= (1 << (ina_pin + 1));
        //sets INB to HIGH
        *ina_PORT &= ~(1 << (ina_pin));
        //sets INA to LOW
        *duty_OCR = abs(power_in);
        // places power in data into the comparators config register for the duty
    }
    else{}


}

/** This method allows the motor to be braked by setting both INA and INB to high logic
 *   values. It also initiates and configures the proper data registers and pin-outs
 *   to control the motor
 *
 *  @param power_in signed variable that allows the motors speed/power to be set
 */

void Motor_driver::brake(int16_t power_in)
{
    *ina_DDR |= (1 << ina_pin) | (1 << (ina_pin + 1));
    //ina_DDR sets both the INA and INB pins as outputs
    *diag_DDR &= ~(1 << diag_pin);
    //clears the diagnostics DDR to ensure pin is an input
    *diag_PORT |= (1 << diag_pin);
    //sets the diagnostics pin pull up resistor
    *pwm_DDR |= (1 << pwm_pin);
    // sets the pwm pin as an output

    *ina_PORT |= (1 << ina_pin)|(1 << (ina_pin +1));
    //sets both INA and INB to HIGH
    *duty_OCR = abs(power_in);
    // places power in data into the comparators config register for the duty

}



//-------------------------------------------------------------------------------------
/** \brief  This method provides access to serial port prints
 *  \details allows messages to be printed to serial port for debugging purposes
 *  @return  A reference to the same serial device on which we write information.
 *           This is used to string together things to write with @c << operators
 */

emstream& operator << (emstream& serpt, Motor_driver& mdr)
{
	// Prints usefull information to the serial port
	serpt << PMS ("Motor Driver")<<endl;


	return (serpt);
}

