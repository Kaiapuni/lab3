//======================================================================================
/** @file motor_dr.h
 *    This file contains the header for the motor driver which will provide use of
 *    multiple motors within the ME405 AVR based board
 *
 *  Revisions:
 *    @li 1/19/2016 created first instance of the motor driver class
 *
 */

//======================================================================================

// This define prevents this .H file from being included multiple times in a .CPP file
#ifndef MOTOR_DR
#define MOTOR_DR

#include "emstream.h"                       // Header for serial ports and devices
#include "FreeRTOS.h"                       // Header for the FreeRTOS RTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // Header for FreeRTOS queues
#include "semphr.h"                         // Header for FreeRTOS semaphores


//-------------------------------------------------------------------------------------
/** @brief   This constructor creates a motor driver that is able to driver multiple
 *              motors at a single time
 *  @details Utilizes generic pointers to registers and pin-outs on the AVR board
 *              that when called set appropriate registry values and pin configurations
 *              to get motors running.  It also contains two methods for setting the
 *              speed of the motor and allows the motor to brake
 *
 */

class Motor_driver
{
private:
  // No private variables.
protected:
    emstream* ptr_to_serial;
    volatile uint8_t* ina_PORT;
    volatile uint8_t* ina_DDR;
    uint8_t ina_pin;
    volatile uint8_t* diag_DDR;
    volatile uint8_t* diag_PORT;
    uint8_t diag_pin;
    volatile uint8_t* pwm_DDR;
    volatile uint8_t* pwm_PORT;
    uint8_t pwm_pin;
    volatile uint16_t* duty_OCR;

public:
    Motor_driver(emstream* p_serial_port,
		 volatile uint8_t* my_ina_PORT, uint8_t my_ina_pin,
		 volatile uint8_t* my_diag_PORT, uint8_t my_diag_pin,
                 volatile uint8_t* my_pwm_PORT, uint8_t my_pwm_pin,
		 volatile uint16_t* my_duty_OCR);

    void set_power (int16_t);

    void brake (int16_t);
};

emstream& operator << (emstream&, Motor_driver&);

#endif // MOTOR_DR
