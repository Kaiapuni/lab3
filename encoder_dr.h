#ifndef encoder_dr
#define encoder_dr

#include "emstream.h"                       // Header for serial ports and devices
#include "FreeRTOS.h"                       // Header for the FreeRTOS RTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // Header for FreeRTOS queues
#include "semphr.h"                         // Header for FreeRTOS semaphores


class Encoder_dr
{
protected:
  
private:
	emstream* ptr_to_serial;
	volatile uint8_t* p_isr_cntl;
	uint8_t isr_pin1;
	uint8_t isr_pin2;
	volatile uint8_t* p_isr_enable;
	uint8_t isr_enable_pin1;
	uint8_t isr_enable_pin2;
	
	volatile uint8_t* p_encoder_DDR;
	volatile uint8_t* p_encoder_PORT;
	uint8_t encoder_pin;
	
	
  
public:
  
	Encoder_dr(emstream* ptr_to_serial, volatile uint8_t* p_isr_cntl, uint8_t isr_pin1, uint8_t isr_pin2,
	    volatile uint8_t* p_isr_enable, uint8_t isr_enable_pin1, uint8_t isr_enable_pin2, 
	    volatile uint8_t* p_encoder_DDR, volatile uint8_t* p_encoder_PORT);
  
};

emstream& operator << (emstream&, ISR&);

#endif 