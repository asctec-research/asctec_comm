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

#include <algorithm>
#include <deque>

#include <asctec_comm/datalink.h>
#include <asctec_comm/cobs.h>
#include <asctec_comm/macros.h>

namespace asctec_comm
{

DataLink::DataLink(RawBufferPtr rawBuffer)
    : sendSequence_(0), receiveSequence_(0), nFramesSent_(0), nFramesSentSkipped_(0), nFramesReceived_(0), nFramesReceivedMissed_(),
      nFramesReceivedCrcError_(0), receiveProcessingBufferPos_(0)
{
  if(!rawBuffer)
  {
    ASCTEC_ERROR_STREAM("Rawbuffer is a nullptr, exiting");
    exit (EXIT_FAILURE);
  }

  rawBuffer_ = rawBuffer;
}

DataLink::~DataLink()
{
}

void DataLink::sendFrame(const ByteVector& frame)
{
  constexpr uint8_t separator = 0;

  uint16_t crc = 0xffff;
  crc = trinity_msgs::crc16(frame.cbegin(), frame.cend(), crc);

  cobs::Encoder encoder;
  encoder << frame;
  encoder << sendSequence_;
  encoder << trinity_msgs::crc16((uint8_t*)&sendSequence_, (uint8_t*)&sendSequence_ + 2, crc);

  ByteVector buffer = encoder.getResult();
  buffer.push_back(separator);

  rawBuffer_->writeBuffer(buffer.data(), buffer.size());

  ++sendSequence_;
}

void DataLink::pollFramesUnBuffered(std::vector<ByteVector>* frames)
{
  if(!frames)
  {
    ASCTEC_ERROR_STREAM("hey, don't pass nullptrs!!");
    return;
  }
  frames->clear();

  int bytesRead = rawBuffer_->readBuffer(receiveBuffer_, kReceiveBufferSize_);

  if(bytesRead < 1)
  {
    return;
  }

  int receiveBufferPos = 0;

  for(; receiveBufferPos < bytesRead; ++receiveBufferPos)
  {
    const uint8_t data = receiveBuffer_[receiveBufferPos];
    if(data != 0)
    {
      receiveProcessingBuffer_[receiveProcessingBufferPos_] = data;
      ++receiveProcessingBufferPos_;

      if(receiveProcessingBufferPos_ >= kReceiveBufferSize_)
      {
        receiveProcessingBufferPos_ = 0;
        ASCTEC_WARN_STREAM("receive buffer overflow");
        return;
      }
    }
    else
    {
      if(receiveProcessingBufferPos_ == 0)
      {
        continue;
      }

      ByteVector decodedStream = cobs::decode(&receiveProcessingBuffer_[0],
          &receiveProcessingBuffer_[receiveProcessingBufferPos_]);
      receiveProcessingBufferPos_ = 0;

      if(decodedStream.size() >= 4)
      {
        // Decompose into seq, data, crc, and check
        const uint16_t seq = *reinterpret_cast<const uint16_t*>(&(*(decodedStream.cend() - 4)));
        const uint16_t crc = *reinterpret_cast<const uint16_t*>(&(*(decodedStream.cend() - 2)));
        const uint16_t crcMsg = trinity_msgs::crc16(decodedStream.cbegin(), decodedStream.cend() - 2, 0xffff);

        if(crc == crcMsg)
        {
          frames->push_back(ByteVector(decodedStream.cbegin(), decodedStream.cend() - 4));
        }
        else
        {
          ASCTEC_ERROR_STREAM("crc failed. crc=" << crc << " crc computed=" << crcMsg);
          ++nFramesReceivedCrcError_;
        }
      }
      else
      {
        ASCTEC_ERROR_STREAM("Encoded message size < 4, this should not happen");
      }
    }
  }
}

}  //end namespace asctec_comm
