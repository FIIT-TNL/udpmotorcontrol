#include "dcdriver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <cstring>

#include "scchk.h"

int openGPIO(const char *number) {
	static char path[256] = "/sys/devices/virtual/gpio/gpio";
	size_t pinlen = strlen(number);

	strcpy(path + 30, number);
	strcpy(path + 30 + pinlen, "/value");

	return open(path, O_WRONLY | O_CLOEXEC);
}

DCDriver::DCDriver(int pwmID, const char *pin1, const char *pin2) : mPwmID(pwmID), mDir(None) {
	mPinFwd = SC_CHK(openGPIO, pin1);
	try {
		mPinBwd = SC_CHK(openGPIO, pin2);
	} catch(...) {
		close(mPinFwd);
		throw;
	}
}

/*
DCDriver::DCDriver(int pwmID, int pin1, int pin2) {
	
}*/

DCDriver::~DCDriver() {
	close(mPinFwd);
	close(mPinBwd);
}


void DCDriver::setDirection(Direction dir) {
	if(mDir == dir) return;
	char c;
	if(dir == None) {
		c = '0';
		write(mPinFwd, &c, 1);
		write(mPinBwd, &c, 1);
	} else {
		c = '0' + dir;
		write(mPinFwd, &c, 1);
		c = '0' + !dir;
		write(mPinBwd, &c, 1);
	}
	mDir = dir;
}

void DCDriver::setSpeed(unsigned char speed) {
	printf("%d=%u%%\n", mPwmID, (unsigned int)speed);
	fflush(stdout);
}
