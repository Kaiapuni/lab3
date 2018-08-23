#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>
#include <avr/interrupt.h>

#include "rs232int.h"                       // Include header for serial port class
#include "encoder_dr.h"                       // Include header for the A/D class




Encoder_dr::Encoder_dr(emstream* p_serial, volatile uint8_t* my_p_isr_cntl, uint8_t my_isr_pin1, uint8_t my_isr_pin2,
	 volatile uint8_t* my_p_isr_enable, uint8_t my_isr_enable_pin1, uint8_t my_isr_enable_pin2, 
	    volatile uint8_t* my_p_encoder_DDR, volatile uint8_t* my_p_encoder_PORT)
{
	ptr_to_serial = p_serial;
	p_isr_cntl = my_p_isr_cntl;
	isr_pin1 = my_isr_pin1;
	isr_pin2 = my_isr_pin2;
	p_isr_enable = my_p_isr_enable;
	isr_enable_pin1 = my_isr_enable_pin1;
	isr_enable_pin2 = my_isr_enable_pin2;
	p_encoder_DDR = my_p_encoder_DDR;
	p_encoder_PORT = my_p_encoder_PORT;
}

void Encoder_dr::encoder_setup ()
{
	*p_isr_control |= (1 << isr_pin1) | (1 << (isr_pin1 + 1)) | (1 << isr_pin2) | (1 << (isr_pin2 + 1));
	*p_isr_enable  |= (1 << isr_enable_pin1) | (1 << isr_enable_pin2);
	
	*p_encoder_DDR &= ~();
	*p_encoder_PORT |= ();
}

ISR(INT5_vect)
{
	switch(p_encoder_state->get())
	{
		case (0): //case 10
			break;
		case (1): //case 11
			break;
		case (2): //case 01
			break;
		case (3): //case 00
			break;
	}
}
ISR(INT6_vect, ISR_ALIASOF(INT5_vect));

