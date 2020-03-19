run: main
	./main

CXXFLAGS += -g

SOURCES= main.cc IFObject.cc Pragma.cc \
	Module.cc  Use.cc \
	Info.cc BasicType.cc ChainType.cc FunctionType.cc \
	Stamp.cc \
	Function.cc Graph.cc Node.cc Compound.cc \
	Edge.cc Literal.cc EdgeCluster.cc \
	CompoundGraph.cc

OBJECTS=${patsubst %.cc,%.o,$(SOURCES)}

include ${patsubst %,depends/%,$(SOURCES)}

depends/%: %
ifeq ("${wildcard depends}","")
	mkdir depends
endif
	$(CXX) -M $<  > $@


main: $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)

