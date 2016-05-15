#include <stdio.h>
#include <stdlib.h>

class DCDriver {
  public:
    typedef enum {
      Forward = 0,
      Backward = 1,
      None = 2
    } Direction;

    DCDriver(int pinPwm, const char *pinFwd, const char *pinBwd);
    ~DCDriver();

    void setDirection(Direction dir);
    void setSpeed(unsigned char speed);
  private:
    FILE *mPiBlaster;
    int mPinPwm;
    int mPinFwd, mPinBwd;
    Direction mDir;
};
