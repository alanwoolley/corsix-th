#include "logging.h"
#include <stdio.h>

FILE* fo;
FILE* fe;

void LOG_ERROR(std::string strtext) {

	if (strtext.length() > 0) {
		if (strtext.at((strtext.length() - 1)) != '\n') {
			strtext.resize(strtext.length() + 1, '\n');
		}
	}

	char *cstr = (char*) strtext.c_str();
	LOGW(cstr);
	fprintf(stderr, cstr);
}

void LOG_INFO(std::string strtext) {

	if (strtext.length() > 0) {
		if (strtext.at((strtext.length() - 1)) != '\n') {
			strtext.resize(strtext.length() + 1, '\n');
		}
	}

	char *cstr = (char*) strtext.c_str();
	LOGI(cstr);
	fprintf(stdout, cstr);
}

void START_LOGGING(const char* root) {
	std::string logfile = std::string(root) + "/cthlog.txt";
	std::string errlogfile = std::string(root) + "/ctherrlog.txt";
	// Redirect stdout and stderr
	fo = freopen(logfile.c_str(), "w", stdout);
	fe = freopen(errlogfile.c_str(), "w", stderr);

	// Disable buffering so messages are written instantly
	setvbuf(fo, 0, _IONBF, 0);
	setvbuf(fe, 0, _IONBF, 0);

	LOG_INFO(root);
}
