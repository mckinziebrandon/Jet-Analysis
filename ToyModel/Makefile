
SOURCES=ToyModel.C ./lib/ToyModel.cxx
EXECUTABLE=ToyModel

CXX = g++ -W -Wall -Wextra -pedantic -std=c++11
ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(root-config --glibs)
ROOTLIBS     += -lEG

INCFLAGS = -I$(ROOTSYS)/include
LDFLAGS = -L$(ROOTSYS)/lib

FLAGS = -Wall -g $(INCFLAGS) $(LDFLAGS) -fPIC

COMPILE = $(CXX) $(FLAGS) -c

EXE=ToyModel



ToyModel: 
	$(CXX) -o $(EXE) $(ROOTCFLAGS) $(ROOTLIBS) -m64 -lCore -lHist -lRIO -lTree -lGpad ToyModel.C ./lib/ToyModel.cxx
#	g++ -Wall -c -I$(ROOTSYS)/include/  -L$(ROOTSYS)/lib/ ToyModel.C -lCore -lHist -lCint -lRIO -lTree -lGpad -o ToyModel

#ToyModel: ToyModel.C
#	g++ -L$(ROOTSYS)/lib -I$(ROOTSYS)/include -Wall -c ToyModel.C -o ./lib



clean:
	rm *.pcm *.so *.d
