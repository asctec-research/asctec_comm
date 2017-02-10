/*
 * Copyright (C) 2017 Intel Deutschland GmbH, Germany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sstream>

#include <asctec_uav_msgs/message_definitions.h>

#include <asctec_comm/types.h>

namespace asctec_comm
{

std::ostream& operator <<(std::ostream& stream, const ByteVector& v)
{
  stream << std::hex;
  for (auto& c : v) {
    stream << (int) c << " ";
  }
  stream << std::endl;
  return stream;
}

std::string flightModeFlagsToString(int32_t flightMode)
{
  std::stringstream ss;

  if (flightMode & asctec_uav_msgs::FLIGHTMODE_ACC)
    ss << "absolute angle control, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_POS)
    ss << "xy position control, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_FLYING)
    ss << "motors running, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_EMERGENCY)
    ss << "emergency, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_TRAJECTORY)
    ss << "automatic navigation, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_HEIGHT)
    ss << "altitude control, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_MOTOR_CURRENT_CALIB)
    ss << "motor current_calibration running, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_AUTO_COMPASS_CALIB)
    ss << "auto compass calibration_running, ";
  if (flightMode & asctec_uav_msgs::FLIGHTMODE_HOVER_CALIB)
    ss << "hover calibration running, ";

  if (ss.str().empty()) {
    ss << "unknown";
  }

  std::string ret_string = ss.str();
  const size_t last_comma_pos = ret_string.find_last_of(",");
  if (last_comma_pos != std::string::npos) {
    ret_string.erase(ret_string.find_last_of(","), 1);
  }

  return ret_string;
}

std::string safetyPilotFlagsToString(uint8_t safetyPilotFlags)
{
  std::stringstream ss;

  if (safetyPilotFlags & asctec_uav_msgs::SAFETY_PILOT_STATE_ARMED)
    ss << "armed, ";
  if (safetyPilotFlags & asctec_uav_msgs::SAFETY_PILOT_STATE_ACTIVE)
    ss << "active, ";
  if (safetyPilotFlags & asctec_uav_msgs::SAFETY_PILOT_STATE_STICKS_ACTIVE)
    ss << "sticks active, ";

  if (safetyPilotFlags == 0) {
    ss.clear();
    ss << "inactive";
  }

  std::string ret_string = ss.str();
  const size_t last_comma_pos = ret_string.find_last_of(",");
  if (last_comma_pos != std::string::npos) {
    ret_string.erase(ret_string.find_last_of(","), 1);
  }

  return ret_string;
}

std::string vehicleTypeToString(const asctec_uav_msgs::VehicleType& vehicleType)
{
  using namespace asctec_uav_msgs;

  switch (vehicleType) {
    case VehicleType::VEHICLE_TYPE_HUMMINGBIRD:
      return std::string("Hummingbird");
    case VehicleType::VEHICLE_TYPE_PELICAN:
      return std::string("Pelican");
    case VehicleType::VEHICLE_TYPE_FIREFLY:
      return std::string("Firefly");
    case VehicleType::VEHICLE_TYPE_NEO_6_9:
      return std::string("Neo 6-9");
    case VehicleType::VEHICLE_TYPE_NEO_6_11:
      return std::string("Neo 6-11");
    default:
      return std::string("unknown");
  }
}

int getNumberOfRotorsFromVehicleType(const asctec_uav_msgs::VehicleType& vehicleType)
{
  using asctec_uav_msgs::VehicleType;

  switch (vehicleType) {
    case VehicleType::VEHICLE_TYPE_HUMMINGBIRD:
    case VehicleType::VEHICLE_TYPE_PELICAN:
      return 4;
    case VehicleType::VEHICLE_TYPE_FIREFLY:
    case VehicleType::VEHICLE_TYPE_NEO_6_9:
    case VehicleType::VEHICLE_TYPE_NEO_6_11:
      return 6;
    default:
      return -1;
  };
}

}  // end namespace asctec_comm
