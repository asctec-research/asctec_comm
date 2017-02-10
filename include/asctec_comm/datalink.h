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

#include <thread>
#include <vector>

#include <asctec_uav_msgs/crc16.h>

#include <asctec_comm/cobs.h>
#include <asctec_comm/raw_buffer.h>
#include <asctec_comm/thread_safe_queue.h>
#include <asctec_comm/types.h>

namespace asctec_comm
{

/**
 * \brief Datalink abstraction.
 * Takes care of sending and receiving arbitrary data-frames with a checksum and sequence number.
 */
class DataLink
{
public:
  DataLink(RawBufferPtr rawBuffer);
  ~DataLink();

  /**
   * \brief Sends a frame over the serial port.
   */
  void sendFrame(const ByteVector& frame);

  /**
   * \brief Performs a non blocking read on the raw-buffer (e.g.) serial port, and writes completed frames to frames.
   */
  void pollFramesUnBuffered(std::vector<ByteVector>* frames);

private:

  RawBufferPtr rawBuffer_;

  uint16_t sendSequence_;
  uint16_t receiveSequence_;

  int nFramesSent_;
  int nFramesSentSkipped_;
  int nFramesReceived_;
  int nFramesReceivedMissed_;
  int nFramesReceivedCrcError_;

  static constexpr int kReceiveBufferSize_ = 2048;
  uint8_t receiveBuffer_[kReceiveBufferSize_];
  uint8_t receiveProcessingBuffer_[kReceiveBufferSize_];
  int receiveProcessingBufferPos_;
};

typedef std::shared_ptr<asctec_comm::DataLink> DataLinkPtr;

}  // end namespace asctec_comm

