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

#include <asctec_comm/helper.h>
#include <asctec_comm/uart.h>

namespace asctec_comm
{
namespace helper
{

TransportPtr createUartTransport(const std::string & port, int baudrate)
{
  auto uart = std::make_shared<Uart>();
  if(!uart->connect(port, baudrate))
  {
    return nullptr;
  }

  // Initiate the datalink layer, use UART as raw buffer
  auto dataLink = std::make_shared<DataLink>(uart);

  // Initiate transport layer
  return std::make_shared<Transport>(dataLink);
}

void ConfigureMessageRates::addMessage(uint32_t id, uint16_t div)
{
  Entry e;
  e.id = id;
  e.div = div;
  entries_.push_back(e);
}

}
}
