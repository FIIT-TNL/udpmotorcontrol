#include "packetreader.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <cstdio>

#include "scchk.h"

#define MAXPACKET 128

PacketReader::PacketReader(int sock, DCDriver &motorA, DCDriver &motorB) :
	mFD(sock),
	mMotorA(motorA),
	mMotorB(motorB) {
	
}

PacketReader::~PacketReader() {
	close(mFD);
}


void PacketReader::readPacket() {
	signed char buf[256];
	SC_CHK(::read, mFD, buf, 256);
	parsePacket(buf);
}

void PacketReader::readPacket(struct sockaddr_in *saddr) {
	socklen_t slen = sizeof(struct sockaddr_in);
	signed char buf[256];
	SC_CHK(::recvfrom, mFD, buf, 256, 0, (struct sockaddr *)saddr, &slen);
	fprintf(stderr, "packet received\n");
	parsePacket(buf);
}

void PacketReader::parsePacket(const signed char *packet) {
	switch((PacketReader::PacketType)packet[0]) {
		case Ping:
			parsePing(packet + 1);
		case DCControl:
			parseDCCmd(packet + 1);
		case ServoControl:
			parseServoCmd(packet + 1);
	}
}

void PacketReader::parseDCCmd(const signed char *packet) {
	int left = packet[0];
	int right = packet[1];
	mMotorA.setDirection(left > 0?DCDriver::Forward:DCDriver::Backward);
	mMotorB.setDirection(right > 0?DCDriver::Forward:DCDriver::Backward);

	if(left < 0) left = -left;
	if(right < 0) right = -right;

	mMotorA.setSpeed(left);
	mMotorB.setSpeed(right);
}

void PacketReader::parseServoCmd(const signed char *packet) {
	unsigned int vert = packet[1];
	unsigned int horiz = packet[2];

	printf("%d=%u%%\n", 2, vert);
	printf("%d=%u%%\n", 3, horiz);
	fflush(stdout);                                  
}

void PacketReader::parsePing(const signed char *packet) {
	
}

