
#ifndef _UTILS_H_
#define _UTILS_H_	1

#include <stdio.h> // printf
#include <stdlib.h> // memset
#include <stdint.h> // int types
#include <stdbool.h> // bool type
#include <arpa/inet.h> // sockets
#include <sys/socket.h> // sockets

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while (0)

#define BUFFLEN 512 // Maximum buffer length
#define PORT 1683 // Operating port
#define TIMEOUT 1000000 // Predefined timeout until protocol starts sending again
#define SERVER "127.0.0.1"

typedef struct packet{
	char data[BUFFLEN];
	uint8_t flags; // ACK = 0000 0001, FIN = 0001 0000
	uint32_t seq_no; // 4294967296 number of possible packets sent before overflow
}Packet;

bool IsTransferDone(Packet package);
bool IsItAck(Packet package);
void SetFIN(Packet* package);
void SetACK(Packet* package);

#endif
