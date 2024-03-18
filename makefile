# Compiler
CXX = mpic++

# Compiler flags
CXXFLAGS = -g -I. -L/appl/spack/v018/install-tree/gcc-11.3.0/boost-1.79.0-eipty5/lib -lboost_mpi -lboost_serialization -lm

# Include directory
INCLUDES = -I/appl/spack/v018/install-tree/gcc-11.3.0/boost-1.79.0-eipty5/include

# Source files
SOURCES = main.cpp algo/algo_mpi.cpp algo/graph_local.cpp algo/graph.cpp algo/quick_union.cpp algo/prim.cpp algo/kruskal.cpp algo/utils/graph_util.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
