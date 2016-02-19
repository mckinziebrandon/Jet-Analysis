#ifndef __TTrack__
#define __TTrack__
#include "TObject.h"
class TEvent;
class TTrack : public TObject {
private:
    Int_t fId;      //track sequential id
    TEvent *fEvent; //event to which track belongs
    Float_t fPx;    //x part of track momentum
    Float_t fPy;    //y part of track momentum
    Float_t fPz;    //z part of track momentum
public:
    TTrack() { fId = 0; fEvent = 0; fPx = fPy = fPz = 0; }
    TTrack(Int_t id, Event *ev, Float_t px,Float_t py,Float_t pz);
    Float_t Momentum() const;
    TEvent *GetEvent() const { return fEvent; }
    void Print(Option_t *opt="");
    ClassDef(TTrack, 1);
};

#endif
