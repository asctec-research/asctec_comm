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

#include <asctec_comm/transport.h>
#include <asctec_uav_msgs/transport_definitions.h>

namespace asctec_comm
{

Transport::Transport(std::shared_ptr<DataLink> dataLink)
    : sendQueue_(100), receiveQueue_(100), nextAckId_(0), shutdownRequested_(false)
{
  if(!dataLink)
  {
    ASCTEC_ERROR_STREAM("dataLink is a nullptr, exiting");
    exit (EXIT_FAILURE);
  }

  dataLink_ = dataLink;
  sendThread_ = std::thread(&Transport::sendThread, this);
  receiveThread_ = std::thread(&Transport::receiveThread, this);
}

Transport::~Transport()
{
  shutdownRequested_ = true;

  if(sendThread_.joinable())
  {
    sendThread_.join();
  }

  if(receiveThread_.joinable())
  {
    receiveThread_.join();
  }
}

void Transport::sendThread()
{
  while(!shutdownRequested_)
  {
    ByteVector frame;
    bool success = sendQueue_.popWithTimeout(std::chrono::milliseconds(100), &frame);

    if(!success)
    {
      continue;
    }

    dataLink_->sendFrame(frame);
  }
}

void Transport::receiveThread()
{
  while(!shutdownRequested_)
  {
    std::vector<ByteVector> frames;
    dataLink_->pollFramesUnBuffered(&frames);
    if(frames.empty())
    {
      continue;
    }

    for(auto& frame : frames)
    {
      if(frame.size() < POS_DATAGRAM)
      {
        ASCTEC_WARN_STREAM("Frame size smaller than header size. This should not happen.");
        continue;
      }

      uint16_t flags, ackId;
      Datagram datagram;
      deserialize(frame, &(datagram.id_), &flags, &ackId, &(datagram.data_));

      if(flags & asctec_uav_msgs::TRANSPORT_FLAG_ACK_REQUEST)
      {
        ByteVector frame, dummy;
        serialize(datagram.id_, asctec_uav_msgs::TRANSPORT_FLAG_ACK_RESPONSE, ackId, dummy.begin(), dummy.end(), &frame);
        sendQueue_.push(frame);
      }
      else if(flags & asctec_uav_msgs::TRANSPORT_FLAG_ACK_RESPONSE)
      {
        UniqueLock lock(ackMutex_);
        pendingAcks_.insert(ackId);
        lock.unlock();
        ackCondition_.notify_all();
      }
      else
      {
        receiveQueue_.push(datagram);
      }
    }
  }
}

void Transport::deserialize(const ByteVector& frame, int32_t* id, uint16_t* flags, uint16_t* ackId,
    ByteVector* datagram)
{
  if(id)
  {
    *id = *reinterpret_cast<const uint32_t*>(frame.data() + POS_ID);
  }

  if(flags)
  {
    *flags = *reinterpret_cast<const uint16_t*>(frame.data() + POS_FLAGS);
  }

  if(ackId)
  {
    *ackId = *reinterpret_cast<const uint16_t*>(frame.data() + POS_ACK_ID);
  }

  if(datagram)
  {
    datagram->clear();
    datagram->insert(datagram->begin(), frame.cbegin() + POS_DATAGRAM, frame.cend());
  }
}

}  // end namespace asctec_comm
