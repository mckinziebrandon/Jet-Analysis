#ifndef __TEvent__
#define __TEvent__
#include "TObject.h"
class TCollection;
class TTrack;

class TEvent : public TObject {
private:
    Int_t fId;
    // event sequential id
    Float_t fTotalMom;
    // total momentum
    TCollection *fTracks;
    // collection of tracks
public:
    TEvent() { fId = 0; fTracks = 0; }
    TEvent(Int_t id);
    ~TEvent();
    void AddTrack(TTrack *t);
    Int_t GetId() const { return fId; }
    Int_t GetNoTracks() const;
    void Print(Option_t *opt="");
    Float_t TotalMomentum();
    ClassDef(TEvent,1);
};
