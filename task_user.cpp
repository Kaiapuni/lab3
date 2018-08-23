//**************************************************************************************
/** @file task_user.cpp
 *    This file contains source code for a user interface task for a ME405/FreeRTOS
 *    test suite. And now allows the user to control multiple motors seperatly and
 *    efficiently
 *
 *  Revisions:
 *    @li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    @li 10-05-2012 JRR Split into multiple files, one for each task
 *    @li 10-25-2012 JRR Changed to a more fully C++ version with class task_user
 *    @li 11-04-2012 JRR Modified from the data acquisition example to the test suite
 *    @li 01-04-2014 JRR Changed base class names to TaskBase, TaskShare, etc.
 *
 *  License:
 *    This file is copyright 2012 by JR Ridgely and released under the Lesser GNU
 *    Public License, version 2. It intended for educational use only, but its use
 *    is not limited thereto. */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 *    TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//**************************************************************************************

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header

#include "task_user.h"                      // Header for this file


/** This constant sets how many RTOS ticks the task delays if the user's not talking.
 *  The duration is calculated to be about 5 ms.
 */
const TickType_t ticks_to_delay = ((configTICK_RATE_HZ / 1000) * 5);


//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */

task_user::task_user (const char* a_name,
					  unsigned portBASE_TYPE a_priority,
					  size_t a_stack_size,
					  emstream* p_ser_dev
					 )
	: TaskBase (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/** This task interacts with the user and allows them to have control over multiple motors
 */

void task_user::run (void)
{
	char char_in;                           // Character read from serial device
	time_stamp a_time;                      // Holds the time so it can be displayed
	int8_t number_entered = 0;            // Holds a number being entered by user
	bool num_sign = 0;
	uint8_t motor_sel = 0;
	
	// Tell the user how to get into command mode (state 1), where the user interface
	// task does interesting things such as diagnostic printouts
	*p_serial << PMS ("Press 'h' or '?' for help") << endl;

	// This is an infinite loop; it runs until the power is turned off. There is one
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 0, we're in command mode, so when the user types characters,
			// the characters are interpreted as commands to do something
			case (0):
				if (p_serial->check_for_char ())            // If the user typed a
				{                                           // character, read
					char_in = p_serial->getchar ();         // the character

					// In this switch statement, we respond to different characters as
					// commands typed in by the user
					switch (char_in)
					{
						// The 't' command asks what time it is right now
						case ('t'):
							*p_serial << (a_time.set_to_now ()) << endl;
							break;

						// The 's' command asks for version and status information
						case ('s'):
							show_status ();
							break;

						// The 'd' command has all the tasks dump their stacks
						case ('d'):
							print_task_stacks (p_serial);
							break;

						// The 'm' command enters you into motor control
						case ('m'):
							*p_serial << endl
							  << PMS ("You are in motor control") << endl
							  << PMS ("press 'h' for help") << endl;
							transition_to(2);
							break;

						// The 'h' command is a plea for help; '?' works also
						case ('h'):
						case ('?'):
							print_help_message ();
							break;

						// A control-C character causes the CPU to restart
						case (3):
							*p_serial << PMS ("Resetting AVR") << endl;
							wdt_enable (WDTO_120MS);
							for (;;);
							break;

						// If character isn't recognized, ask What's That Function?
						default:
							*p_serial << '"' << char_in << PMS ("\": WTF?") << endl;
							break;
					}; // End switch for characters
				} // End if a character was received

				break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, wait for user to enter digits and build 'em into a number
			case (1):
				if (p_serial->check_for_char ())        // If the user typed a
				{                                       // character, read
					char_in = p_serial->getchar ();     // the character

					// Respond to numeric characters, Enter or Esc only. Numbers are
					// put into the numeric value we're building up
					
					if (char_in == 'r')//return function
					{
						*p_serial << PMS ("Moved to motor control home") << endl;
						transition_to(2);//returns to motor control home state
					}
					
					//decrement power step
					else if (char_in == '<')
					{
						*p_serial << PMS ("Decrement power by 1") << endl;
						number_entered = get_power(motor_sel);//access shared value
						number_entered -= 1;
						set_power(motor_sel, number_entered);//set decremented value
					}
					
					//increment power step
					else if (char_in == '>')
					{
						*p_serial << PMS ("Increment power by 1") << endl;
						number_entered = get_power(motor_sel);//acess shared var
						number_entered += 1;
						set_power(motor_sel, number_entered);//set shared var	
					}
					
					//catch for negative value and sets flag for negative or not
					else if (char_in == '-')
					{
						*p_serial << char_in;
						num_sign = 1;
					}
					
					//convert char into binary equivelant 
					else if (char_in >= '0' && char_in <= '9')
					{
						*p_serial << char_in;
						number_entered *= 10;
						number_entered += char_in - '0';
					}
					// Carriage return is ignored; the newline character ends the entry
					else if (char_in == 10)
					{
						*p_serial << "\r";
					}
					// Carriage return or Escape ends numeric entry
					else if (char_in == 13 || char_in == 27)
					{
						if(num_sign == 0)//added functionality based on sign of val
						{
							*p_serial << endl << PMS ("Number entered: ")
									  << number_entered << endl;
							transition_to (4);//return to power state
						}
						else
						{
							*p_serial << endl << PMS ("Number entered: -")
									  << number_entered << endl;
							number_entered *= -1;//add the negative value
							transition_to (4);//return to power state
						}
						
						  
					}
					else
					{
						*p_serial << PMS ("<invalid char \"") << char_in
								  << PMS ("\">");
					}
				}

				// Check the print queue to see if another task has sent this task
				// something to be printed
				else if (p_print_ser_queue->check_for_char ())
				{
					p_serial->putchar (p_print_ser_queue->getchar ());
				}

				break; // End of state 1

			case(2):        //case for motor control
				if (p_serial->check_for_char ())        // If the user typed a
				{                                       // character, read
					char_in = p_serial->getchar ();     // the character
					switch(char_in)
					{
						case('h')://help message
							motor_control_home_message();
							break;
						case('t'): //will enter state for motor select
							*p_serial << PMS ("enter motor number (a or b):  ") << endl;
							transition_to(5);//case 5 is for motor select
							break;
						case('s'): // transitions to motor control state
							*p_serial << PMS ("enter value for motor control: ") << endl
								  << PMS ("press h for help") << endl;
							transition_to(3);
							break;
						case('p'): //will shift into power set state
							*p_serial << PMS ("enter value for motor power: ") << endl;
							number_entered = 0;//reseting num_entered 
							transition_to(1);
							break;
						case('r'): //goes back to original menu
							*p_serial << PMS ("Moved to initial command mode") << endl;
							transition_to(0);
							break;
					}
				}
				break; //end of state 2
				
			case(3):    //motor control state
				if (p_serial->check_for_char ())        // If the user typed a
				{                                       // character, read
					char_in = p_serial->getchar ();     // the character
					switch(char_in)
					{
						case('h'): //help message
							motor_control_message();
							break;
						case('p')://sets motor to potentiometer mode
							*p_serial << PMS ("Moved to initial command mode") << endl;
							set_control(motor_sel, 0);//sets proper state for shared var
							break;
						case('u')://sets motor to user input mode
							*p_serial << PMS ("User input mode") << endl;
							*p_serial << PMS ("Moved to motor control home") << endl;
							set_control(motor_sel, 1);//sets proper state for shared var
							transition_to(2);//move back to motor control menu
							break;
						case('b')://sets motor into brake 
							*p_serial << PMS ("Brake on") << endl;
							set_control(motor_sel, 2);//sets proper state for shared var
							break;
						case('r'):
							*p_serial << PMS ("Moved to motor control home") << endl;
							transition_to(2);
							break;

					}
					//p_motor_state.put();
				}
				break; //end of case 3
			case(4):    //motor power state
				{       
					//catch for input into the power of the motor
					if(number_entered >= -127 && number_entered <=  127)
					{
						set_power(motor_sel, number_entered);//sets the user input to motor power
						number_entered = 0;//clears input variable
						num_sign = 0;//clears sign var flag
						transition_to(1);// go back to value input
					}
					else
					{
						*p_serial << PMS ("invalid entry, enter again:  ") << endl;
						number_entered = 0;
						num_sign = 0;
						transition_to(1);
					}
				}
				break;
			case(5):
				if (p_serial->check_for_char ())        // If the user typed a
				{                                       // character, read
					char_in = p_serial->getchar ();     // the character
					
					//accept character to change  motor select to allow individual operation of
					//the motors
					if(char_in == 'a')
					{
						motor_sel = 0;
						*p_serial << PMS ("First motor selected") << endl;
						transition_to(2);
					}
					else if(char_in == 'b')
					{
						motor_sel = 1;
						*p_serial << PMS ("Second motor selected") << endl;
						transition_to(2);
					}
					else
					{
						*p_serial << PMS ("not a valid motor") << endl;
					}
				}
				break;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// We should never get to the default state. If we do, complain and restart
			default:
				*p_serial << PMS ("Illegal state! Resetting AVR") << endl;
				wdt_enable (WDTO_120MS);
				for (;;);
				break;



		} // End switch state

		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we
		// run the loop again. This gives lower priority tasks a chance to run
		delay_ms (1);
	}
}


