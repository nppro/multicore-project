#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

/**
 * Advanced MPI Example: Demonstrates key MPI operations
 * used in the traffic simulation
 */

void demonstrate_broadcast() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int emergency_signal = 0;
    
    if (rank == 0) {
        std::cout << "\n=== MPI_Bcast Example ===" << std::endl;
        emergency_signal = 1; // Emergency detected!
        std::cout << "Rank 0: Broadcasting emergency signal = " << emergency_signal << std::endl;
    }
    
    // Broadcast from rank 0 to all processes
    MPI_Bcast(&emergency_signal, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        std::cout << "Rank " << rank << ": Received emergency signal = " 
                  << emergency_signal << std::endl;
    }
}

void demonstrate_gather() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Each process has local vehicle count
    int local_vehicles = rank + 1; // Simulated count
    
    if (rank == 0) {
        std::cout << "\n=== MPI_Gather Example ===" << std::endl;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Rank " << rank << ": Local vehicles = " << local_vehicles << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Gather all counts to rank 0
    std::vector<int> all_vehicles;
    if (rank == 0) {
        all_vehicles.resize(size);
    }
    
    MPI_Gather(&local_vehicles, 1, MPI_INT, 
               all_vehicles.data(), 1, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        int total = 0;
        std::cout << "Rank 0: Gathered vehicle counts: ";
        for (int i = 0; i < size; i++) {
            std::cout << all_vehicles[i] << " ";
            total += all_vehicles[i];
        }
        std::cout << "\nTotal vehicles across all processes: " << total << std::endl;
    }
}

void demonstrate_scatter() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    std::vector<int> segments;
    int local_segment;
    
    if (rank == 0) {
        std::cout << "\n=== MPI_Scatter Example ===" << std::endl;
        // Rank 0 has all segment IDs to distribute
        segments.resize(size);
        for (int i = 0; i < size; i++) {
            segments[i] = i * 10; // Segment IDs: 0, 10, 20, 30...
        }
        std::cout << "Rank 0: Distributing segments: ";
        for (int seg : segments) {
            std::cout << seg << " ";
        }
        std::cout << std::endl;
    }
    
    // Scatter segments to all processes
    MPI_Scatter(segments.data(), 1, MPI_INT,
                &local_segment, 1, MPI_INT,
                0, MPI_COMM_WORLD);
    
    std::cout << "Rank " << rank << ": Received segment ID = " 
              << local_segment << std::endl;
}

void demonstrate_reduce() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Each process has some execution time
    double local_time = (rank + 1) * 0.5; // Simulated time
    
    if (rank == 0) {
        std::cout << "\n=== MPI_Reduce Example ===" << std::endl;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Rank " << rank << ": Local execution time = " 
              << local_time << " seconds" << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Find maximum time (slowest process)
    double max_time = 0.0;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, 
               MPI_MAX, 0, MPI_COMM_WORLD);
    
    // Calculate total time (sum)
    double total_time = 0.0;
    MPI_Reduce(&local_time, &total_time, 1, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "Maximum execution time: " << max_time << " seconds" << std::endl;
        std::cout << "Total execution time: " << total_time << " seconds" << std::endl;
        std::cout << "Average execution time: " << (total_time / size) << " seconds" << std::endl;
    }
}

void demonstrate_shared_memory() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        std::cout << "\n=== Shared Memory (MPI_Win) Example ===" << std::endl;
    }
    
    // Allocate shared memory window
    MPI_Win window;
    int* shared_data;
    
    MPI_Win_allocate(size * sizeof(int), sizeof(int), MPI_INFO_NULL,
                     MPI_COMM_WORLD, &shared_data, &window);
    
    // Initialize shared memory
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            shared_data[i] = 0;
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Each process writes to its slot using RMA (Remote Memory Access)
    int value = rank * 100; // Value to write
    
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, window);
    MPI_Put(&value, 1, MPI_INT, 0, rank, 1, MPI_INT, window);
    MPI_Win_unlock(0, window);
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Rank 0 reads and displays all values
    if (rank == 0) {
        std::cout << "Shared memory contents after all writes:" << std::endl;
        for (int i = 0; i < size; i++) {
            std::cout << "  Slot " << i << ": " << shared_data[i] << std::endl;
        }
    }
    
    MPI_Win_free(&window);
}

void demonstrate_barrier() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0) {
        std::cout << "\n=== MPI_Barrier Example ===" << std::endl;
        std::cout << "All processes will synchronize at barrier..." << std::endl;
    }
    
    // Simulate different work times
    std::cout << "Rank " << rank << ": Starting work..." << std::endl;
    
    // Wait for random time (simulating different workloads)
    for (int i = 0; i < (rank + 1) * 100000; i++) {
        // Busy work
    }
    
    std::cout << "Rank " << rank << ": Finished work, waiting at barrier" << std::endl;
    
    // Synchronize all processes
    MPI_Barrier(MPI_COMM_WORLD);
    
    std::cout << "Rank " << rank << ": Passed barrier!" << std::endl;
    
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "All processes synchronized successfully." << std::endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║   Advanced MPI Operations Demo            ║" << std::endl;
        std::cout << "║   For Traffic Simulation                   ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝" << std::endl;
        std::cout << "\nRunning with " << size << " processes" << std::endl;
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Demonstrate various MPI operations
    demonstrate_broadcast();
    MPI_Barrier(MPI_COMM_WORLD);
    
    demonstrate_gather();
    MPI_Barrier(MPI_COMM_WORLD);
    
    demonstrate_scatter();
    MPI_Barrier(MPI_COMM_WORLD);
    
    demonstrate_reduce();
    MPI_Barrier(MPI_COMM_WORLD);
    
    demonstrate_shared_memory();
    MPI_Barrier(MPI_COMM_WORLD);
    
    demonstrate_barrier();
    
    if (rank == 0) {
        std::cout << "\n╔════════════════════════════════════════════╗" << std::endl;
        std::cout << "║   All demonstrations completed!            ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════╝\n" << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}
