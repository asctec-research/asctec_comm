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

#include <memory>

namespace asctec_comm
{

/// Interface to a raw buffer.
class RawBuffer
{
public:
  virtual ~RawBuffer()
  {
  }

  /// Write to buffer. Has to be thread-safe.
  virtual int writeBuffer(uint8_t* data, int size) = 0;

  /// Reads from buffer, should be blocking with timeout.
  virtual int readBuffer(uint8_t* data, int size) = 0;
};

typedef std::shared_ptr<RawBuffer> RawBufferPtr;

}
