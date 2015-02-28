#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "scchk.h"
#include "dcdriver.h"

int main(int argc, char **argv) {
	try {
		unsigned short port = 5566;

		if(argc < 5) {
			fprintf(stderr, "Usage: %s MOTORA_PIN0 MOTORA_PIN1 MOTORB_PIN0 MOTORB_PIN1 [port]\n", argv[0]);
			return 1;
		}

		DCDriver motorA(0, argv[1], argv[2]);
		DCDriver motorB(0, argv[3], argv[4]);
		
		if(argc > 5) sscanf(argv[5], "%hu", &port);

		int sfd = SC_CHK(socket, AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(port);
		saddr.sin_addr.s_addr = INADDR_ANY;

		SC_CHK(bind, sfd, (struct sockaddr *)&saddr, sizeof(saddr));

		socklen_t slen;

		signed char buf[3];
		while(1) {
			slen = sizeof(saddr);
			SC_CHK(recvfrom, sfd, buf, 3, 0, (struct sockaddr *)&saddr, &slen);
			SC_CHK(sendto, sfd, buf + 2, 1, 0, (struct sockaddr *)&saddr, slen);
			int left = buf[0];
			int right = buf[1];

			motorA.setDirection(left > 0?DCDriver::Forward:DCDriver::Backward);
			motorB.setDirection(right > 0?DCDriver::Forward:DCDriver::Backward);

			if(left < 0) left = -left;
			if(right < 0) right = -right;

			motorA.setSpeed(left);
			motorB.setSpeed(right);
		}

		return 0;
	} catch(const Errno &e) {
		fprintf(stderr, "Error in %s on %u: %s\n", e.srcFile(), e.srcLine(), e.what());
	} catch(std::exception &e) {
		fprintf(stderr, "Error: %s\n", e.what());
	}

	return 1;
}
