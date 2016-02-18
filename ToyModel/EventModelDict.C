// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME EventModelDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "./include/EventModel.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_EventModel(void *p = 0);
   static void *newArray_EventModel(Long_t size, void *p);
   static void delete_EventModel(void *p);
   static void deleteArray_EventModel(void *p);
   static void destruct_EventModel(void *p);
   static void streamer_EventModel(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventModel*)
   {
      ::EventModel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::EventModel >(0);
      static ::ROOT::TGenericClassInfo 
         instance("EventModel", ::EventModel::Class_Version(), "include/EventModel.h", 20,
                  typeid(::EventModel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::EventModel::Dictionary, isa_proxy, 16,
                  sizeof(::EventModel) );
      instance.SetNew(&new_EventModel);
      instance.SetNewArray(&newArray_EventModel);
      instance.SetDelete(&delete_EventModel);
      instance.SetDeleteArray(&deleteArray_EventModel);
      instance.SetDestructor(&destruct_EventModel);
      instance.SetStreamerFunc(&streamer_EventModel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventModel*)
   {
      return GenerateInitInstanceLocal((::EventModel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::EventModel*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr EventModel::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *EventModel::Class_Name()
{
   return "EventModel";
}

//______________________________________________________________________________
const char *EventModel::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventModel*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int EventModel::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventModel*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *EventModel::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventModel*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *EventModel::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventModel*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void EventModel::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventModel.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> has_bkg;
      R__b >> has_V2;
      R__b >> fPolynomial;
      R__b >> fLine;
      R__b >> fdNdPhi;
      R__b >> fTrackSpectrum;
      R__b >> rand;
      R__b.CheckByteCount(R__s, R__c, EventModel::IsA());
   } else {
      R__c = R__b.WriteVersion(EventModel::IsA(), kTRUE);
      TNamed::Streamer(R__b);
      R__b << has_bkg;
      R__b << has_V2;
      R__b << fPolynomial;
      R__b << fLine;
      R__b << fdNdPhi;
      R__b << fTrackSpectrum;
      R__b << rand;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_EventModel(void *p) {
      return  p ? new(p) ::EventModel : new ::EventModel;
   }
   static void *newArray_EventModel(Long_t nElements, void *p) {
      return p ? new(p) ::EventModel[nElements] : new ::EventModel[nElements];
   }
   // Wrapper around operator delete
   static void delete_EventModel(void *p) {
      delete ((::EventModel*)p);
   }
   static void deleteArray_EventModel(void *p) {
      delete [] ((::EventModel*)p);
   }
   static void destruct_EventModel(void *p) {
      typedef ::EventModel current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_EventModel(TBuffer &buf, void *obj) {
      ((::EventModel*)obj)->::EventModel::Streamer(buf);
   }
} // end of namespace ROOT for class ::EventModel

namespace {
  void TriggerDictionaryInitialization_EventModelDict_Impl() {
    static const char* headers[] = {
"./include/EventModel.h",
0
    };
    static const char* includePaths[] = {
"/home/student/root/include",
"/home/student/jet-radius-analysis/ToyModel/",
0
    };
    static const char* fwdDeclCode = 
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$./include/EventModel.h")))  EventModel;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "./include/EventModel.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"EventModel", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("EventModelDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_EventModelDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_EventModelDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_EventModelDict() {
  TriggerDictionaryInitialization_EventModelDict_Impl();
}
