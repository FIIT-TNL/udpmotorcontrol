#include "dcdriver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>

#include "scchk.h"

int openGPIO(const char *number) {
	// TODO: usage of virtual device may be needed: "/sys/devices/virtual/gpio/gpio"
	static char path[256] = "/sys/class/gpio/gpio";
	size_t pinlen = strlen(number);

	strcpy(path + 20, number);
	strcpy(path + 20 + pinlen, "/value");

	return open(path, O_WRONLY | O_CLOEXEC);
}

DCDriver::DCDriver(int pinPwm,  const char *pinFwd, const char *pinBwd) : 
  mPinPwm(pinPwm), 
  mDir(None) {
   /* 
	mPinFwd = SC_CHK(openGPIO, pinFwd);
	try {
		mPinBwd = SC_CHK(openGPIO, pinBwd);
	} catch(...) {
		close(mPinFwd);
		throw;
	}
    */
    mPinFwd = atoi(pinFwd);
    mPinBwd = atoi(pinBwd);
    printf("DC: pinPWM %d, pinFwd %d, pinBwd %d\n", mPinPwm, mPinFwd, mPinBwd);
    mPiBlaster = fopen("/dev/pi-blaster", "w");
}

DCDriver::~DCDriver() {
	close(mPinFwd);
	close(mPinBwd);
    fclose(mPiBlaster);
}

void DCDriver::setDirection(Direction dir) {
	if(mDir == dir) return;
	char c;
	if(dir == None) {
		c = '0';
	    //write(mPinFwd, &c, 1);
		//write(mPinBwd, &c, 1);
	    fprintf(mPiBlaster, "%d=0\n", mPinFwd);
	    fprintf(mPiBlaster, "%d=0\n", mPinBwd);
	    //printf("%d=0\n", mPinFwd);
	    //printf("%d=0\n", mPinBwd);
    	fflush(mPiBlaster);
	} else {
		c = '0' + dir;
		//write(mPinFwd, &c, 1);
	    fprintf(mPiBlaster, "%d=%c\n", mPinFwd, c);
	    //printf("%d=%c %d\n", mPinFwd, c, dir);
		c = '0' + !dir;
		//write(mPinBwd, &c, 1);
	    fprintf(mPiBlaster, "%d=%c\n", mPinBwd, c);
	    //printf("%d=%c\n", mPinBwd, c);
	}
    fflush(mPiBlaster);
	mDir = dir;
}

void DCDriver::setSpeed(unsigned char speed) {
	if(!speed) setDirection(None);
	fprintf(mPiBlaster, "%d=%.2lf\n", mPinPwm, (double)speed / 100);
	fflush(mPiBlaster);
	//printf("%d=%.2lf\n", mPinPwm, (double)speed / 100);
}
