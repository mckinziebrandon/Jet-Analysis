CXX = g++

# Local directory definitions.
BASEDIR =/home/student/jet-radius-analysis/ToyModel
INCLUDE =$(BASEDIR)/include
LIB 	=$(BASEDIR)/lib

APHYS=/home/student/alice/aliphysics/master/inst
AROOT=/home/student/alice/aliroot/master/inst
APHYSINC:=-I$(APHYS)/include
AROOTINC:=-I$(AROOT)/include
ROOTINC:=-I$(ROOTSYS)/include
APHYSLIB:=-L$(APHYS)/lib
AROOTLIB:=-L$(AROOT)/lib

DICTCXXFLAGS:= -I$(PYTHIA8)/include
ROOTCFLAGS 	= $(DICTCXXFLAGS) `root-config --cflags`
ROOTLIBS 	= `root-config --libs`

# Libraries to include if GZIP support is enabled
ifeq (x$(ENABLEGZIP),xyes)
LIBGZIP=-L$(BOOSTLIBLOCATION) -lboost_iostreams -L$(ZLIBLOCATION) -lz
endif

STATICLIB:=$(PYTHIA8)/lib/archive/libpythia8.a
ROOTGLIBS   = `root-config --ldflags --glibs` \
			  -L$(PYTHIA8)/lib/archive -lpythia8 -llhapdfdummy $(LIBGZIP)
ROOTCLING 	= `rootcling`
FJETFLAGS 	= `fastjet-config --cxxflags --libs --plugins`

ALI=$(APHYSINC) $(AROOTINC) $(ROOTINC)

DEPS= $(INCLUDE)/EventGenerator.h $(INCLUDE)/RootClasses.h  $(INCLUDE)/EventFunctions.h $(INCLUDE)/MyJetFinder.h
LIBFILES= $(LIB)/EventGenerator.so  $(LIB)/EventFunctions.so $(LIB)/MyJetFinder.so

OBJ1= ToyModel.so $(LIBFILES) $(STATICLIB)
OBJ2= PlotModel.so $(LIBFILES) $(STATICLIB)
OBJ3= PlotFinder.so $(LIBFILES) $(STATICLIB)
OBJ4= PythiaEvent.so $(LIBFILES) $(STATICLIB)
OBJ5= PlotData.so  $(LIBFILES) $(STATICLIB) 

#/home/student/alice/aliphysics/master/inst/include/

DICT=EventModelDict.cxx
LINK=LinkDef.h

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ALI) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS)

$(DICT): 
	rootcint -f $(DICT) -c $(DEPS) $(LINK)

ToyModel: $(OBJ1) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ1) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ2) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotFinder: $(OBJ3)
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ3) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PythiaEvent: $(OBJ4) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ4) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotData: $(OBJ5) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(APHYSLIB) $(AROOTLIB) $(ALI) $(OBJ5) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) 


FileWithFastJet:
	 g++ short-example.cc -o short-example `fastjet-config --cxxflags --libs --plugins`

clean:
	rm *.so $(LIB)/*.so
