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

#ifndef SRC_TEST_LOOPBACK_H_
#define SRC_TEST_LOOPBACK_H_

#include <asctec_comm/raw_buffer.h>
#include <asctec_comm/thread_safe_queue.h>

namespace trinity_comm
{

class Loopback : public RawBuffer
{
public:
  Loopback();

  virtual int writeBuffer(uint8_t* data, int size);
  virtual int readBuffer(uint8_t* data, int size);

  int writeBufferImpl(uint8_t* data, int size);
  int readBufferImpl(uint8_t* data, int size);
  int totalRead_, totalSent_;
private:
  ThreadSafeQueue<uint8_t> queue_;
};

class LoopbackBridge
{
public:
  LoopbackBridge();
  class DoubleLoopBack : public RawBuffer
  {
  public:
    DoubleLoopBack(Loopback& _1, Loopback& _2)
        : RawBuffer(), rx_(_1), tx_(_2)
    {
    }

    virtual int writeBuffer(uint8_t* data, int size)
    {
      return tx_.writeBufferImpl(data, size);
    }

    virtual int readBuffer(uint8_t* data, int size)
    {
      return rx_.readBufferImpl(data, size);
    }

  private:
    Loopback& rx_;
    Loopback& tx_;
  };
  std::shared_ptr<DoubleLoopBack> rxTxLoopback_;
  std::shared_ptr<DoubleLoopBack> txRxLoopback_;
private:
  Loopback rxTx_;
  Loopback txRx_;
};

}  // end namespace trinity_comm
#endif /* SRC_TEST_LOOPBACK_H_ */
