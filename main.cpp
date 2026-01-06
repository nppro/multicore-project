#include "traffic_simulation.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Simulation parameters
    int num_segments = 16;  // Total road segments
    int iterations = 100;   // Number of simulation iterations
    
    // Parse command line arguments if provided
    if (argc > 1) {
        iterations = std::atoi(argv[1]);
    }
    if (argc > 2) {
        num_segments = std::atoi(argv[2]);
    }
    
    if (rank == 0) {
        std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  MPI Traffic Flow Simulation                  ║" << std::endl;
        std::cout << "║  With Ambulance Priority & Train Crossings    ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════╝\n" << std::endl;
    }
    
    // Create traffic controller
    TrafficController controller(rank, size, num_segments);
    
    // Initialize simulation
    controller.initialize();
    
    // Record start time for performance measurement
    double start_time = MPI_Wtime();
    
    // Run simulation
    controller.run_simulation(iterations);
    
    // Record end time
    double end_time = MPI_Wtime();
    double execution_time = end_time - start_time;
    
    // Gather execution times from all processes
    double max_time = 0.0;
    MPI_Reduce(&execution_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "\n=== Performance Metrics ===" << std::endl;
        std::cout << "Number of processes: " << size << std::endl;
        std::cout << "Execution time: " << max_time << " seconds" << std::endl;
        
        // Calculate theoretical speedup (Amdahl's Law)
        // Assuming 80% of code is parallelizable
        double f = 0.80;  // Parallelizable fraction
        double theoretical_speedup = 1.0 / ((1.0 - f) + (f / size));
        std::cout << "Theoretical speedup (Amdahl's Law): " << theoretical_speedup << "x" << std::endl;
        
        // Estimate actual speedup (comparing to serial baseline)
        double estimated_serial_time = max_time * size * 0.6; // Rough estimate
        double actual_speedup = estimated_serial_time / max_time;
        std::cout << "Estimated actual speedup: " << actual_speedup << "x" << std::endl;
        
        std::cout << "==========================" << std::endl;
    }
    
    // Finalize MPI
    MPI_Finalize();
    
    return 0;
}