//-------------------------------------------------------------------------------------
/** This method prints a simple help message.
 */

void task_user::print_help_message (void)
{
	*p_serial << PROGRAM_VERSION << PMS (" help") << endl;
	*p_serial << PMS ("  m:     Enter motor control") << endl;
	*p_serial << PMS ("  t:     Show the time right now") << endl;
	*p_serial << PMS ("  s:     Version and setup information") << endl;
	*p_serial << PMS ("  d:     Stack dump for tasks") << endl;
	*p_serial << PMS ("  n:     Enter a number (demo)") << endl;
	*p_serial << PMS ("  Ctl-C: Reset the AVR") << endl;
	*p_serial << PMS ("  h:     HALP!") << endl;
}

//help message for motor control home menu
void task_user::motor_control_home_message (void)
{
	*p_serial << endl
		  << PMS ("  s: state entry mode") << endl
		  << PMS ("  t: toggle motor select") << endl
		  << PMS ("  p: power entry") << endl
		  << PMS ("  r: return to initial command mode") << endl;

}

//help message for motor control menu
void task_user::motor_control_message (void)
{
	*p_serial << endl
		  << PMS ("  p: potentiometer mode") << endl
		  << PMS ("  u: user set mode") << endl
		  << PMS ("  b: brake") << endl
		  << PMS ("  r: return") << endl
		  << PMS ("  h: help") << endl;
}

