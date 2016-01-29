
#ToyModel: 
#	g++ -Wall -I$(ROOTSYS)/include -L$(ROOTSYS)/lib ToyModel.C -lCore -lHist -lCint -lRIO -lTree -lGpad ToyModel.o -o ToyModel


#ToyModel: ToyModel.C
#	g++ -L$(ROOTSYS)/lib -I$(ROOTSYS)/include -Wall -c ToyModel.C -o ./lib



#CC=g++
#CFLAGS=-c -g -Wall `root-config --cflags`
#LDFLAGS=`root-config --glibs`
#SOURCES=ToyModel.C
#OBJECTS=$(SOURCES:.cc=.o)
#EXECUTABLE=main
##
#all: $(SOURCES) $(EXECUTABLE)
#
#$(EXECUTABLE): $(OBJECTS)
#	   $(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o ToyModel
#
#.cc.o:
#	   $(CC) $(CFLAGS) $< -o $@

clean:
	   rm *.pcm *.so *.d


