#include <stdio.h>

class ServoDriver {
  public:

    ServoDriver(int pwmID);
    ServoDriver(int pwmID, unsigned int min, unsigned int max);
    ~ServoDriver();

    void setPosition(unsigned char speed);
  private:
    int mPwmID;
    FILE *mServoBlaster;
    unsigned int mMin;
    unsigned int mMax;
};
