# MPI Traffic Flow Simulation

## Overview
This is a distributed traffic flow simulation system implementing parallel programming concepts using MPI (Message Passing Interface). The system simulates realistic traffic scenarios including:

- **Cars and Walkers**: Regular traffic participants
- **Ambulance Priority**: Emergency vehicles get priority routing
- **Train Crossings**: Periodic train crossings that block traffic
- **Deadlock Prevention**: Automatic detection and resolution of traffic deadlocks
- **Shared Memory Management**: Coordinated resource sharing between processes

## Features

### 1. Parallel Processing
- Uses MPI for distributed computation across multiple processes
- Each process manages a subset of road segments
- Efficient inter-process communication for coordination

### 2. Emergency Handling
- Ambulances automatically get priority
- Emergency broadcast to all processes
- Dynamic path clearing for emergency vehicles

### 3. Traffic Management
- Dynamic traffic light control
- Capacity monitoring for each segment
- Prevention of race conditions and deadlocks

### 4. Performance Metrics
- Real-time speedup calculations
- Amdahl's Law theoretical predictions
- Execution time tracking

## System Architecture

```
Master Process (Rank 0)
├── Manages overall coordination
├── Broadcasts emergency signals
└── Collects statistics

Worker Processes (Rank 1..N)
├── Manage assigned road segments
├── Process vehicles in segments
├── Update shared memory
└── Synchronize with other processes
```

## Prerequisites

### Required Software
- **MPI Implementation**: OpenMPI or MPICH
- **C++ Compiler**: g++ with C++11 support
- **Make**: Build automation tool

### Installation on Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libopenmpi-dev openmpi-bin
```

### Installation on macOS
```bash
brew install open-mpi
```

## Compilation

### Using Make
```bash
# Compile the program
make

# Clean build artifacts
make clean

# Clean and rebuild
make rebuild
```

### Manual Compilation
```bash
mpic++ -std=c++11 -Wall -O2 -c main.cpp
mpic++ -std=c++11 -Wall -O2 -c traffic_simulation.cpp
mpic++ -std=c++11 -Wall -O2 -o traffic_sim main.o traffic_simulation.o
```

## Usage

### Basic Execution
```bash
# Run with 4 processes (default)
mpirun -np 4 ./traffic_sim

# Run with 8 processes
mpirun -np 8 ./traffic_sim

# Run with custom iteration count
mpirun -np 4 ./traffic_sim 200

# Run with custom iterations and segments
mpirun -np 4 ./traffic_sim 200 32
```

### Using Make Targets
```bash
# Run with 4 processes
make run

# Run with 8 processes
make run8

# Run with 16 processes
make run16

# Run with custom parameters
make run-custom NP=4 ITER=200 SEG=32
```

## Command Line Arguments

```
./traffic_sim [iterations] [num_segments]
```

- **iterations**: Number of simulation iterations (default: 100)
- **num_segments**: Total number of road segments (default: 16)

## Example Output

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

## Key Components

### 1. TrafficController Class
Main controller managing the simulation:
- `initialize()`: Sets up shared memory and MPI communication
- `run_simulation()`: Main simulation loop
- `process_vehicles()`: Moves vehicles through segments
- `handle_emergency()`: Manages ambulance priority
- `manage_traffic_lights()`: Controls traffic flow
- `check_for_deadlock()`: Detects and resolves deadlocks

### 2. Vehicle Types
- **CAR**: Standard vehicle (speed: 1.0, priority: 1)
- **WALKER**: Pedestrian (speed: 0.5, priority: 1)
- **AMBULANCE**: Emergency vehicle (speed: 1.0, priority: 100)
- **TRAIN**: Train at crossings (speed: 2.0, priority: 1)

### 3. Traffic States
- **RED**: No vehicles allowed
- **GREEN**: Vehicles can proceed
- **YELLOW**: Transition state
- **EMERGENCY**: Priority for ambulances

## Performance Considerations

### Speedup Formula (Amdahl's Law)
```
S(p) = 1 / ((1 - f) + f/p)
```
Where:
- S(p) = Speedup with p processors
- f = Fraction of code that can be parallelized
- p = Number of processors

### Scaling Recommendations
- **Optimal processes**: 4-16 for best efficiency
- **Segments per process**: At least 2-4 for load balance
- **Total segments**: Should be divisible by number of processes

## MPI Functions Used

### Communication
- `MPI_Init()`: Initialize MPI environment
- `MPI_Finalize()`: Clean up MPI
- `MPI_Bcast()`: Broadcast emergency signals
- `MPI_Reduce()`: Gather statistics
- `MPI_Barrier()`: Synchronize processes

### Shared Memory
- `MPI_Win_allocate()`: Allocate shared memory window
- `MPI_Win_lock()`: Exclusive access for atomic operations
- `MPI_Win_unlock()`: Release lock
- `MPI_Win_fence()`: Synchronize memory access
- `MPI_Put()`: Write to shared memory

## Deadlock Prevention

The system prevents deadlocks through:
1. **Wait time monitoring**: Tracks how long vehicles wait
2. **Automatic green light**: Forces green after timeout
3. **Emergency override**: Ambulances bypass normal rules
4. **Capacity checks**: Prevents segment overflow

## Race Condition Prevention

Protected through:
1. **MPI_Win_lock**: Exclusive access to shared memory
2. **Atomic operations**: MPI_Put with locking
3. **Barrier synchronization**: Coordinated state updates
4. **Process-specific segments**: Reduced contention

## Troubleshooting

### Common Issues

**Error: "MPI not found"**
```bash
# Install OpenMPI
sudo apt-get install libopenmpi-dev openmpi-bin
```

**Error: "mpic++ command not found"**
```bash
# Add MPI to PATH
export PATH=/usr/lib/openmpi/bin:$PATH
```

**Deadlock in simulation**
- Increase timeout threshold in `check_for_deadlock()`
- Reduce number of vehicles per segment
- Adjust traffic light timing

## Future Enhancements

- [ ] GPU acceleration for vehicle processing
- [ ] Machine learning for traffic prediction
- [ ] Visualization of traffic flow
- [ ] Weather conditions impact
- [ ] Multi-lane support
- [ ] Intersection management
- [ ] Real-time traffic data integration

## Research Questions Addressed

### RQ1: CPU/GPU Impact
- Demonstrates CPU-based parallel processing
- Framework ready for GPU extension

### RQ2: Task Splitting
- Segments divided among processes
- Data distribution via MPI communication

### RQ3: Real-world Application
- Models realistic traffic scenarios
- Emergency handling mimics actual systems

### RQ4: Distributed System Efficiency
- MPI provides significant speedup
- Scales well with process count

## Authors
Group 1 - Software Engineering
- Klaus Caka
- Sorasith Chormalee
- Reema Sara Stephen
- Nguyen Phuoc Pham
- Mugdha Kashyap

University of Europe for Applied Sciences, Potsdam, Germany

## License
This code is provided for educational and research purposes.

## References
See the research paper for detailed methodology and results.
