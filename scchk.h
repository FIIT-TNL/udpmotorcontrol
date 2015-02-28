#ifndef SCCHK_H
#define SCCHK_H

#include <stdexcept>

class Errno : public std::runtime_error {
	public:
		Errno(const char *name, const char *filename, unsigned int line);
		~Errno() throw();

		const char *srcFile() const;
		unsigned int srcLine() const;
	private:
		const char *mSrcFile;
		unsigned int mSrcLine;
};

#define SC_CHK(SYSCALL, ...) sc_chk(SYSCALL(__VA_ARGS__), #SYSCALL, __FILE__, __LINE__)

int sc_chk(int ret, const char *name, const char *filename, unsigned int line);

inline const char *Errno::srcFile() const {
	return mSrcFile;
}

inline unsigned int Errno::srcLine() const {
	return mSrcLine;
}

#endif
