#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SC_CHK(SYSCALL, ...) if(SYSCALL(__VA_ARGS__) < 0) { perror(#SYSCALL); return 1; }

void setdir(int dir, int m1, int m2) {
	char c = '0' + dir;
	write(m1, &c, 1);
	c = '0' + !dir;
	write(m2, &c, 1);
}

int main(int argc, char **argv) {
	unsigned short port = 5566;

	if(argc < 5) {
		fprintf(stderr, "Usage: %s MOTORA_PIN0 MOTORA_PIN1 MOTORB_PIN0 MOTORB_PIN1 [port]\n", argv[0]);
		return 1;
	}

	char path[256] = "/sys/devices/virtual/gpio/gpio";
	size_t pinlen = strlen(argv[1]);
	strcpy(path + 30, argv[1]);
	strcpy(path + 30 + pinlen, "/value");
	int mA0 = open(path, O_WRONLY);
	
	pinlen = strlen(argv[2]);
	strcpy(path + 30, argv[2]);
	strcpy(path + 30 + pinlen, "/value");
	int mA1 = open(path, O_WRONLY);
	
	pinlen = strlen(argv[3]);
	strcpy(path + 30, argv[3]);
	strcpy(path + 30 + pinlen, "/value");
	int mB0 = open(path, O_WRONLY);
	
	pinlen = strlen(argv[4]);
	strcpy(path + 30, argv[4]);
	strcpy(path + 30 + pinlen, "/value");
	int mB1 = open(path, O_WRONLY);
	

	if(argc > 5) sscanf(argv[5], "%hu", &port);

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd < 0) {
		perror("socket");
		return 1;
	}

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
		sendto(sfd, buf + 2, 1, 0, (struct sockaddr *)&saddr, slen);
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
}
