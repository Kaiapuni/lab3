#ifndef _TASK_encoder_H_
#define _TASK_encoder_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Header for special function registers
#include <avr/interrupt.h>

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues

#include "taskbase.h"                       // ME405/507 base task class
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "taskqueue.h"                      // Header of wrapper for FreeRTOS queues
#include "taskshare.h"                      // Header for thread-safe shared data

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "adc.h"                            // Header for A/D converter driver class
#include "motor_dr.h"




class task_encoder : public TaskBase
{
private:
	// No private variables or methods for this class

protected:
	// No protected variables or methods for this class

public:
	task_encoder (const char*, unsigned portBASE_TYPE, size_t, emstream*);

	void run (void);
};

#endif // _TASK_encoder_H_