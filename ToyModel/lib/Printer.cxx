#include "../include/Printer.h"

/* Printer McPrintyFace over here */

bool Printer::debug;

void Printer::print(const char* str, int i_event, const int nEvents) {
	bool fivePercentInterval = (i_event % (nEvents / 20) == 0);
	if (fivePercentInterval) {
		std::cout << str << i_event * 100.0 / nEvents <<  std::endl;
	}
}

void Printer::print(const char* str) {
	if (debug) {
		std::cout << str << std::endl;
	}
}

