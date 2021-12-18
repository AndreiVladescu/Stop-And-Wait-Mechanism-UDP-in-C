#include "utils.h"

int main(int argc, char** argv)
{
	if (argc != 2){
		printf("Usage: %s <output_file>", argv[0]);
		exit(0);
	}

	struct sockaddr_in serverAddr, clientAddr;
	Packet recvPacket, sentPacket;
	int sockfd, slen = sizeof(clientAddr), recv_len, rc, frame_id = 0;
	FILE* filePointer;
	bool resendFlag = true;

	// File Pointer opening
	filePointer = fopen(argv[1], "w");

	// UDP Socket creation
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	rc = sockfd;
	DIE(rc == -1, "socket");

	// Zeroing the structure
	memset((char *) &serverAddr, 0, sizeof(serverAddr));

	// Setting up socket
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Binding the socket
	rc = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	DIE(rc == -1, "bind");

	printf("[!] Preparing to receive..\n");
	// Loop for listening to infinity and beyond
	while(true){
		RECEIVE_TRY_AGAIN:

		// Try to receive data
		// No MSG_DONTWAIT flag, so this is a blocking call
		recv_len = recvfrom(sockfd, &recvPacket, sizeof(recvPacket), 0, (struct sockaddr *) &clientAddr, &slen);
		rc = recv_len;
		DIE (rc == -1, "recvfrom");

		// Output to console received message
		printf("[+] Received packet no %d: %s\n", recvPacket.seq_no, recvPacket.data);

		// Simulate [ACK] lost on the first run of the server
		if (resendFlag){
			printf("[!] The package arrived, but we test stop-and-wait capability!\n");
			resendFlag = false;
			goto RECEIVE_TRY_AGAIN;
		}

		// If received packet has the FIN flag set, we break out of the loop
		if (IsTransferDone(recvPacket)){
			break;
		}

		// Write received data to the file pointer
		fputs(recvPacket.data, filePointer);
		memset(&(recvPacket.data[0]), 0, sizeof(recvPacket.data));

		// Prepare ACK packet
		sentPacket.seq_no = frame_id;
		frame_id ++;
		SetACK(&sentPacket);

		// Send ACK packet
		rc = sendto(sockfd, &sentPacket, sizeof(sentPacket), 0, (struct sockaddr*) &clientAddr, slen);
		DIE(rc == -1, "sendto");

	}

	// Done
	printf("[*] File received succesfully!\n");

	// Close File Pointer
	fclose(filePointer);
	// Close Socket
	close(sockfd);

	return 0;
}
