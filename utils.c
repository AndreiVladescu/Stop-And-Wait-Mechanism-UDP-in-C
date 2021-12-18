#include "utils.h"

bool IsTransferDone(Packet package){
	uint8_t fin_flag = 0b00010000;
	return (package.flags & fin_flag) ? true : false;
}

bool IsItAck(Packet package){
	uint8_t ack_flag = 0b00000001;
	return (package.flags & ack_flag) ? true : false;
}

void SetFIN(Packet* package){
	package->flags = 0b00010000;
}

void SetACK(Packet* package){
	package->flags = 0b00000001;
}
