#include "scchk.h"

#include <errno.h>
#include <cstring>
#include <string>

using namespace std;

int sc_chk(int ret, const char *name, const char *filename, unsigned int line) {
	if(ret < 0) {
		throw Errno(name, filename, line);
	}
	return ret;
}

static std::string genErrorMsg(const char *name) {
	std::string msg(name);
	msg += ": ";
	msg += strerror(errno);
	return msg;
}

Errno::Errno(const char *name, const char *filename, unsigned int line) :
	std::runtime_error(genErrorMsg(name)),
	mSrcFile(filename),
	mSrcLine(line) {
}

Errno::~Errno() throw() {
}
