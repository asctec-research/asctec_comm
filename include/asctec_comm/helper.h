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

#include <vector>
#include "transport.h"

namespace asctec_comm
{
namespace helper
{

TransportPtr createUartTransport(const std::string & port, int baudrate);

class ConfigureMessageRates
{
public:
  template<class Rep, class Period>
  bool sendConfiguration(TransportPtr transport, const std::chrono::duration<Rep, Period>& timeout);

  void addMessage(uint32_t id, uint16_t div);

private:
  struct Entry
  {
    uint32_t id;
    uint16_t div;
  };

  std::vector<Entry> entries_;
};

}
}

#include <asctec_comm/implementation/helper_impl.h>
