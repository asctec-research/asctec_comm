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

#pragma once

#include <inttypes.h>
#include <iostream>
#include <string>
#include <vector>

#include <asctec_uav_msgs/message_definitions.h>

namespace asctec_comm
{

typedef std::vector<uint8_t> ByteVector;
std::ostream& operator <<(std::ostream& stream, const ByteVector& v);

std::string flightModeFlagsToString(int32_t flightMode);
std::string safetyPilotFlagsToString(uint8_t safetyPilotFlags);
std::string vehicleTypeToString(const asctec_uav_msgs::VehicleType& type);

int getNumberOfRotorsFromVehicleType(const asctec_uav_msgs::VehicleType& vehicleType);

} // end namespace asctec_comm
