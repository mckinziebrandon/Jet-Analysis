CXX = g++
ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`

DEPS= ./include/EventModel.h ./include/RootClasses.h EventModelDict.C
OBJ1= ToyModel.so EventModel.o ./lib/RootClasses.so EventModelDict.o
OBJ2= PlotModel.so ./lib/EventModel.so ./lib/RootClasses.so 

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ROOTCFLAGS) $(ROOTLIBS)

ToyModel: $(OBJ1)
	@$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

EventModel.o: ./include/EventModel.h

EventModelDict.C: ./include/EventModel.h
	@echo "Generating dictionary . . . "
	@rootcint EventModelDict.C -c ./include/EventModel.h



PlotModel: $(OBJ2)
	echo "Beginning compilation . . . "
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)
	echo "Running program . . . "
	./PlotModel
	echo "Opening ROOT . . . "
	root -l -L ./rootFiles/PlotModel.root

clean:
	rm *.pcm *.so *.d
