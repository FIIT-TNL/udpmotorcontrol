#include "dcdriver.h"

class PacketReader {
	public:
		typedef enum {
			Ping = 0,
			DCControl = 1,
			ServoControl = 2,
			Discover = 3
		} PacketType;

		PacketReader(int sock, DCDriver &motorA, DCDriver &motorB);
		~PacketReader();

		void readPacket();
		void readPacket(struct sockaddr_in *saddr);
	private:
		int mFD;
		DCDriver &mMotorA, &mMotorB;

		void parsePacket(const signed char *packet, const struct sockaddr_in *saddr);
		void parseDCCmd(const signed char *packet);
		void parseServoCmd(const signed char *packet);
		void parsePing(const signed char *packet);
		void parseDiscover(const signed char *packet, const struct sockaddr_in *saddr);
};
