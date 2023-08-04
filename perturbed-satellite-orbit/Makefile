# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++14 \
	-I$(CONDA_PREFIX)/include \
	-I$(CONDA_PREFIX)/include/eigen3 \
	-I$(CONDA_PREFIX)/include/sofa \
	-I$(CONDA_PREFIX)/include/cspice -I$(CONDA_PREFIX)/include/boost \
	-I/home/simon/lib/tudat-bundle/tudat/include -I/home/simon/lib/tudat-bundle/build/tudat/include \
	-I../utils

# Output binary name
BUILD_DIR := build
TARGET := $(BUILD_DIR)/perturbed-sat

# Source files (add more .cpp files here if needed)
SRCS := $(wildcard *.cpp)

# Object files (automatically generated from source files)
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.cpp=.o))

# Library flags for linking
LDFLAGS := -L$(CONDA_PREFIX)/lib -L/home/simon/lib/tudat-bundle/build/lib

TUDAT_LIBS := \
	-ltudat_environment_setup \
	-ltudat_propagation_setup \
	-ltudat_propulsion \
	-ltudat_data \
	-ltudat_system_models \
	-ltudat_acceleration_partials \
	-ltudat_propagators \
	-ltudat_low_thrust_trajectories \
	-ltudat_shape_based_methods \
	-ltudat_relativity \
	-ltudat_root_finders \
	-ltudat_mission_segments \
	-ltudat_numerical_integrators \
	-ltudat_numerical_quadrature \
	-ltudat_statistics \
	-ltudat_observation_models \
	-ltudat_observation_partials \
	-ltudat_test_support \
	-ltudat_estimatable_parameters \
	-ltudat_orbit_determination \
	-ltudat_torque_partials \
	-ltudat_estimation_setup \
	-ltudat_geometric_shapes \

TUDAT_LIBS_BASE := \
	 -ltudat_spice_interface \
	-ltudat_gravitation \
	-ltudat_ephemerides \
	-ltudat_earth_orientation \
	-ltudat_interpolators \
	-ltudat_ground_stations \
	-ltudat_basic_mathematics \
	-ltudat_basic_astrodynamics \
	-ltudat_reference_frames \
	-ltudat_aerodynamics \
	-ltudat_sofa_interface \
	-ltudat_input_output \
	-ltudat_electromagnetism \
	-ltudat_basics \



# Libraries to link
LDLIBS := $(TUDAT_LIBS)	$(TUDAT_LIBS_BASE) -lm -lcspice -lsofa -lboost_filesystem -lboost_system

# Library paths
RPATH := -Wl,-rpath,$(CONDA_PREFIX)/lib

# Compilation rule
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(RPATH) $(LDFLAGS) $(LDLIBS) 

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
