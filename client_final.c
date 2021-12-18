#include "utils.h"

int main(int argc, char** argv)
{
  if (argc != 2){
		printf("Usage: %s <input_file>", argv[0]);
		exit(0);
	}

	struct sockaddr_in clientAddr;
  Packet recvPacket, sentPacket;
	int sockfd, slen = sizeof(clientAddr), rc, prev_frame_id = 0;
	char message[BUFFLEN];
  FILE* filePointer;

  // File Pointer opening
	filePointer = fopen(argv[1], "r");

  // UDP Socket creation
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	rc = sockfd;
	DIE(rc == -1, "socket");

  // Zeroing the structure
	memset((char *) &clientAddr, 0, sizeof(clientAddr));

  // Setting up socket
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);

  // Transform ipv4 address to binary form
	rc = inet_aton(SERVER , &clientAddr.sin_addr);
	DIE(rc == 0, "inet_aton");

  printf("[!] Preparing to send..\n");
  // Loop for sending while filePointer runs out of juice
	while(fgets(message, BUFFLEN, filePointer)){
		RETRY_SEND_AGAIN:

    // Prepare the packet to be sent
    sentPacket.seq_no = prev_frame_id;
    strcpy(sentPacket.data, message);

		// Send the packet
		rc = sendto(sockfd, &sentPacket, sizeof(sentPacket) , 0 , (struct sockaddr *) &clientAddr, slen);
    DIE(rc == -1, "sendto");

    // Output to console sent message
		printf("[!] Sent packet no %d: %s\n", sentPacket.seq_no, sentPacket.data);

    // Wait for TIMEOUT micro-seconds
    // If it didn't arrive by then, the message will be resent
		usleep(TIMEOUT);

    // MSG_DONTWAIT is set - this function is NOT blocking
    // If it didn't receive the package it goes back to the RETRY_SEND_AGAIN label
		rc = recvfrom(sockfd, &recvPacket, sizeof(recvPacket), MSG_DONTWAIT, (struct sockaddr *) &clientAddr, &slen);
    if (rc == -1){
      printf("[~] Timeout exceeded.\n");
      goto RETRY_SEND_AGAIN;
    }

    // Output it
    printf("[+] The server received packet %d\n", prev_frame_id);

    // Or not an Ack
    // Or not the same sequence number
		if (recvPacket.seq_no != prev_frame_id || !IsItAck(recvPacket))
			goto RETRY_SEND_AGAIN;

    // Increment frame id
		prev_frame_id++;
	}

  // Configure last packet to let the server know it's done
  sentPacket.seq_no = prev_frame_id;
  SetFIN(&sentPacket);

  // Send FIN packet
  rc = sendto(sockfd, &sentPacket, sizeof(sentPacket) , 0 , (struct sockaddr *) &clientAddr, slen);
	DIE(rc == -1, "sendto");

  // Done
  printf("[*] File sent succesfully!\n");

  // Close File Pointer
	fclose(filePointer);
	// Close Socket
	close(sockfd);

	return 0;
}
