#include "servodriver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cmath>

#include "scchk.h"
ServoDriver::ServoDriver(int pwmID) : 
  mPwmID(pwmID), 
  mMin(0), 
  mMax(0) {
    printf("SERVO: SB-id %d, pulse range <%u,%u>us\n", mPwmID, mMin, mMax);
    mServoBlaster = fopen("/dev/servoblaster", "w");
}

ServoDriver::ServoDriver(int pwmID, unsigned int min, unsigned int max) : 
  mPwmID(pwmID), 
  mMin(min), 
  mMax(max) {
    printf("SERVO: SB-id %d, pulse range <%u,%u>us\n", mPwmID, mMin, mMax);
    mServoBlaster = fopen("/dev/servoblaster", "w");
}

ServoDriver::~ServoDriver() {
  fclose(mServoBlaster);
}

void ServoDriver::setPosition(unsigned char position) {
    if (position > 100) position = 100;
  
    if (mMax) {
        // Fit to range <mMin, mMax>
        unsigned int usecs = mMin + (int)((double)position * (mMax - mMin) / 100);
        fprintf(mServoBlaster, "%u=%uus\n", mPwmID, usecs);
        //printf("%u=%uus\n", mPwmID, usecs);
    } else {
        fprintf(mServoBlaster, "%u=%u%%\n", mPwmID, (unsigned int) position);
        //printf("%u=%u%%\n", mPwmID, (unsigned int) position);
    }
    fflush(mServoBlaster);

}
