#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>

#include "scchk.h"
#include "packetreader.h"

#define MAXPACKET 128

int setupUDP(unsigned short port) {
	int sfd = SC_CHK(socket, AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	SC_CHK(bind, sfd, (struct sockaddr *)&saddr, sizeof(saddr));

	return sfd;
}

int main(int argc, char **argv) {
	try {
		unsigned short port = 5566;

		if(argc < 5) {
			fprintf(stderr, "Usage: %s MOTORA_PIN0 MOTORA_PIN1 MOTORB_PIN0 MOTORB_PIN1 [port]\n", argv[0]);
			return 1;
		}

		DCDriver motorA(0, argv[1], argv[2]);
		DCDriver motorB(1, argv[3], argv[4]);
		
		if(argc > 5) sscanf(argv[5], "%hu", &port);

		struct pollfd pfd;
		pfd.fd = setupUDP(port);
		pfd.events = POLLIN | POLLPRI;

		PacketReader reader(pfd.fd, motorA, motorB);

		unsigned int timeoutCnt = 0;

		struct sockaddr_in saddr;
		reader.readPacket(&saddr);
		while(1) {
			if(SC_CHK(poll, &pfd, 1, 200)) {
				reader.readPacket();
				timeoutCnt = 0;
			} else {
				++timeoutCnt;
				if(timeoutCnt > 4) {
					motorA.setSpeed(0);
					motorB.setSpeed(0);
				}
			}
			sendto(pfd.fd, "Hello world!", 13, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
		}

		return 0;
	} catch(const Errno &e) {
		fprintf(stderr, "Error in %s on %u: %s\n", e.srcFile(), e.srcLine(), e.what());
	} catch(std::exception &e) {
		fprintf(stderr, "Error: %s\n", e.what());
	}

	return 1;
}
