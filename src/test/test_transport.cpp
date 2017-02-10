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

#include <deque>
#include <stdlib.h>
#include <random>

#include <gtest/gtest.h>

#include <asctec_comm/raw_buffer.h>
#include <asctec_comm/transport.h>

#include "loopback.h"

using namespace trinity_comm;
using namespace std::chrono;

int nRuns = 100;
constexpr int payloadSize = 100;

struct Data
{
  std::chrono::system_clock::time_point timeSent;
  std::chrono::system_clock::time_point timeReceived;
  int seq;
  uint8_t payload[payloadSize];
};

TEST(trinity_comm, Transport_loopback_bridge)
{
  LoopbackBridge localBridge;

  constexpr int size = 50;
  uint8_t dataOut[size];
  uint8_t dataIn[size];
  for(int i = 0; i < size; ++i)
  {
    dataOut[i] = i;
  }

  for(int nRun = 0; nRun < nRuns; ++nRun)
  {

    localBridge.rxTxLoopback_->writeBuffer(dataOut, size);
    localBridge.txRxLoopback_->writeBuffer(dataOut, size);

    int bytesRead = 0;
    int nTries = 0;
    return;

    while(bytesRead < size && nTries < size)
    {
      bytesRead += localBridge.txRxLoopback_->readBuffer(dataIn, size);
      nTries++;
    }

    EXPECT_EQ(bytesRead, size) << " --> tried " << nTries << " times";

    for(int i = 0; i < size; ++i)
    {
      EXPECT_EQ(dataOut[i], dataIn[i]);
    }

    bytesRead = 0;
    nTries = 0;
    while(bytesRead < size && nTries < size * 500)
    {
      bytesRead += localBridge.rxTxLoopback_->readBuffer(dataIn, size);
      nTries++;
    }

    EXPECT_EQ(bytesRead, size) << " --> tried " << nTries << " times";

    for(int i = 0; i < size; ++i)
    {
      EXPECT_EQ(dataOut[i], dataIn[i]);
    }
  }
}

class SendReceiveTest
{
public:
  SendReceiveTest()
      : shutdown_(false)
  {
    dlDevice_.reset(new DataLink(bridge_.rxTxLoopback_, 100, 100));
    dlPc_.reset(new DataLink(bridge_.txRxLoopback_, 100, 100));
    device_.reset(new Transport(dlDevice_));
    pc_.reset(new Transport(dlPc_));
  }

  std::shared_ptr<Transport> device_, pc_;
  std::shared_ptr<DataLink> dlDevice_, dlPc_;
  LoopbackBridge bridge_;
  bool shutdown_;
};

TEST(trinity_comm, Transport_test_ack)
{
  SendReceiveTest test;

  Data data;
  for(auto d : data.payload)
  {
    d = rand();
  }
  ByteVector datagram((uint8_t*)&data, (uint8_t*)&data + sizeof(Data));

  for(int i = 0; i < nRuns; ++i)
  {
    for(auto d : data.payload)
    {
      d = rand();
    }
    EXPECT_TRUE(test.pc_->sendDataAcknowledged(milliseconds(100), 1, datagram));
    EXPECT_TRUE(test.device_->sendDataAcknowledged(milliseconds(100), 1, datagram));
  }
}

TEST(trinity_comm, Transport_test_ack_fun_with_threads)
{
  SendReceiveTest test;

  struct ThreadObj
  {
    ThreadObj(Transport& tp, int id)
        : tp_(tp), id_(id)
    {
    }

    void worker()
    {
      Data data;
      for(auto d : data.payload)
      {
        d = rand();
      }
      ByteVector _data((uint8_t*)&data, (uint8_t*)&data + sizeof(Data));
      EXPECT_TRUE(tp_.sendDataAcknowledged(milliseconds(1000), 1, _data)) << " in thread " << id_;
    }
    Transport& tp_;
    int id_;
  };

  constexpr int nThreads = 99;
  std::thread threads[nThreads];
  ThreadObj* threadObjs[nThreads];
  for(int i = 0; i < nThreads; ++i)
  {
    threadObjs[i] = new ThreadObj(*(test.pc_), i);
    threads[i] = std::thread(&ThreadObj::worker, threadObjs[i]);
  }

  for(int i = 0; i < nThreads; ++i)
  {
    threads[i].join();
    delete threadObjs[i];
  }
}

int main(int argc, char **argv)
{
  srand(12345678);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
