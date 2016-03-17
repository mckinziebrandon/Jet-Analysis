CXX = g++

BASEDIR =/home/student/jet-radius-analysis/ToyModel
INCLUDE =$(BASEDIR)/include
LIB 	=$(BASEDIR)/lib

ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`
ROOTCLING 	= `rootcling`
FJETFLAGS 	= `fastjet-config --cxxflags --libs --plugins`

DEPS= $(INCLUDE)/EventModel.h $(INCLUDE)/RootClasses.h  $(INCLUDE)/EventFunctions.h $(INCLUDE)/MyJetFinder.h
LIBFILES= $(LIB)/EventModel.so $(LIB)/RootClasses.so $(LIB)/EventFunctions.so $(LIB)/MyJetFinder.so

OBJ1= ToyModel.so $(LIBFILES)
OBJ2= PlotModel.so $(LIBFILES)


DICT=EventModelDict.cxx
LINK=LinkDef.h

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS)

$(DICT): 
	rootcint -f $(DICT) -c $(DEPS) $(LINK)

ToyModel: $(OBJ1) 
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ1) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ2) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

FileWithFastJet:
	 g++ short-example.cc -o short-example `fastjet-config --cxxflags --libs --plugins`

clean:
	rm *.pcm *.so *Dict* $(LIB)/*.so
