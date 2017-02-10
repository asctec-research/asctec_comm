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

#include "loopback.h"

namespace trinity_comm
{

Loopback::Loopback()
    : RawBuffer(), totalRead_(0), totalSent_(0), queue_(100000)
{
}

int Loopback::writeBuffer(uint8_t* data, int size)
{
  return writeBufferImpl(data, size);
}

int Loopback::readBuffer(uint8_t* data, int size)
{
  return readBufferImpl(data, size);
}

int Loopback::writeBufferImpl(uint8_t* data, int size)
{
  queue_.push(data, data + size);
  totalSent_ += size;

  return size;
}

int Loopback::readBufferImpl(uint8_t* data, int size)
{
  // Add some fun randomness about how many bytes we actually read :)
  int bytesToRead = 1;
  if(size > 1)
  {
    bytesToRead = rand() % size;
  }

  int i;
  for(i = 0; i < bytesToRead; ++i)
  {
    bool success = queue_.popWithTimeout(std::chrono::milliseconds(1), &data[i]);
    if(!success)
    {
      return i;
    }
    totalRead_++;
  }
  return bytesToRead;
}

LoopbackBridge::LoopbackBridge()
{
  rxTxLoopback_.reset(new DoubleLoopBack(rxTx_, txRx_));
  txRxLoopback_.reset(new DoubleLoopBack(txRx_, rxTx_));
}

}  // end namespace trinity_comm
