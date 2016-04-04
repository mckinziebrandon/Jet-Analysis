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
OBJ1= JetFinderTester.so $(LIB)/EventModel.so $(LIB)/EventFunctions.so $(LIB)/MyJetFinder.so

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS)

JetFinderTester: $(OBJ1) 
	reset && echo "Beginning compilation of $@ . . . "
	$(CXX) -o $@ $(OBJ1) $(FJETFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)
	./$@


