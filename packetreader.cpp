#include "packetreader.h" 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>


#include <cstdio>
#include <cstring>

#include "scchk.h"

#define MAXPACKET 128

struct sockaddr_in rpiaddr;

PacketReader::PacketReader(int sock, DCDriver &dcDrive, ServoDriver &servoSteer, ServoDriver &servoYaw) :
  mFD(sock),
  mDcDrive(dcDrive),
  mServoSteer(servoSteer),
  mServoYaw(servoYaw) {
    static bool rpiaddr_initialized = false;
    if(!rpiaddr_initialized) {
      rpiaddr.sin_family = AF_INET;
      rpiaddr.sin_port = htons(5566);
      inet_aton("192.168.129.2", &rpiaddr.sin_addr);
      rpiaddr_initialized = true;
    }
  
}

PacketReader::~PacketReader() {
  close(mFD);
}

void PacketReader::readPacket() {
  struct sockaddr_in saddr;
  readPacket(&saddr);
}

void PacketReader::readPacket(struct sockaddr_in *saddr) {
  socklen_t slen = sizeof(struct sockaddr_in);
  signed char buf[256];
  SC_CHK(::recvfrom, mFD, buf, 256, 0, (struct sockaddr *)saddr, &slen);
 // fprintf(stderr, "packet received %2x %2x %2x\n", (int)buf[0], (int)buf[1], (int)buf[2]);
  parsePacket(buf, saddr);
}

void PacketReader::parsePacket(const signed char *packet, const struct sockaddr_in *saddr) {
  switch((PacketReader::PacketType)packet[0]) {
    case Ping:
      parsePing(packet + 1);
      break;
    case DriveControl:
      parseDriveCmd(packet + 1);
      break;
    case CameraControl:
      parseCameraCmd(packet + 1);
      break;
    case Discover:
      parseDiscover(packet + 1, saddr);
      break;
  }
}

void PacketReader::parseDriveCmd(const signed char *packet) {
  int speed = packet[0];
  int direction = packet[1];
  
  
  mDcDrive.setDirection(speed > 0?DCDriver::Forward:DCDriver::Backward);
  mDcDrive.setSpeed(abs(speed));
  
  if (direction > 100) direction = 100;
  else if (direction < -100) direction = -100;
 
  direction = (100 + direction) / 2;
  mServoSteer.setPosition(direction);
}

void PacketReader::parseCameraCmd(const signed char *packet) {
  int yaw = packet[0];
  //unsigned int pitch = packet[1];
  //unsigned int roll = packet[2];

  if (yaw > 100) yaw = 100;
  else if (yaw < -100) yaw = -100;
  
  char ypos = (unsigned char) (100 + yaw) / 2;
  mServoYaw.setPosition(ypos);
}

void PacketReader::parsePing(const signed char *packet) {
  
}

void PacketReader::parseDiscover(const signed char *packet, const struct sockaddr_in *saddr) const {
  if(strcmp((char *)packet, "FIIT_TechNoLogic_Motorcontrol_Discover"))
    return;
  fprintf(stderr, "Discover received\n");
  SC_CHK(::sendto, mFD, "FIIT_TechNoLogic_Motorcontrol_ACK", 33, 0, (const struct sockaddr *)saddr, sizeof(struct sockaddr_in));
  system("killall -9 raspivid");
  system("pkill -9 sendtemp.sh");
  system("ssh pi@192.168.129.2 killall -9 raspivid");
  char buf[1024];
  char *ip = inet_ntoa(saddr->sin_addr);
  //sprintf(buf, "ssh pi@192.168.129.2 /home/pi/raspivid -t 999999 -o udp://%s:5567 -w 540 -h 480 -b 850000 &>/dev/null &", ip);
  // sprintf(buf, "/home/pi/raspivid -t 999999 -o udp://%s:5568 -w 540 -h 480 -b 850000 &>/dev/null &", ip);

  int w = 1080;
  int h = 960;
  int bitrate = 850000;
  int fps = 25;
  sprintf(buf, "ssh pi@192.168.129.2 /home/pi/raspivid -t 999999 -o udp://%s:5567 -w %d -h %d -b %d -fps %d &>/dev/null &", ip, w, h, bitrate);
  system(buf);
  sprintf(buf, "/home/pi/raspivid -t 999999 -o udp://%s:5568 -w %d -h %d -b %d -fps %d&>/dev/null &", ip, w, h, bitrate, fps);
  system(buf);
  sprintf(buf, "/home/pi/udpmotorcontrol/sendtemp.sh %s 5566 2>/dev/null &", ip);
  system(buf);
  int gpiofd = open("/sys/class/gpio/gpio23/value", O_WRONLY);
  write(gpiofd, "1", 1);
  close(gpiofd);
}
