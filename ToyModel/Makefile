CXX = g++
ROOTCFLAGS 	=`root-config --cflags`
ROOTLIBS 	= `root-config --libs`
ROOTGLIBS   = `root-config --glibs`
ROOTCLING 	= `rootcling`

DEPS= ./include/EventModel.h ./include/RootClasses.h
OBJ1= ToyModel.so ./lib/EventModel.so ./lib/RootClasses.so 
OBJ2= PlotModel.so ./lib/EventModel.so ./lib/RootClasses.so 

%.so: %.cxx $(DEPS)
	$(CXX) -c -o $@ $< $(ROOTCFLAGS) $(ROOTLIBS)

EventModelDict.cxx: ./include/EventModel.h ./lib/LinkDef.h
	rootcint -f $@ -c $(ROOTCFLAGS) -p $^
libMyLib.so: EventModelDict.cxx ./include/EventModel.h ./lib/LinkDef.h
	g++ -fPIC -shared -o$@ `root-config --ldflags` $(ROOTCFLAGS) -I$(ROOTSYS)/include $^

ToyModel: $(OBJ1) libMyLib.so
	rootcint -f ./lib/EventModelDict.cxx -c ./include/EventModel.h ./lib/LinkDef.h
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)

PlotModel: $(OBJ2)
	echo "Beginning compilation . . . "
	$(CXX) -o $@ $^ $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS)
	echo "Running program . . . "
	./PlotModel
	echo "Opening ROOT . . . "
	root -l -L ./rootFiles/PlotModel.root

clean:
	rm *.pcm *.so ./lib/*Dict* ./lib/*.so
