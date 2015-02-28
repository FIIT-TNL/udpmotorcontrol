#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "scchk.h"

void setdir(int dir, int m1, int m2) {
	char c = '0' + dir;
	write(m1, &c, 1);
	c = '0' + !dir;
	write(m2, &c, 1);
}

int openGPIO(const char *number) {
	static char path[256] = "/sys/devices/virtual/gpio/gpio";
	size_t pinlen = strlen(number);

	strcpy(path + 30, number);
	strcpy(path + 30 + pinlen, "/value");

	return open(path, O_WRONLY);
}

int main(int argc, char **argv) {
	try {
		unsigned short port = 5566;

		if(argc < 5) {
			fprintf(stderr, "Usage: %s MOTORA_PIN0 MOTORA_PIN1 MOTORB_PIN0 MOTORB_PIN1 [port]\n", argv[0]);
			return 1;
		}

		int mA0 = SC_CHK(openGPIO, argv[1]);
		int mA1 = SC_CHK(openGPIO, argv[2]);
		int mB0 = SC_CHK(openGPIO, argv[3]);
		int mB1 = SC_CHK(openGPIO, argv[4]);
		
		if(argc > 5) sscanf(argv[5], "%hu", &port);

		int sfd = SC_CHK(socket, AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(port);
		saddr.sin_addr.s_addr = INADDR_ANY;

		SC_CHK(bind, sfd, (struct sockaddr *)&saddr, sizeof(saddr));

		int mAlastdir = 2, mBlastdir = 2;

		socklen_t slen;

		signed char buf[3];
		while(1) {
			slen = sizeof(saddr);
			SC_CHK(recvfrom, sfd, buf, 3, 0, (struct sockaddr *)&saddr, &slen);
			SC_CHK(sendto, sfd, buf + 2, 1, 0, (struct sockaddr *)&saddr, slen);
			int left = buf[0];
			int right = buf[1];

			if((left > 0) != mAlastdir) {
				mAlastdir = left > 0;
				setdir(mAlastdir, mA0, mA1);
			}

			if((right > 0) != mBlastdir) {
				mBlastdir = left > 0;
				setdir(mBlastdir, mB0, mB1);
			}

			if(left < 0) left = -left;
			if(right < 0) right = -right;

			printf("0=%d%%\n1=%d%%\n", left, right);
			fflush(stdout);
		}

		return 0;
	} catch(const Errno &e) {
		fprintf(stderr, "Error in %s on %u: %s\n", e.srcFile(), e.srcLine(), e.what());
	} catch(std::exception &e) {
		fprintf(stderr, "Error: %s\n", e.what());
	}

	return 1;
}
