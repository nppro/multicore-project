# Makefile for MPI Traffic Simulation

CXX = mpic++
CXXFLAGS = -std=c++11 -Wall -O2
TARGET = traffic_sim
SOURCES = main.cpp traffic_simulation.cpp
HEADERS = traffic_simulation.h
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run with default parameters (4 processes)
run: $(TARGET)
	mpirun -np 4 ./$(TARGET)

# Run with 8 processes
run8: $(TARGET)
	mpirun -np 8 ./$(TARGET)

# Run with 16 processes
run16: $(TARGET)
	mpirun -np 16 ./$(TARGET)

# Run with custom parameters: make run-custom NP=4 ITER=200 SEG=32
run-custom: $(TARGET)
	mpirun -np $(NP) ./$(TARGET) $(ITER) $(SEG)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Clean and rebuild
rebuild: clean all

.PHONY: all run run8 run16 run-custom clean rebuild
