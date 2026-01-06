#include "traffic_simulation.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iomanip>

// Constructor
TrafficController::TrafficController(int _rank, int _size, int _num_segments) 
    : rank(_rank), size(_size), num_segments(_num_segments), 
      emergency_mode(false), shared_memory(nullptr) {
    
    // Initialize random number generator
    rng.seed(std::chrono::system_clock::now().time_since_epoch().count() + rank);
    dist = std::uniform_real_distribution<double>(0.0, 1.0);
    
    // Initialize segments for this process
    int segments_per_process = num_segments / size;
    for (int i = 0; i < segments_per_process; i++) {
        int segment_id = rank * segments_per_process + i;
        bool has_crossing = (segment_id % 5 == 0); // Every 5th segment has crossing
        segments.push_back(RoadSegment(segment_id, 10, has_crossing));
    }
}

// Destructor
TrafficController::~TrafficController() {
    if (shared_memory != nullptr) {
        MPI_Win_free(&shared_window);
    }
}

// Initialize the simulation
void TrafficController::initialize() {
    // Allocate shared memory for coordination
    int mem_size = num_segments * sizeof(int);
    
    MPI_Win_allocate(mem_size, sizeof(int), MPI_INFO_NULL, 
                     MPI_COMM_WORLD, &shared_memory, &shared_window);
    
    // Initialize shared memory
    if (rank == 0) {
        for (int i = 0; i < num_segments; i++) {
            shared_memory[i] = 0; // 0 = free, 1 = occupied
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "=== Traffic Simulation Initialized ===" << std::endl;
        std::cout << "Number of processes: " << size << std::endl;
        std::cout << "Number of road segments: " << num_segments << std::endl;
        std::cout << "Segments per process: " << num_segments / size << std::endl;
        std::cout << "=====================================" << std::endl;
    }
}

// Main simulation loop
void TrafficController::run_simulation(int iterations) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    SimulationStats stats;
    
    for (int iter = 0; iter < iterations; iter++) {
        // Check for emergency vehicles
        if (dist(rng) < 0.05) { // 5% chance of emergency
            Vehicle ambulance(1000 + iter, AMBULANCE, rank * 10, rank * 10 + 50);
            handle_emergency(ambulance);
            stats.emergency_vehicles_processed++;
        }
        
        // Add random vehicles
        if (dist(rng) < 0.3) { // 30% chance of new vehicle
            VehicleType type = (dist(rng) < 0.7) ? CAR : WALKER;
            Vehicle v(iter * size + rank, type, rank * 10, (rank + 1) * 10);
            add_vehicle(v);
        }
        
        // Manage traffic lights
        manage_traffic_lights();
        
        // Process vehicles
        process_vehicles();
        
        // Check for deadlock
        check_for_deadlock();
        
        // Update shared memory
        update_shared_memory();
        
        // Synchronize processes
        synchronize_processes();
        
        // Gather statistics every 10 iterations
        if (iter % 10 == 0 && rank == 0) {
            std::cout << "Iteration " << iter << " completed" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    stats.total_execution_time = elapsed.count();
    
    // Gather final statistics
    gather_statistics();
    
    if (rank == 0) {
        std::cout << "\n=== Simulation Complete ===" << std::endl;
        std::cout << "Total execution time: " << stats.total_execution_time << " seconds" << std::endl;
        std::cout << "Emergency vehicles handled: " << stats.emergency_vehicles_processed << std::endl;
    }
}

// Process vehicles in all segments
void TrafficController::process_vehicles() {
    for (auto& segment : segments) {
        std::vector<Vehicle> remaining_vehicles;
        
        for (auto& vehicle : segment.vehicles) {
            // Check if vehicle reached destination
            if (vehicle.position >= vehicle.destination) {
                // Vehicle completed journey
                continue;
            }
            
            // Move vehicle based on traffic state and type
            if (segment.state == GREEN || vehicle.type == AMBULANCE) {
                vehicle.position += static_cast<int>(vehicle.speed * 10);
                vehicle.wait_time = 0;
            } else {
                vehicle.wait_time++;
            }
            
            // Keep vehicle if not at destination
            if (vehicle.position < vehicle.destination) {
                remaining_vehicles.push_back(vehicle);
            }
        }
        
        segment.vehicles = remaining_vehicles;
    }
}

// Handle emergency vehicle (ambulance)
void TrafficController::handle_emergency(const Vehicle& ambulance) {
    emergency_mode = true;
    emergency_queue.push(ambulance);
    
    // Broadcast emergency to all processes
    broadcast_emergency();
    
    // Clear path for ambulance
    for (auto& segment : segments) {
        segment.state = EMERGENCY;
    }
    
    if (rank == 0) {
        std::cout << ">>> EMERGENCY: Ambulance #" << ambulance.id 
                  << " detected! Clearing path..." << std::endl;
    }
    
    // Process emergency vehicle with priority
    if (!emergency_queue.empty()) {
        Vehicle amb = emergency_queue.front();
        emergency_queue.pop();
        
        // Add to appropriate segment with highest priority
        int target_segment = amb.position / 10 % segments.size();
        if (target_segment < segments.size()) {
            segments[target_segment].vehicles.insert(
                segments[target_segment].vehicles.begin(), amb
            );
        }
    }
    
    // Reset emergency mode after some time
    MPI_Barrier(MPI_COMM_WORLD);
    emergency_mode = false;
    
    for (auto& segment : segments) {
        if (segment.state == EMERGENCY) {
            segment.state = GREEN;
        }
    }
}

// Manage traffic light states
void TrafficController::manage_traffic_lights() {
    if (emergency_mode) {
        return; // Keep emergency state during emergency
    }
    
    for (auto& segment : segments) {
        // Simple traffic light logic
        if (segment.vehicles.size() > segment.capacity * 0.7) {
            segment.state = RED; // Too crowded, stop new vehicles
        } else if (segment.vehicles.empty()) {
            segment.state = GREEN; // Empty, allow vehicles
        } else {
            // Rotate based on segment id for coordination
            int cycle = (segment.segment_id % 3);
            if (cycle == 0) segment.state = GREEN;
            else if (cycle == 1) segment.state = YELLOW;
            else segment.state = RED;
        }
        
        // Handle train crossing
        if (segment.has_crossing && dist(rng) < 0.1) { // 10% chance of train
            segment.state = RED;
            if (rank == 0) {
                std::cout << "Train crossing at segment " << segment.segment_id << std::endl;
            }
        }
    }
}

// Check for deadlock situations
void TrafficController::check_for_deadlock() {
    for (auto& segment : segments) {
        // Check if vehicles are waiting too long
        bool potential_deadlock = false;
        for (const auto& vehicle : segment.vehicles) {
            if (vehicle.wait_time > 50 && vehicle.type != AMBULANCE) {
                potential_deadlock = true;
                break;
            }
        }
        
        // Resolve deadlock by forcing green light
        if (potential_deadlock) {
            segment.state = GREEN;
            if (rank == 0) {
                std::cout << "Deadlock detected at segment " << segment.segment_id 
                          << ", forcing green light" << std::endl;
            }
        }
    }
}

// Update shared memory with current segment states
void TrafficController::update_shared_memory() {
    for (const auto& segment : segments) {
        int occupied = (segment.vehicles.size() > 0) ? 1 : 0;
        
        // Use MPI_Win_lock for atomic operations
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, shared_window);
        MPI_Put(&occupied, 1, MPI_INT, 0, segment.segment_id, 1, MPI_INT, shared_window);
        MPI_Win_unlock(0, shared_window);
    }
}

// Synchronize all processes
void TrafficController::synchronize_processes() {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Win_fence(0, shared_window);
}

// Broadcast emergency signal to all processes
void TrafficController::broadcast_emergency() {
    int emergency_signal = emergency_mode ? 1 : 0;
    MPI_Bcast(&emergency_signal, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        emergency_mode = (emergency_signal == 1);
    }
}

// Gather statistics from all processes
void TrafficController::gather_statistics() {
    int local_vehicle_count = 0;
    for (const auto& segment : segments) {
        local_vehicle_count += segment.vehicles.size();
    }
    
    int total_vehicles = 0;
    MPI_Reduce(&local_vehicle_count, &total_vehicles, 1, MPI_INT, 
               MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "Total vehicles in system: " << total_vehicles << std::endl;
    }
}

// Scatter vehicles to different processes
void TrafficController::scatter_vehicles() {
    // This would distribute vehicles from master to workers
    // Implementation depends on specific requirements
}

// Add vehicle to appropriate segment
void TrafficController::add_vehicle(const Vehicle& v) {
    int target_segment = v.position / 10 % segments.size();
    if (target_segment < segments.size()) {
        if (check_capacity(target_segment)) {
            segments[target_segment].vehicles.push_back(v);
        }
    }
}

// Remove vehicle from system
void TrafficController::remove_vehicle(int vehicle_id) {
    for (auto& segment : segments) {
        auto it = std::remove_if(segment.vehicles.begin(), segment.vehicles.end(),
                                  [vehicle_id](const Vehicle& v) { return v.id == vehicle_id; });
        segment.vehicles.erase(it, segment.vehicles.end());
    }
}

// Check if segment has capacity
bool TrafficController::check_capacity(int segment_id) {
    if (segment_id < segments.size()) {
        return segments[segment_id].vehicles.size() < segments[segment_id].capacity;
    }
    return false;
}

// Print current statistics
void TrafficController::print_statistics() {
    if (rank == 0) {
        std::cout << "\n=== Current Statistics ===" << std::endl;
        for (const auto& segment : segments) {
            std::cout << "Segment " << segment.segment_id 
                      << ": " << segment.vehicles.size() << " vehicles, "
                      << "State: " << segment.state << std::endl;
        }
    }
}

// Calculate speedup based on Amdahl's Law
double TrafficController::calculate_speedup(double serial_time, double parallel_time) {
    return serial_time / parallel_time;
}
