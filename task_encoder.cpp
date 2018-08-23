#include "textqueue.h"                      // Header for text queue class
#include "task_motor.h"                // Header for this task
#include "shares.h"                         // Shared inter-task communications


task_encoder::task_encoder (const char* a_name,
				unsigned portBASE_TYPE a_priority,
				size_t a_stack_size,
				emstream* p_ser_dev)
	: TaskBase (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------


void task_motor::run (void)
{
	
}

