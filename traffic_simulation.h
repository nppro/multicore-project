#ifndef TRAFFIC_SIMULATION_H
#define TRAFFIC_SIMULATION_H

#include <mpi.h>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <chrono>

// Vehicle types
enum VehicleType {
    CAR,
    WALKER,
    AMBULANCE,
    TRAIN
};

// Traffic states
enum TrafficState {
    RED,
    GREEN,
    YELLOW,
    EMERGENCY
};

// Vehicle structure
struct Vehicle {
    int id;
    VehicleType type;
    int position;
    int destination;
    int priority;
    double speed;
    int wait_time;
    
    Vehicle() : id(0), type(CAR), position(0), destination(0), 
                priority(0), speed(0.0), wait_time(0) {}
    
    Vehicle(int _id, VehicleType _type, int _pos, int _dest) 
        : id(_id), type(_type), position(_pos), destination(_dest),
          priority(_type == AMBULANCE ? 100 : 1), 
          speed(_type == WALKER ? 0.5 : (_type == TRAIN ? 2.0 : 1.0)),
          wait_time(0) {}
};

// Road segment structure
struct RoadSegment {
    int segment_id;
    TrafficState state;
    std::vector<Vehicle> vehicles;
    int capacity;
    bool has_crossing;
    int wait_queue_size;
    
    RoadSegment() : segment_id(0), state(RED), capacity(10), 
                    has_crossing(false), wait_queue_size(0) {}
    
    RoadSegment(int id, int cap, bool crossing = false) 
        : segment_id(id), state(RED), capacity(cap), 
          has_crossing(crossing), wait_queue_size(0) {}
};

// Traffic Controller Class
class TrafficController {
private:
    int rank;
    int size;
    int num_segments;
    std::vector<RoadSegment> segments;
    std::queue<Vehicle> emergency_queue;
    bool emergency_mode;
    MPI_Win shared_window;
    int* shared_memory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    
public:
    TrafficController(int _rank, int _size, int _num_segments);
    ~TrafficController();
    
    // Core functions
    void initialize();
    void run_simulation(int iterations);
    void process_vehicles();
    void handle_emergency(const Vehicle& ambulance);
    void manage_traffic_lights();
    void check_for_deadlock();
    void update_shared_memory();
    void synchronize_processes();
    
    // Communication functions
    void broadcast_emergency();
    void gather_statistics();
    void scatter_vehicles();
    
    // Utility functions
    void add_vehicle(const Vehicle& v);
    void remove_vehicle(int vehicle_id);
    bool check_capacity(int segment_id);
    void print_statistics();
    double calculate_speedup(double serial_time, double parallel_time);
};

// Statistics structure
struct SimulationStats {
    int total_vehicles_processed;
    int emergency_vehicles_processed;
    int deadlock_incidents;
    int race_condition_avoided;
    double average_wait_time;
    double total_execution_time;
    
    SimulationStats() : total_vehicles_processed(0), 
                       emergency_vehicles_processed(0),
                       deadlock_incidents(0), 
                       race_condition_avoided(0),
                       average_wait_time(0.0), 
                       total_execution_time(0.0) {}
};

#endif // TRAFFIC_SIMULATION_H
