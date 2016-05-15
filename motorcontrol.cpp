#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "scchk.h"
#include "packetreader.h"

#define MAXPACKET 128

int setupUDP(unsigned short port) {
  int sfd = SC_CHK(socket, AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);

  struct sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;

  SC_CHK(bind, sfd, (struct sockaddr *)&saddr, sizeof(saddr));

  return sfd;
}

int main(int argc, char **argv) {
  try {
    unsigned short port = 5566;

    if(argc < 5) {
      fprintf(stderr, "Usage: %s  DRIVE_PWM_PIN DRIVE_PIN_FWD DRIVE_PIN_BWD STEER_SERVO_ID YAW_SERVO_ID [STEER_MIN_PULSE_US] [STEER_MAX_PULSE][YAW_MIN_PULSE] [YAW_MAX_PULSE] [PORT]", argv[0]);
      fprintf(stderr, "  All pulses are in us. If max is 0, range confured with servoblaster is used.\n");
      return 1;
    }
printf("NASRAC %d argumentow\n", argc);

    //DCDriver motorA(0, argv[1], argv[2]);
    //DCDriver motorB(1, argv[3], argv[4]);

    // DC to drive
    DCDriver dcDrive(atoi(argv[1]), argv[2], argv[3]);
    
    // Steering servo
    unsigned int steer_min = 0;
    unsigned int steer_max= 0;
    if (argc > 7) {
        steer_min = strtoul(argv[6], NULL, 10);
        steer_max = strtoul(argv[7], NULL, 10);
    }
    ServoDriver servoSteer(atoi(argv[4]), steer_min, steer_max);
    
    // Yaw for camaera position servo
    unsigned int yaw_min = 0;
    unsigned int yaw_max= 0;
    if (argc > 9) {
        yaw_min = strtoul(argv[8], NULL, 10);
        yaw_max = strtoul(argv[9], NULL, 10);
    }
    ServoDriver servoYaw(atoi(argv[5]), yaw_min, yaw_max);

    int redled = open("/sys/class/gpio/gpio25/value", O_WRONLY);
    
    if(argc > 11) sscanf(argv[10], "%hu", &port);


    struct pollfd pfd;
    pfd.fd = setupUDP(port);
    pfd.events = POLLIN | POLLPRI;

    PacketReader reader(pfd.fd, dcDrive, servoSteer, servoYaw);

    unsigned int timeoutCnt = 0;

    //printf("NASRAC sleepin 3s\n");
    //system("sleep 3");

    /*
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7777);
    inet_aton("", &saddr.sin_addr); */

    //reader.readPacket(&saddr);
    reader.readPacket();
    while(1) {
      if(SC_CHK(poll, &pfd, 1, 200)) {
        reader.readPacket();
        timeoutCnt = 0;
        write(redled, "0", 1);
      } else {
        ++timeoutCnt;
        if(timeoutCnt > 4) {
          dcDrive.setSpeed(0);
          write(redled, "1", 1);
        }
      }
      //sendto(pfd.fd, "Hello world!", 13, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    }

    close(redled);
    return 0;
  } catch(const Errno &e) {
    fprintf(stderr, "Error in %s on %u: %s\n", e.srcFile(), e.srcLine(), e.what());
  } catch(std::exception &e) {
    fprintf(stderr, "Error: %s\n", e.what());
  }

  return 1;
}
