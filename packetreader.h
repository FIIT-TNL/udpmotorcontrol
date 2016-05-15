#include "dcdriver.h"
#include "servodriver.h"

class PacketReader {
  public:
    typedef enum {
      Ping = 0,
      DriveControl = 1,
      CameraControl = 2,
      Discover = 3
    } PacketType;

    PacketReader(int sock, DCDriver &dcDrive, ServoDriver &servoSteer, ServoDriver &servoYaw);
    ~PacketReader();

    void readPacket();
    void readPacket(struct sockaddr_in *saddr);
  private:
    int mFD;
    DCDriver &mDcDrive;
    ServoDriver &mServoSteer, &mServoYaw;

    void parsePacket(const signed char *packet, const struct sockaddr_in *saddr);
    void parseDriveCmd(const signed char *packet);
    void parseCameraCmd(const signed char *packet);
    void parsePing(const signed char *packet);
    void parseDiscover(const signed char *packet, const struct sockaddr_in *saddr) const;
};
