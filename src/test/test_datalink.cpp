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
#include <asctec_comm/datalink.h>

#include "loopback.h"

using namespace trinity_comm;
using namespace std::chrono;

int nRuns = 100;
constexpr int payloadSize = 100;
bool shutdown;

struct Data
{
  std::chrono::system_clock::time_point timeSent;
  std::chrono::system_clock::time_point timeReceived;
  int seq;
  uint8_t payload[payloadSize];
};

typedef std::vector<uint8_t> Packet;

std::shared_ptr<RawBuffer> comm;
std::shared_ptr<DataLink> transport;

std::vector<Data> testDataIn, testDataOut;

void sender()
{
  TRINITY_INFO_STREAM("data size " << sizeof(Data));
  for(auto& data : testDataOut)
  {
    data.timeSent = system_clock::now();
    Packet dataSent((uint8_t*)&data, (uint8_t*)&data + sizeof(Data));
    transport->sendFrame(dataSent);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void receiver()
{
  Packet dataRec;
  Data data;
  while(!shutdown)
  {
    bool success = transport->waitForFrame(milliseconds(10), &dataRec);
    if(success)
    {
      memcpy(&data, dataRec.data(), sizeof(Data));
      data.timeReceived = system_clock::now();
      testDataIn.push_back(data);
    }
    else
    {
      // Timeout
    }
  }
}

TEST(trinity_comm, datalink)
{
  srand(123456);

  shutdown = false;

  testDataOut.reserve(nRuns);
  testDataIn.reserve(nRuns);

  for(int i = 0; i < nRuns; ++i)
  {
    Data data;
    data.seq = i;
    for(auto& c : data.payload)
    {
      c = rand();
    }
    testDataOut.push_back(data);
  }

  std::thread _sender(&sender);
  std::thread _receiver(&receiver);

  _sender.join();
  std::this_thread::sleep_for(milliseconds(10) * nRuns);
  shutdown = true;
  _receiver.join();

  EXPECT_EQ(testDataOut.size(), testDataIn.size()) << " we lost packets ";

  // testDataIn should be in order
  int lastSeq = 0;
  for(auto& d : testDataIn)
  {
    EXPECT_GE(d.seq, lastSeq) << " packets out of order";
    lastSeq = d.seq;
  }

  for(auto& dOut : testDataOut)
  {
    bool found = false;
    Data currentDin;
    for(auto& dIn : testDataIn)
    {
      if(dIn.seq == dOut.seq)
      {
        found = true;
        currentDin = dIn;
        break;
      }
    }
    EXPECT_TRUE(found) << " sent packet with seq " << dOut.seq << " not found in received data";
    bool dataEqual = true;
    for(int i = 0; i < sizeof(dOut.payload); ++i)
    {
      if(dOut.payload[i] != currentDin.payload[i])
      {
        dataEqual = false;
        break;
      }
    }
    EXPECT_TRUE(dataEqual) << " data error";
  }

  double count = 0.0;
  double dtBuf = 0.0;
  for(auto& dIn : testDataIn)
  {
    dtBuf += duration_cast<std::chrono::duration<double>>(dIn.timeReceived - dIn.timeSent).count();
    count += 1.0;
  }
  TRINITY_INFO_STREAM("Avg transport time: " << dtBuf / count);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

  // Looping back over memory.
  comm.reset(new Loopback);

  // If connected to serial port with rx and tx connected
  //  UartPtr uart;
  //  uart->connect("/dev/ttyUSB0", 460800);
  //  comm = uart;

  transport.reset(new DataLink(comm, 10, 10));

  return RUN_ALL_TESTS();
}
