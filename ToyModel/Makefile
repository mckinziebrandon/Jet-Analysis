CXX = g++

BASEDIR =/home/student/jet-radius-analysis/ToyModel
INCLUDE =$(BASEDIR)/include
LIB 	=$(BASEDIR)/lib

ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`
ROOTCLING 	= `rootcling`

DEPS= $(INCLUDE)/EventModel.h $(INCLUDE)/RootClasses.h  $(INCLUDE)/EventFunctions.h
OBJ1= ToyModel.so $(LIB)/EventModel.so $(LIB)/RootClasses.so $(LIB)/EventFunctions.so
OBJ2= PlotModel.so $(LIB)/EventModel.so $(LIB)/RootClasses.so $(LIB)/EventFunctions.so


DICT=EventModelDict.cxx
LINK=LinkDef.h

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ROOTCFLAGS) $(ROOTLIBS)

$(DICT): 
	rootcint -f $(DICT) -c $(DEPS) $(LINK)

ToyModel: $(OBJ1) 
	$(CXX) -o $@ $(OBJ1) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	echo "Beginning compilation . . . "
	$(CXX) -o $@ $(OBJ2) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)
	echo "Running program . . . "
	./PlotModel
	echo "Opening ROOT . . . "
	root -l -L ./rootFiles/PlotModel.root

clean:
	rm *.pcm *.so *Dict* $(LIB)/*.so
