CXX = g++
ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`

DEPS= ./include/ToyModel.h ./include/RootClasses.h
OBJ= ToyModel.so ./lib/ToyModel.so ./lib/RootClasses.so

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ROOTCFLAGS) $(ROOTLIBS)

ToyModel: $(OBJ)
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

clean:
	rm *.pcm *.so *.d
