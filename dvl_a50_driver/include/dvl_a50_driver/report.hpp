#pragma once

#include <array>

namespace waterlinked
{

struct TransducerReport
{
  // Transducer ID (0-3)
  int id;

  // Velocity measurement obtained from the particular transducer (m/s)
  double velocity;

  // Distance to the reflecting surface (m)
  double distance;

  // Signal strength of the reflection (dBm)
  double rssi;

  // Noise level of the reflection (dBm)
  double nsd;

  // Indicates whether or not the beam is valid (True/False)
  bool beam_valid;
};

/**
 * @brief Velocity report sent by the DVL A50.
 *
 * @details A velocity-and-transducer report is sent for each velocity calculation of the DVL. The rate depends on the
 *          altitude of the DVL (distance to the sea bottom or other reflecting surface), but will be in the range 2-15
 *          Hz.
 *
 * @note The X, Y, and Z axes are with respect to body frame of the DVL, or the vehicle frame if the DVL is mounted on a
 *       vehicle at an angle, specified as a 'mounting rotation offset', from the forward axis of the vehicle.
 */
struct VelocityReport
{
  // Milliseconds since last velocity report (ms)
  double time;

  // Velocity in x direction (m/s)
  double vx;

  // Velocity in y direction (m/s)
  double vy;

  // Velocity in z direction (m/s)
  double vz;

  // Figure of merit, a measure of the accuracy of the velocities (m/s)
  double fom;

  // Covariance matrix for the velocities. The figure of merit is calculated from this (entries in (m/s)^2)
  std::array<std::array<double, 3>, 3> covariance;

  // Distance to the reflecting surface along the Z axis (m)
  double altitude;

  // Report regarding the transducers
  std::array<TransducerReport, 4> transducers;

  // If true, the DVL has a lock on the reflecting surface, and the altitude and velocities are valid (True/False)
  bool velocity_valid;

  // 8 bit status mask. Bit 0 is set to 1 for high temperature and DVL will soon enter thermal shutdown. Remaining bits
  // are reserved for future use.
  char status;

  // Timestamp of the surface reflection, aka 'center of ping' (Unix timestamp in microseconds)
  int time_of_validity;

  // Timestamp from immediately before sending of the report over TCP (Unix timestamp in microseconds)
  int time_of_transmission;
};

/**
 * @brief Report containing the relative position and orientation of the DVL A50.
 *
 * @details A dead reckoning report outputs the current position and orientation of the DVL as calculated by
 *          dead reckoning, with respect to a frame defined by the axes of the DVL's body frame, or vehicle frame if a
 *          mounting rotation offset is set, at the start of the dead reckoning run. The expected update rate is 5 Hz.
 */
struct DeadReckoningReport
{
  // Time stamp of report (Unix timestamp in seconds)
  double ts;

  // Distance in X direction (m)
  double x;

  // Distance in Y direction (m)
  double y;

  // Distance in Z direction (m)
  double z;

  // Standard deviation (figure of merit) for position (m)
  double std;

  // Rotation around X axis (degrees)
  double roll;

  // Rotation around Y axis (degrees)
  double pitch;

  // Rotation around Z axis (degrees)
  double yaw;

  // Reports if there are any issues with the DVL (0 if no errors, 1 otherwise)
  int status;
};

}  // namespace waterlinked
