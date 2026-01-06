# Traffic Simulation - Quick Start Guide

## 📦 What You Have

A complete C++ MPI implementation of your traffic flow simulation research paper, including:

### Core Files
1. **traffic_simulation.h** - Header file with all class definitions
2. **traffic_simulation.cpp** - Main implementation with traffic logic
3. **main.cpp** - Entry point for the simulation
4. **mpi_examples.cpp** - Educational examples of MPI operations

### Build & Test
5. **Makefile** - Automated compilation and execution
6. **test_build.sh** - Automated build and test script

### Documentation
7. **README.md** - Comprehensive documentation
8. **visualize_speedup.py** - Python script to generate performance graphs

## 🚀 Quick Start (3 Steps)

### Step 1: Install MPI (if needed)
```bash
# Ubuntu/Debian
sudo apt-get install libopenmpi-dev openmpi-bin

# macOS
brew install open-mpi

# Check installation
mpirun --version
```

### Step 2: Compile
```bash
cd traffic_simulation
make
```

### Step 3: Run
```bash
# Run with 4 processes
mpirun -np 4 ./traffic_sim

# Or use make
make run
```

## 📊 Generate Performance Graphs

```bash
# Install matplotlib if needed
pip3 install matplotlib numpy

# Generate graphs
python3 visualize_speedup.py
```

This creates 4 graphs:
- `ideal_speedup.png` - Ideal linear speedup (Figure 4 from paper)
- `observed_speedup.png` - Actual MPI speedup (Figure 5 from paper)
- `speedup_comparison.png` - Comparison of all models
- `parallel_efficiency.png` - Efficiency analysis

## 🎓 Learn MPI Operations

Compile and run the examples:
```bash
# Compile examples
mpic++ -std=c++11 -o mpi_demo mpi_examples.cpp

# Run with 4 processes
mpirun -np 4 ./mpi_demo
```

This demonstrates:
- **MPI_Bcast**: Broadcasting emergency signals
- **MPI_Gather**: Collecting statistics from all processes
- **MPI_Scatter**: Distributing work to processes
- **MPI_Reduce**: Finding maximum/sum across processes
- **MPI_Win**: Shared memory operations
- **MPI_Barrier**: Process synchronization

## 🎯 Key Features Implemented

### From Your Research Paper

✅ **Parallel Processing** (Section III.A.1)
   - Task decomposition into subtasks
   - Speedup calculation with Amdahl's Law
   - Multiple processes working simultaneously

✅ **MPI Communication** (Section III.A.2)
   - MPI_Bcast for emergency broadcasts
   - MPI_Gather/Scatter for data distribution
   - MPI_Wait for synchronization (traffic lights)
   - MPI_Win_lock for atomic operations

✅ **Shared Memory** (Section II.C)
   - Distributed system with shared memory
   - Race condition prevention
   - Deadlock detection and resolution

✅ **Traffic Components**
   - Cars and Walkers (different speeds)
   - Ambulance Priority (emergency handling)
   - Train Crossings (periodic blockages)
   - Traffic Light Management

✅ **Performance Metrics** (Section III.B)
   - Real-time speedup calculation
   - Amdahl's Law validation
   - Efficiency measurement

## 📈 Expected Results

When running with different process counts:

| Processes | Ideal Speedup | Amdahl (80%) | Expected Actual |
|-----------|---------------|--------------|-----------------|
| 1         | 1.00x         | 1.00x        | 1.00x           |
| 2         | 2.00x         | 1.67x        | 1.85x           |
| 4         | 4.00x         | 2.50x        | 3.45x           |
| 8         | 8.00x         | 3.64x        | 6.20x           |
| 16        | 16.00x        | 4.71x        | 9.80x           |

## 🔧 Common Commands

```bash
# Different process counts
mpirun -np 2 ./traffic_sim   # 2 processes
mpirun -np 4 ./traffic_sim   # 4 processes
mpirun -np 8 ./traffic_sim   # 8 processes

# Custom iterations and segments
mpirun -np 4 ./traffic_sim 200 32
#                          ↑   ↑
#                          |   └─ Number of road segments
#                          └───── Number of iterations

# Using Makefile shortcuts
make run      # 4 processes
make run8     # 8 processes
make run16    # 16 processes

# Custom with make
make run-custom NP=8 ITER=500 SEG=64

# Clean and rebuild
make clean
make rebuild
```

## 🎨 Example Output

```
╔════════════════════════════════════════════════╗
║  MPI Traffic Flow Simulation                  ║
║  With Ambulance Priority & Train Crossings    ║
╚════════════════════════════════════════════════╝

=== Traffic Simulation Initialized ===
Number of processes: 4
Number of road segments: 16
Segments per process: 4
=====================================

>>> EMERGENCY: Ambulance #1005 detected! Clearing path...
Train crossing at segment 5
Deadlock detected at segment 12, forcing green light
Iteration 10 completed
Iteration 20 completed
...

=== Simulation Complete ===
Total execution time: 2.345 seconds
Emergency vehicles handled: 8

=== Performance Metrics ===
Number of processes: 4
Execution time: 2.345 seconds
Theoretical speedup (Amdahl's Law): 2.86x
Estimated actual speedup: 2.34x
==========================
```

## 🐛 Troubleshooting

**"MPI not found"**
```bash
# Check if installed
which mpirun
which mpic++

# Install if missing (Ubuntu)
sudo apt-get install libopenmpi-dev openmpi-bin
```

**Compilation errors**
```bash
# Ensure C++11 support
g++ --version  # Should be 4.8 or newer

# Clean and rebuild
make clean
make
```

**Runtime errors**
```bash
# Check MPI is working
mpirun -np 2 hostname

# Reduce processes if system limited
mpirun -np 2 ./traffic_sim  # Use fewer processes
```

## 📚 Code Structure

```
traffic_simulation/
├── traffic_simulation.h     → Class definitions
├── traffic_simulation.cpp   → Implementation
├── main.cpp                 → Entry point
├── mpi_examples.cpp         → Educational examples
├── Makefile                 → Build automation
├── README.md                → Full documentation
├── visualize_speedup.py     → Graph generator
└── test_build.sh            → Build tester
```

## 🎓 Research Questions Addressed

### RQ1: CPU/GPU Impact
- Demonstrates CPU-based parallelism
- Measures performance scaling
- Framework ready for GPU extension

### RQ2: Task Splitting
- Road segments divided among processes
- MPI communication for coordination
- Efficient data distribution

### RQ3: Real-world Application
- Emergency vehicle handling
- Train crossing simulation
- Deadlock prevention

### RQ4: Distributed System Efficiency
- MPI provides significant speedup
- Shared memory coordination
- Scalable to many processes

## 💡 Next Steps

1. **Run the simulation** with different parameters
2. **Generate graphs** for your paper
3. **Modify parameters** in the code to match your specific scenarios
4. **Add features** like more vehicle types or complex intersections
5. **Benchmark** on different hardware configurations

## 📞 Support

- See `README.md` for detailed documentation
- Check `mpi_examples.cpp` for MPI operation examples
- Review your research paper for theoretical background

---

**Prepared for:** Group 1 - Software Engineering
**Course:** Multicore Programming
**Institution:** University of Europe for Applied Sciences, Potsdam

Good luck with your research! 🚗🚑🚶‍♂️🚆
