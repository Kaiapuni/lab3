//*************************************************************************************
/** @file adc.cpp
 *    This file contains a very simple A/D converter driver. This driver should be
 *
 *  Revisions:
 *    @li 01-15-2008 JRR Original (somewhat useful) file
 *    @li 10-11-2012 JRR Less original, more useful file with FreeRTOS mutex added
 *    @li 10-12-2012 JRR There was a bug in the mutex code, and it has been fixed
 *
 *  License:
 *    This file is copyright 2015 by JR Ridgely and released under the Lesser GNU 
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
//*************************************************************************************

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232int.h"                       // Include header for serial port class
#include "adc.h"                            // Include header for the A/D class


//-------------------------------------------------------------------------------------
/** \brief This constructor sets up an A/D converter. 
 *  \details The A/D is made ready so that when a  method such as @c read_once() is 
 *  called, correct A/D conversions can be performed. 
 */

adc::adc (emstream* p_serial_port)
{
	ptr_to_serial = p_serial_port;

	

	ADMUX |= 1 << REFS0; //enables voltage refrence AVCC and sets AREF external capacitor
	ADCSRA |= ((1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS0)); // ADC enable, start convertion and set prescaler to 32
	
	
	// Print a handy debugging message
	DBG (ptr_to_serial, "A/D constructor OK" << endl);
}


//-------------------------------------------------------------------------------------
/** @brief   This method takes one A/D reading from the given channel and returns it. 
 *  @param   ch The A/D channel which is being read must be from 0 to 7
 *  @return  The result of the A/D conversion
 */

uint16_t adc::read_once (uint8_t ch)
{
	uint8_t ADSC_Check = 0;  // Initialize variable to check conversion completion
	uint16_t ADC_out = 0;  // Initalize result
	
	if (ch > 7)  // Saturate ch at 7
	{
	  ch=7;
	}
	ADMUX &= ~(7<<MUX0);
	ADMUX |= ch<<MUX0;  // Sets MUX registers to proper channel
	ADCSRA |= 1<<ADSC;  // Begin conversion
	ADSC_Check = (ADCSRA & 1<<ADSC);    // Set value of variable to check conversion completion
	while (ADCSRA & 1<<ADSC) // Wait until ADSC is zero
	{
	}
	ADC_out = ((uint16_t) ADCL | (uint16_t) ADCH<<8);  // Concatenate Low and High bytes of ADC results
	return ADC_out;
}


//-------------------------------------------------------------------------------------
/** @brief   This method does @b NOTHING because it hasn't been written yet.
 *  \details Nothing, nothing, nothing!
 *  @param   bacon
 *  @param   waffles
 *  @return  Is it Tuesday yet? (Answer: a Boolean value.)
 */

uint16_t adc::read_oversampled (uint8_t channel, uint8_t samples)
{
    static uint16_t average_result = 0;  // Initialize the value to be returned
    uint16_t ro_result;  // Initialize the result of calling read_once
    
    if(samples >= 64) // Saturate the amount of samples that can be taken at 255
    {
      samples = 64;  // 255 is the greatest amount of the highest 8-bit number can fit into a 16-bit number
    }
    for(uint8_t i=0; i <= samples; i++)  // Average as many times as there are samples
    {
	ro_result = read_once(channel);  // Read once
	
	if(average_result == 0)  //  Do not divide the first sample by 2
	{
	  average_result = ro_result;
	}
	else  // Average the previous average with the new sample
	{
	  average_result = (average_result + ro_result) / 2;
	}
    }

    return average_result;
	/*
	DBG (ptr_to_serial, "All your readings are belong to us" << endl);
	*ptr_to_serial << "MY MESSAGE" << endl;
	return (0xFFFF);
	*/
}


//-------------------------------------------------------------------------------------
/** \brief   This overloaded operator "prints the A/D converter." 
 *  \details The precise meaning of print is left to the user to interpret; it should 
 *           be something useful. For example, one might print the values of the 
 *           configuration registers or show current readings from all the A/D 
 *           channels.
 *  @param   serpt Reference to a serial port to which the printout will be printed
 *  @param   a2d   Reference to the A/D driver which is being printed
 *  @return  A reference to the same serial device on which we write information.
 *           This is used to string together things to write with @c << operators
 */

emstream& operator << (emstream& serpt, adc& a2d)
{
	// Prints usefull information to the serial port
	serpt << PMS ("The A/D Converter Values")<<endl 
	      << PMS ("ADMUX value: ")<< bin << ADMUX << dec << endl
	      << PMS ("ADCSRA value: ")<< bin<< ADCSRA << dec << endl
	      << PMS ("Average result channel 0: ")<< a2d.read_oversampled (0, 10) << endl
	      << PMS ("Average result channel 1: ")<< a2d.read_oversampled (1, 10) << endl;
	
	
	return (serpt);
}

