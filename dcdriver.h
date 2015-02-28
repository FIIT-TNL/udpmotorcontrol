class DCDriver {
	public:
		typedef enum {
			Forward = 0,
			Backward = 1,
			None = 2
		} Direction;

		DCDriver(int pwmID, const char *pin1, const char *pin2);
		//DCDriver(int pwmID, int pin1, int pin2);
		~DCDriver();

		void setDirection(Direction dir);
		void setSpeed(unsigned char speed);
	private:
		int mPwmID;
		int mPinFwd, mPinBwd;
		Direction mDir;
};
