#include "./include/RootClasses.h"
#include <iostream>
using namespace std;


void temp() {
    for (int i = 1; i < numBins + 1; i++) {
        cout << "i: " << i 
             << "\tdelta: " << xBins[i] - xBins[i-1] << endl;
    }
}
