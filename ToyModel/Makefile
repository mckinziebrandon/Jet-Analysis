CXX = g++

# Libraries to include if GZIP support is enabled
ifeq (x$(ENABLEGZIP),xyes)
LIBGZIP=-L$(BOOSTLIBLOCATION) -lboost_iostreams -L$(ZLIBLOCATION) -lz
endif

# Local directory definitions.
BASEDIR =/home/student/jet-radius-analysis/ToyModel
INCLUDE =$(BASEDIR)/include
LIB 	=$(BASEDIR)/lib

# All ALICE-related variables.
APHYS 	 =/home/student/alice/aliphysics/master/inst
AROOT 	 =/home/student/alice/aliroot/master/inst
AROOT2 	 =/home/student/alice/root/alice_v5-34-30/build
APHYSINC:=-I$(APHYS)/include
AROOTINC:=-I$(AROOT)/include -I$(AROOT2)/include
ROOTINC :=-I$(ROOTSYS)/include
APHYSLIB:=-L$(APHYS)/lib
AROOTLIB:=-L$(AROOT)/lib -L$(AROOT2)/net/alien/src
ALI 	 =$(APHYSINC) $(AROOTINC) $(ROOTINC)


AROOT2 = $(AROOT2)
# Pythia, ROOT, Fastjet variables.
DICTCXXFLAGS:= -I$(PYTHIA8)/include
ROOTCFLAGS 	 = $(DICTCXXFLAGS) `root-config --cflags`
ROOTLIBS 	 = `root-config --libs`
STATICLIB 	:=$(PYTHIA8)/lib/archive/libpythia8.a
ROOTGLIBS    = `root-config --ldflags --glibs` \
			  -L$(PYTHIA8)/lib/archive -lpythia8 -llhapdfdummy $(LIBGZIP)
ROOTCLING 	 = `rootcling`
FJETFLAGS 	 = `fastjet-config --cxxflags --libs --plugins`

# Header and object files. 
DEPS 	= $(INCLUDE)/EventGenerator.h $(INCLUDE)/RootClasses.h  $(INCLUDE)/EventFunctions.h $(INCLUDE)/MyJetFinder.h $(INCLUDE)/Printer.h
LIBFILES= $(LIB)/EventGenerator.so  $(LIB)/EventFunctions.so $(LIB)/MyJetFinder.so $(LIB)/Printer.so  $(LIB)/RootClasses.so

# Object files needed by each target. 
OBJ1= $(BASEDIR)/ToyModel.so $(LIBFILES) $(STATICLIB)
OBJ2= PlotModel.so 	$(LIBFILES) $(STATICLIB)
OBJ3= $(BASEDIR)/PlotFinder.so $(LIB)/RootClasses.so
OBJ4= PythiaEvent.so $(LIBFILES) $(STATICLIB)
OBJ5= PlotData.so  	$(LIBFILES) $(STATICLIB) 

DICT=EventModelDict.cxx
LINK=LinkDef.h

# --------------------------------------------------------------------------
%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(APHYSLIB) $(AROOTLIB) $(ALI) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS)

$(DICT): 
	rootcint -f $(DICT) -c $(DEPS) $(LINK)

ToyModel: $(OBJ1) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $^ $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ2) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotFinder: $(OBJ3)
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $^ $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -Wl,--no-as-needed

PythiaEvent: $(OBJ4) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(ROOTCFLAGS) $(OBJ4) $(FJETFLAGS)  $(ROOTLIBS) $(ROOTGLIBS)

PlotData: $(OBJ5) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(APHYSLIB) $(AROOTLIB) $(ALI) $(OBJ5) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) 

clean:
	rm *.so $(LIB)/*.so
# --------------------------------------------------------------------------
