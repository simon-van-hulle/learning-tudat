#!/usr/bin/env python

# This file is entirely based on an example from the Tudat repository.
# The original file can be found here:  https://github.com/tudat-team/tudatpy-examples/blob/83780c76062fb23aabea3386866cb368dfacaf86/propagation/perturbed_satellite_orbit.py


import matplotlib.pyplot as plt
import numpy as np



## Post-process the propagation results
"""
The results of the propagation are then processed to a more user-friendly form.
"""


### Read the results
states_array_python = np.loadtxt("output/python/state.dat")[1:]
dep_vars_array_python = np.loadtxt("output/python/depvar.dat")[1:]
states_array_cpp = np.loadtxt("output/cpp/state.dat")[1:]
dep_vars_array_cpp = np.loadtxt("output/cpp/depvar.dat")[1:]
outputdir = "output"



state_diff = ((states_array_cpp - states_array_python) / states_array_cpp)
dep_var_diff = ((dep_vars_array_cpp - dep_vars_array_python) / dep_vars_array_cpp)

state_diff = state_diff[np.abs(state_diff) < 50]
dep_var_diff = dep_var_diff[np.abs(dep_var_diff) < 50]

plt.figure(figsize=(9, 5))
plt.hist(dep_var_diff.flatten(), alpha=0.5,  bins=100, label="Dependent variables", log=True)
plt.hist(state_diff.flatten(), alpha=0.5, bins=100, label="State", color='r', log=True)
# plt.scatter(state_diff.flatten(), dep_var_diff.flatten())
# plt.plot(state_diff.flatten())
# plt.hist(dep_var_diff.flatten(),label="Dependent variables")
plt.legend()
plt.xlabel("Relative difference [%]")
plt.ylabel("Frequency")
plt.title("Relative difference between Python and C++ results")
plt.savefig(f"{outputdir}/relative_difference.pdf")
plt.savefig(f"figs/relative_difference.png")




