#include <iostream>
#include "TRandom.h"
using namespace std;

void hello()
{
    TRandom* rnd = new TRandom(time(0));
    Double_t x = rnd->Rndm();
    cout << "hello world!" << endl;
}

#ifndef __CINT__
int main()
{
    hello();
    return 0;
}
#endif
