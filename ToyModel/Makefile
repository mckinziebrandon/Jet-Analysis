CXX = g++
ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`

DEPS= ./include/EventModel.h ./include/RootClasses.h
OBJ1= ToyModel.so ./lib/EventModel.so ./lib/RootClasses.so
OBJ2= PlotModel.so ./lib/EventModel.so ./lib/RootClasses.so 

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ROOTCFLAGS) $(ROOTLIBS)

ToyModel: $(OBJ1)
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

clean:
	rm *.pcm *.so *.d
