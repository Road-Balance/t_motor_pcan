#include <iostream>
#include "ManualReadWrite.h"

#include <unistd.h>

int main()
{
	// TimerWrite start;
    auto start = std::make_shared<ManualReadWrite>();

	uint8_t id = 0x601;
	float pos = 0.0;

	start->StartStatusFeedback();

	while (true){
		start->WriteMessages(id, pos);
		// start->GetMotorMessageLeft();
		sleep(1);
	}

	// motor data example
	// 1a a0 ef de 0 3f 22 0 
	
	return 0;
}