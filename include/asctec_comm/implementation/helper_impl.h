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

namespace asctec_comm
{
namespace helper
{

  template<class Rep, class Period>
  bool ConfigureMessageRates::sendConfiguration(TransportPtr transport, const std::chrono::duration<Rep, Period>& timeout)
  {
    ByteVector bv(entries_.size()*6);
    size_t pos = 0;

    for(auto& e : entries_)
    {
      *((uint32_t*)(bv.data()+pos)) = e.id;
      pos += sizeof(uint32_t);
      *((uint16_t*)(bv.data()+pos)) = e.div;
      pos += sizeof(uint16_t);
    }

    return transport->sendDataAcknowledged(timeout, asctec_uav_msgs::MESSAGE_ID_CONFIG_SET_MESSAGE_RATE_DIVISOR, bv);
  }

}
}

