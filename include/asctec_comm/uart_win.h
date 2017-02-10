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

#include <asctec_comm/raw_buffer.h>

#include <string>
#include <cstdint>

namespace asctec_comm
{

  class Uart : public RawBuffer
  {
  public:
    Uart();
    virtual ~Uart();

    /** Connect to serial port. We always use 8N1 as configuration. */
    bool connect(const std::string & port, int baudrate);

    /** Set read timeout in milliseconds */
    void setReadTimeout(uint32_t timeout);

    /** Set write timeout in milliseconds */
    void setWriteTimeout(uint32_t timeout);

    virtual int readBuffer(uint8_t* pData, int size);

    virtual int writeBuffer(uint8_t* pData, int size);

  private:
    void setTimeouts(uint32_t read, uint32_t write);

    void* hCom_;

    uint32_t readTimeout_;
    uint32_t writeTimeout_;
  };

} // end namespace asctec_comm