//method to get power from shared var depending on motor select
int8_t task_user::get_power (uint8_t mtr_cntl)
{
	if(mtr_cntl == 0)
	{
		return p_motor_power->get();
	}
	else if (mtr_cntl == 1)
	{
		return p_motor_power2->get();
	}
}

//sets power of shared based on motor select
void task_user::set_power (uint8_t mtr_cntl, int8_t val)
{
	if(mtr_cntl == 0)
	{
		p_motor_power->put(val);
	}
	else if (mtr_cntl == 1)
	{
		p_motor_power2->put(val);
	}  
}

//sets control value of shared vars based on motor control
void task_user::set_control (uint8_t mtr_cntl, uint8_t val)
{
	if(mtr_cntl == 0)
	{
		p_motor_state->put(val);
	}
	else if (mtr_cntl == 1)
	{
		p_motor_state2->put(val);
	}  
}

//-------------------------------------------------------------------------------------
/** This method displays information about the status of the system, including the
 *  following:
 *    \li The name and version of the program
 *    \li The name, status, priority, and free stack space of each task
 *    \li Processor cycles used by each task
 *    \li Amount of heap space free and setting of RTOS tick timer
 */

void task_user::show_status (void)
{
	time_stamp the_time;					// Holds current time for printing

	// First print the program version, compile date, etc.
	*p_serial << endl << PROGRAM_VERSION << PMS (__DATE__) << endl
			  << PMS ("System time: ") << the_time.set_to_now ()
			  << PMS (", Heap: ") << heap_left() << "/" << configTOTAL_HEAP_SIZE
			  #ifdef OCR5A
				<< PMS (", OCR5A: ") << OCR5A << endl << endl;
			  #elif (defined OCR3A)
				<< PMS (", OCR3A: ") << OCR3A << endl << endl;
			  #else
				<< PMS (", OCR1A: ") << OCR1A << endl << endl;
			  #endif

	// Have the tasks print their status; then the same for the shared data items
	print_task_list (p_serial);
	*p_serial << endl;
	print_all_shares (p_serial);
}

