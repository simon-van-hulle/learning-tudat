// This file is made based on the example code from TudatPy
// https://github.com/tudat-team/tudat-space/blob/develop/docs/source/_src_getting_started/_src_examples/notebooks/propagation/perturbed_satellite_orbit.ipynb
// It is used purely for testing purposes and for getting to know the C++ tudat environment.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <Eigen/Core>
#include <iostream>
// #include <limits>

// #include "tudat/astro/basic_astro/physicalConstants.h"
// #include "tudat/astro/basic_astro/unitConversions.h"
// #include "tudat/astro/ephemerides/approximatePlanetPositions.h"
// #include "tudat/astro/ephemerides/tabulatedEphemeris.h"

#include "tudat/interface/spice/spiceEphemeris.h"
// #include "tudat/interface/spice/spiceRotationalEphemeris.h"

// #include "tudat/astro/aerodynamics/testApolloCapsuleCoefficients.h"
#include "tudat/astro/electromagnetism/radiationPressureInterface.h"
// #include "tudat/astro/ephemerides/constantRotationalEphemeris.h"
#include "tudat/io/basicInputOutput.h"
// #include "tudat/math/interpolators/linearInterpolator.h"
#include "tudat/simulation/environment_setup/createBodies.h"
#include "tudat/simulation/environment_setup/defaultBodies.h"
// #include "tudat/simulation/estimation_setup/createNumericalSimulator.h"

#define deg2Rad unit_conversions::convertDegreesToRadians
#define ENABLE_LOGGING
#include "logging.h"

using namespace tudat;
// using namespace simulation_setup;
// using namespace basic_astrodynamics;
// using namespace input_output;
// using namespace reference_frames;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                   GENERAL CONFIGURATION AND MAGIC NUMBERS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const double START_EPOCH_DAYS = 0.0;
const double END_EPOCH_DAYS = 1.0;

// DelfiC3

const std::string C3_NAME = "DelfiC3";
const double C3_MASS = 400.0;
const double C3_REFERENCE_AREA = 4.0;
const double C3_DRAG_COEFFICIENT = 1.2;
const double C3_RAD_PRES_AREA = 4.0;
const double C3_RAD_PRES_COEFFICIENT = 1.2;

const std::vector<std::string> RAD_PRES_OCCULTING_BODIES{"Earth"};