for states_array, dep_vars_array, outputdir in zip([states_array_python, states_array_cpp], [dep_vars_array_python, dep_vars_array_cpp], ["output/python/", "output/cpp/"]):

    
    ### Total acceleration over time
    """
    Let's first plot the total acceleration on the satellite over time. This can be done by taking the norm of the first three columns of the dependent variable list.
    """

    # Plot total acceleration as function of time
    time_hours = dep_vars_array[:,0]/3600
    total_acceleration_norm = np.linalg.norm(dep_vars_array[:,1:4], axis=1)
    plt.figure(figsize=(9, 5))
    plt.title("Total acceleration norm on Delfi-C3 over the course of propagation.")
    plt.plot(time_hours, total_acceleration_norm)
    plt.xlabel('Time [hr]')
    plt.ylabel('Total Acceleration [m/s$^2$]')
    plt.xlim([min(time_hours), max(time_hours)])
    plt.grid()
    plt.tight_layout()
    plt.savefig(f"{outputdir}/total_acceleration.pdf")

    ### Ground track
    """
    Let's then plot the ground track of the satellite in its first 3 hours. This makes use of the latitude and longitude dependent variables.
    """

    # Plot ground track for a period of 3 hours
    latitude = dep_vars_array[:,10]
    longitude = dep_vars_array[:,11]
    hours = 3
    subset = int(len(time_hours) / 24 * hours)
    latitude = np.rad2deg(latitude[0: subset])
    longitude = np.rad2deg(longitude[0: subset])
    plt.figure(figsize=(9, 5))
    plt.title("3 hour ground track of Delfi-C3")
    plt.scatter(longitude, latitude, s=1)
    plt.xlabel('Longitude [deg]')
    plt.ylabel('Latitude [deg]')
    plt.xlim([min(longitude), max(longitude)])
    plt.yticks(np.arange(-90, 91, step=45))
    plt.grid()
    plt.tight_layout()
    plt.savefig(f"{outputdir}/ground_track.pdf")

    ### Kepler elements over time
    """
    Let's now plot each of the 6 Kepler element as a function of time, also as saved in the dependent variables.
    """

    # Plot Kepler elements as a function of time
    kepler_elements = dep_vars_array[:,4:10]
    fig, ((ax1, ax2), (ax3, ax4), (ax5, ax6)) = plt.subplots(3, 2, figsize=(9, 12))
    fig.suptitle('Evolution of Kepler elements over the course of the propagation.')

    # Semi-major Axis
    semi_major_axis = kepler_elements[:,0] / 1e3
    ax1.plot(time_hours, semi_major_axis)
    ax1.set_ylabel('Semi-major axis [km]')

    # Eccentricity
    eccentricity = kepler_elements[:,1]
    ax2.plot(time_hours, eccentricity)
    ax2.set_ylabel('Eccentricity [-]')

    # Inclination
    inclination = np.rad2deg(kepler_elements[:,2])
    ax3.plot(time_hours, inclination)
    ax3.set_ylabel('Inclination [deg]')

    # Argument of Periapsis
    argument_of_periapsis = np.rad2deg(kepler_elements[:,3])
    ax4.plot(time_hours, argument_of_periapsis)
    ax4.set_ylabel('Argument of Periapsis [deg]')

    # Right Ascension of the Ascending Node
    raan = np.rad2deg(kepler_elements[:,4])
    ax5.plot(time_hours, raan)
    ax5.set_ylabel('RAAN [deg]')

    # True Anomaly
    true_anomaly = np.rad2deg(kepler_elements[:,5])
    ax6.scatter(time_hours, true_anomaly, s=1)
    ax6.set_ylabel('True Anomaly [deg]')
    ax6.set_yticks(np.arange(0, 361, step=60))

    for ax in fig.get_axes():
        ax.set_xlabel('Time [hr]')
        ax.set_xlim([min(time_hours), max(time_hours)])
        ax.grid()
    plt.tight_layout()
    plt.savefig(f"{outputdir}/kepler_elements.pdf")

    ### Accelerations over time
    """
    Finally, let's plot and compare each of the included accelerations.
    """

    plt.figure(figsize=(9, 5))

    # Point Mass Gravity Acceleration Sun
    acceleration_norm_pm_sun = dep_vars_array[:,12]
    plt.plot(time_hours, acceleration_norm_pm_sun, label='PM Sun')

    # Point Mass Gravity Acceleration Moon
    acceleration_norm_pm_moon = dep_vars_array[:,13]
    plt.plot(time_hours, acceleration_norm_pm_moon, label='PM Moon')

    # Point Mass Gravity Acceleration Mars
    acceleration_norm_pm_mars = dep_vars_array[:,14]
    plt.plot(time_hours, acceleration_norm_pm_mars, label='PM Mars')

    # Point Mass Gravity Acceleration Venus
    acceleration_norm_pm_venus = dep_vars_array[:,15]
    plt.plot(time_hours, acceleration_norm_pm_venus, label='PM Venus')

    # Spherical Harmonic Gravity Acceleration Earth
    acceleration_norm_sh_earth = dep_vars_array[:,16]
    plt.plot(time_hours, acceleration_norm_sh_earth, label='SH Earth')

    # Aerodynamic Acceleration Earth
    acceleration_norm_aero_earth = dep_vars_array[:,17]
    plt.plot(time_hours, acceleration_norm_aero_earth, label='Aerodynamic Earth')

    # Cannonball Radiation Pressure Acceleration Sun
    acceleration_norm_rp_sun = dep_vars_array[:,18]
    plt.plot(time_hours, acceleration_norm_rp_sun, label='Radiation Pressure Sun')

    plt.xlim([min(time_hours), max(time_hours)])
    plt.xlabel('Time [hr]')
    plt.ylabel('Acceleration Norm [m/s$^2$]')

    plt.legend(bbox_to_anchor=(1.005, 1))
    plt.suptitle("Accelerations norms on Delfi-C3, distinguished by type and origin, over the course of propagation.")
    plt.yscale('log')
    plt.grid()
    plt.tight_layout()
    plt.savefig(f"{outputdir}/acceleration_norms.pdf")
