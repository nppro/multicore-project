#!/bin/bash

echo "========================================"
echo "Traffic Simulation Build & Test Script"
echo "========================================"
echo ""

# Check if MPI is installed
echo "1. Checking MPI installation..."
if command -v mpirun &> /dev/null; then
    echo "   ✓ MPI found: $(mpirun --version | head -n 1)"
else
    echo "   ✗ MPI not found. Please install OpenMPI or MPICH"
    echo "   Ubuntu/Debian: sudo apt-get install libopenmpi-dev openmpi-bin"
    echo "   macOS: brew install open-mpi"
    exit 1
fi
echo ""

# Check if mpic++ is available
echo "2. Checking MPI compiler..."
if command -v mpic++ &> /dev/null; then
    echo "   ✓ mpic++ found"
else
    echo "   ✗ mpic++ not found"
    exit 1
fi
echo ""

# Compile the code
echo "3. Compiling traffic simulation..."
if make clean > /dev/null 2>&1 && make > /dev/null 2>&1; then
    echo "   ✓ Compilation successful"
else
    echo "   ✗ Compilation failed. See errors above."
    exit 1
fi
echo ""

# Run a quick test
echo "4. Running quick test (2 processes, 10 iterations)..."
echo "   Output:"
echo "   ----------------------------------------"
mpirun -np 2 ./traffic_sim 10 8 2>&1 | head -n 20
echo "   ----------------------------------------"
echo "   ✓ Test completed"
echo ""

echo "========================================"
echo "Build & Test Successful!"
echo "========================================"
echo ""
echo "Next steps:"
echo "  • Run full simulation: mpirun -np 4 ./traffic_sim"
echo "  • See README.md for more options"
echo "  • Generate graphs: python3 visualize_speedup.py"
echo ""
