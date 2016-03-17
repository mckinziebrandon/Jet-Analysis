#include <iostream>

class Printer {
public:
	/* Print function that outputs if debug flag = true. */
	static Bool_t debug;
	static void print(const char* str, Int_t i_event, Int_t nEvents);
	static void print(const char* str);
};

void Printer::print(const char* str, Int_t i_event, Int_t nEvents) {
	Bool_t fivePercentInterval = (i_event % (nEvents / 20) == 0);
	if (debug && fivePercentInterval) {
		std::cout << str << std::endl;
	}
}

void Printer::print(const char* str) {
	if (debug) {
		std::cout << str << std::endl;
	}
}