// Magic numbers
const double ENV_SETUP_BUFFER = 300.0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                MAIN FUNCTION
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    LOG_START();
    LOG("Configuration");

    // Load Spice kernel with gravitational parameters.
    spice_interface::loadStandardSpiceKernels();

    // Set simulation start and end epoch
    const double simulationStartEpoch = START_EPOCH_DAYS * physical_constants::JULIAN_DAY;
    const double simulationEndEpoch = END_EPOCH_DAYS * physical_constants::JULIAN_DAY;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //                              ENVIRONMENT SETUP
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LOG_TITLE("Environment setup");
    // Create body objects.
    LOG("Creating Bodies");
    //  Define string names for bodies to be created from default.
    std::vector<std::string> bodiesToCreate{"Sun", "Earth", "Moon", "Mars", "Venus"};

    // Use "Earth"/"J2000" as global frame origin and orientation.
    std::string frameOrigin = "SSB";
    std::string frameOrientation = "ECLIPJ2000";

    // Create default body settings, usually from `spice`.
    simulation_setup::BodyListSettings bodySettings = simulation_setup::getDefaultBodySettings(
        bodiesToCreate, simulationStartEpoch - ENV_SETUP_BUFFER,
        simulationEndEpoch + ENV_SETUP_BUFFER, frameOrigin, frameOrientation
    );

    // Create system of bodies
    LOG("Creating system of bodies");
    simulation_setup::SystemOfBodies bodies = simulation_setup::createSystemOfBodies(bodySettings);

    // Create vehicle object.
    LOG("Creating vehicle objects");
    bodies.createEmptyBody(C3_NAME);
    bodies.at(C3_NAME)->setConstantBodyMass(C3_MASS);

    // Create aerodynamic interface settings
    LOG("Creating aerodynamic interface settings");
    std::shared_ptr<simulation_setup::AerodynamicCoefficientSettings>
        aerodynamicCoefficientSettings = simulation_setup::constantAerodynamicCoefficientSettings(
            C3_REFERENCE_AREA, Eigen::Vector3d(C3_DRAG_COEFFICIENT, 0.0, 0.0)
        );

    bodies.at(C3_NAME)->setAerodynamicCoefficientInterface(
        simulation_setup::createAerodynamicCoefficientInterface(
            aerodynamicCoefficientSettings, C3_NAME, bodies
        )
    );

    // Create radiation pressure settings
    LOG("Creating radiation pressure settings");
    std::shared_ptr<simulation_setup::RadiationPressureInterfaceSettings>
        C3RadiationPressureSettings = simulation_setup::cannonBallRadiationPressureSettings(
            "Sun", C3_RAD_PRES_AREA, C3_RAD_PRES_COEFFICIENT, RAD_PRES_OCCULTING_BODIES
        );

    bodies.at(C3_NAME)->setRadiationPressureInterface(
        "Sun", simulation_setup::createRadiationPressureInterface(
                   C3RadiationPressureSettings, C3_NAME, bodies
               )
    );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //                              PROPAGATION SETUP
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LOG_TITLE("Propagation setup");

    // Define bodies to propagate
    LOG("Defining bodies to propagate");
    std::vector<std::string> bodiesToPropagate{C3_NAME};

    // Define central bodies
    LOG("Defining central bodies");
    std::vector<std::string> centralBodies{"Earth"};

    // Create acceleration model
    LOG("Creating acceleration model");
    std::map<std::string, std::vector<std::shared_ptr<simulation_setup::AccelerationSettings>>>
        accelerationsOfC3{
            {"Sun",
             {
                 simulation_setup::cannonBallRadiationPressureAcceleration(),
                 simulation_setup::pointMassGravityAcceleration(),
             }},
            {"Earth",
             {
                 simulation_setup::aerodynamicAcceleration(),
                 simulation_setup::sphericalHarmonicAcceleration(5, 5),
             }},
            {"Moon",
             {
                 simulation_setup::pointMassGravityAcceleration(),
             }},
            {"Mars",
             {
                 simulation_setup::pointMassGravityAcceleration(),
             }},
            {"Venus",
             {
                 simulation_setup::pointMassGravityAcceleration(),
             }}};

    simulation_setup::SelectedAccelerationMap accelerationSettings{{C3_NAME, accelerationsOfC3}};

    // Create acceleration models
    LOG("Creating acceleration models");
    basic_astrodynamics::AccelerationMap accelerationSettingsOfC3 =
        simulation_setup::createAccelerationModelsMap(
            bodies, accelerationSettings, bodiesToPropagate, centralBodies
        );

    // Define initial state
    LOG("Defining initial state");
    WARN("Not implemented yet");
    Eigen::Vector6d InitialState = orbital_element_conversions::convertKeplerianToCartesianElements(
        7500.0e3, 0.1, deg2Rad(85.3), deg2Rad(85.3), deg2Rad(85.3), deg2Rad(85.3),
        bodies.at("Earth")->getGravityFieldModel()->getGravitationalParameter()
    );

    // Define Dependent Variables to save
    LOG("Defining dependent variables to save");
    WARN("Not implemented yet");

    // Create propagator settings
    LOG("Creating integrator and propagator settings");
    WARN("Not implemented yet");

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //                              ORBIT PROPAGATION
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LOG_TITLE("Propagating the orbit");

    // Create simulation object and propagate dynamics.
    LOG("Creating simulation object");

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //                              SAVING THE RESULTS
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LOG_TITLE("Saving the results");
    LOG("Saving to <file name>");

    // End of simulation
    LOG_DONE();
    return 0;
}
