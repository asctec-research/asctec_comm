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

#include <condition_variable>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <memory>
#include <sys/time.h>
#include <thread>
#include <vector>

#include <termios.h>

#include <asctec_comm/raw_buffer.h>

namespace asctec_comm
{

class Uart : public RawBuffer
{
public:
  Uart();
  virtual ~Uart();

  /**
   * \brief  Connects to the specified serial port with the given baudrate.
   *
   * @param port port to use
   * @param baudrate baudrate to connect with
   * @return connection successful
   */
  bool connect(const std::string & port, int baudrate);

  /// Closes the serial port(s).
  void closePort();

  virtual int writeBuffer(uint8_t* data, int size)
  {
    if(fd_ < 0)
      return 0;

    return ::write(fd_, data, size);
  }

  virtual int readBuffer(uint8_t* data, int size)
  {
    if(fd_ < 0)
      return 0;

    return ::read(fd_, data, size);
  }

private:
  bool getBestBaudrateConstant(const int baudrate, int* baudConst);

  int fd_;
  termios oldConfig_;
};

typedef std::shared_ptr<Uart> UartPtr;

} // end namespace asctec_comm
