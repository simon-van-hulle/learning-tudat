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
#include "tudat/simulation/estimation_setup/createNumericalSimulator.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                               SETUP AND UTILS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
// Using anonymous namespace to avoid cluttering the global namespace.
namespace {

const double simulationStartEpoch = 0.0 * physical_constants::JULIAN_DAY;
const double simulationEndEpoch = 1.0 * physical_constants::JULIAN_DAY;

// DelfiC3
const std::string dC3Name = "DelfiC3";
const double dC3Mass = 400.0;
const double dC3ReferenceArea = 4.0;
const double dC3DragCoeff = 1.2;
const double dC3RadPressArea = 4.0;
const double dC3RadPressCoeff = 1.2;
const std::vector<std::string> dC3RadPressOccultingBodies = {"Earth"};

// Initial condiation
const double dC3InitialSemiMajorAxis = 7500.0e3;
const double dC3InitialEccentricity = 0.1;
const double dC3InitialInclination = deg2Rad(85.3);
const double dC3InitialRAAN = deg2Rad(235.7);
const double dC3InitialArgOfPeri = deg2Rad(23.4);
const double dC3InitialTrueAnomaly = deg2Rad(139.87);

// Integrator parameters
const double integratorFixedStepSize = 10.0;
const numerical_integrators::CoefficientSets integratorCoefficientSet = numerical_integrators::rungeKutta4Classic;

// Propagator parameters
const propagators::TranslationalPropagatorType propagatorTranslationalType = propagators::cowell;

// Magic numbers
const double envSetupTimeBuffer = 300.0;

} // namespace

//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                MAIN FUNCTION
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
    LOG_START();
    LOG("Configuration");

    // Load Spice kernel with gravitational parameters.
    spice_interface::loadStandardSpiceKernels();

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
        bodiesToCreate,
        simulationStartEpoch - envSetupTimeBuffer,
        simulationEndEpoch + envSetupTimeBuffer,
        frameOrigin,
        frameOrientation
    );

    // Create system of bodies
    LOG("Creating system of bodies");
    simulation_setup::SystemOfBodies bodies = simulation_setup::createSystemOfBodies(bodySettings);

    // Create vehicle object.
    LOG("Creating vehicle objects");
    bodies.createEmptyBody(dC3Name);
    bodies.at(dC3Name)->setConstantBodyMass(dC3Mass);

    // Create aerodynamic interface settings
    LOG("Creating aerodynamic interface settings");
    std::shared_ptr<simulation_setup::AerodynamicCoefficientSettings> aerodynamicCoefficientSettings =
        simulation_setup::constantAerodynamicCoefficientSettings(
            dC3ReferenceArea, Eigen::Vector3d(dC3DragCoeff, 0.0, 0.0)
        );

    bodies.at(dC3Name)->setAerodynamicCoefficientInterface(
        simulation_setup::createAerodynamicCoefficientInterface(aerodynamicCoefficientSettings, dC3Name, bodies)
    );

    // Create radiation pressure settings
    LOG("Creating radiation pressure settings");
    std::shared_ptr<simulation_setup::RadiationPressureInterfaceSettings> C3RadiationPressureSettings =
        simulation_setup::cannonBallRadiationPressureSettings(
            "Sun", dC3RadPressArea, dC3RadPressCoeff, dC3RadPressOccultingBodies
        );

    bodies.at(dC3Name)->setRadiationPressureInterface(
        "Sun", simulation_setup::createRadiationPressureInterface(C3RadiationPressureSettings, dC3Name, bodies)
    );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //                              PROPAGATION SETUP
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LOG_TITLE("Propagation setup");

    // Define bodies to propagate
    LOG("Defining bodies to propagate");
    std::vector<std::string> bodiesToPropagate{dC3Name};

    // Define central bodies
    LOG("Defining central bodies");
    std::vector<std::string> centralBodies{"Earth"};

    // Create acceleration model
    LOG("Creating acceleration model");
    std::map<std::string, std::vector<std::shared_ptr<simulation_setup::AccelerationSettings>>> accelerationsOfC3 = {
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

    simulation_setup::SelectedAccelerationMap accelerationSettings{{dC3Name, accelerationsOfC3}};

    // Create acceleration models
    LOG("Creating acceleration models");
    basic_astrodynamics::AccelerationMap accelerationSettingsOfC3 =
        simulation_setup::createAccelerationModelsMap(bodies, accelerationSettings, bodiesToPropagate, centralBodies);

    // Define initial state
    LOG("Defining initial state");

    double earthGravitationalParameter = bodies.at("Earth")->getGravityFieldModel()->getGravitationalParameter();
    Eigen::VectorXd initialState = orbital_element_conversions::convertKeplerianToCartesianElements(
        dC3InitialSemiMajorAxis,
        dC3InitialEccentricity,
        dC3InitialInclination,
        dC3InitialRAAN,
        dC3InitialArgOfPeri,
        dC3InitialTrueAnomaly,
        earthGravitationalParameter
    );

    LOG("Initial state: ", vectorToString(initialState));

    // Define Dependent Variables to save
    LOG("Defining dependent variables to save");
    std::vector<std::shared_ptr<propagators::SingleDependentVariableSaveSettings>> dependentVariablesToSave = {
        propagators::totalAccelerationDependentVariable(dC3Name),
        propagators::keplerianStateDependentVariable(dC3Name, "Earth"),
        propagators::latitudeDependentVariable(dC3Name, "Earth"),
        propagators::longitudeDependentVariable(dC3Name, "Earth"),
        propagators::singleAccelerationNormDependentVariable(basic_astrodynamics::point_mass_gravity, dC3Name, "Sun"),
        propagators::singleAccelerationNormDependentVariable(basic_astrodynamics::point_mass_gravity, dC3Name, "Moon"),
        propagators::singleAccelerationNormDependentVariable(basic_astrodynamics::point_mass_gravity, dC3Name, "Mars"),
        propagators::singleAccelerationNormDependentVariable(basic_astrodynamics::point_mass_gravity, dC3Name, "Venus"),
        propagators::singleAccelerationNormDependentVariable(
            basic_astrodynamics::spherical_harmonic_gravity, dC3Name, "Earth"
        ),
        propagators::singleAccelerationNormDependentVariable(basic_astrodynamics::aerodynamic, dC3Name, "Earth"),
        propagators::singleAccelerationNormDependentVariable(
            basic_astrodynamics::cannon_ball_radiation_pressure, dC3Name, "Sun"
        ),
    };

    // Create termination settings
    LOG("Creating termination settings");
    std::shared_ptr<propagators::PropagationTerminationSettings> terminationSettings =
        propagators::propagationTimeTerminationSettings(simulationEndEpoch);

    // Create integrator settings
    LOG("Creating integrator settings");
    std::shared_ptr<numerical_integrators::IntegratorSettings<double>> integratorSettings =
        numerical_integrators::rungeKuttaFixedStepSettings(integratorFixedStepSize, integratorCoefficientSet);

    // Create propagator settings
    LOG("Creating integrator and propagator settings");
    std::shared_ptr<propagators::TranslationalStatePropagatorSettings<double, double>> propagatorSettings =
        propagators::translationalStatePropagatorSettings(
            centralBodies,
            accelerationSettingsOfC3,
            bodiesToPropagate,
            initialState, // Not sure if this is the best thing to do.
            simulationStartEpoch,
            integratorSettings,
            terminationSettings,
            propagatorTranslationalType,
            dependentVariablesToSave
        );

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
