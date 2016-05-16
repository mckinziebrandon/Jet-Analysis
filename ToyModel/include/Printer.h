#ifndef PRINTER_H
#define PRINTER_H
#include <iostream>

class Printer {
    public:
	/* Print function that outputs if debug flag = true. */
        static bool debug;
        static void setDebug(bool d) { debug = d;}
        static void print(const char* str, int i_event, const int nEvents);
        static void print(const char* str);
        template<typename T>
        static void print(const char* str, T val);
        template<typename T>
        static void print(const char* str, T effVars[]);
};


template<typename T>
void Printer::print(const char* str, T val) {
	if (debug) {
		std::cout << str << val << std::endl;
	}
}


template<typename T> 
void Printer::print(const char* str, T effVars[]) {
    if (debug) {
		std::cout << str 
            << "(" << effVars[0] 
            << ", " << effVars[1] 
            << ", " << effVars[2] 
            << ", " << effVars[3] 
            << ")" << std::endl;
    }
}

#endif


