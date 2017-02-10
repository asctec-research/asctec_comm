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

#include <asctec_comm/transport.h>

namespace asctec_comm
{

template<class Iterator>
void Transport::serialize(uint32_t id, uint16_t flags, uint16_t ackId, const Iterator& first, const Iterator& last,
    ByteVector* frame)
{
  if(frame)
  {
    frame->clear();
    frame->reserve(4 + 2 + 2 + std::distance(first, last));
    frame->insert(frame->begin() + POS_ID, (uint8_t*)&id, ((uint8_t*)&id) + 4);
    frame->insert(frame->begin() + POS_FLAGS, (uint8_t*)&flags, ((uint8_t*)&flags) + 2);
    frame->insert(frame->begin() + POS_ACK_ID, (uint8_t*)&ackId, ((uint8_t*)&ackId) + 2);
    frame->insert(frame->begin() + POS_DATAGRAM, first, last);
  }
}

template<class Iterator>
void Transport::sendData(uint32_t id, const Iterator& first, const Iterator& last)
{
  ByteVector frame;
  serialize(id, 0, 0, first, last, &frame);
  sendQueue_.push(frame);
}

inline void Transport::sendData(uint32_t id, const ByteVector& data)
{
  sendData(id, data.begin(), data.end());
}

template<class Data>
void Transport::sendData(uint32_t id, const Data& data)
{
  sendData(id, reinterpret_cast<const uint8_t*>(&data), reinterpret_cast<const uint8_t*>(&data) + sizeof(Data));
}

template<class Rep, class Period, class Iterator>
bool Transport::sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id,
    const Iterator& first, const Iterator& last)
{
  uint16_t ackId;
  ByteVector frame;
  {
    UniqueLock lock(ackMutex_);
    ackId = nextAckId_;
    ++nextAckId_;
  }
  serialize(id, asctec_uav_msgs::TRANSPORT_FLAG_ACK_REQUEST, ackId, first, last, &frame);
  sendQueue_.push(frame);

  UniqueLock lock(ackMutex_);
  auto timenow = std::chrono::system_clock::now();
  bool success = ackCondition_.wait_until(lock, timeout + timenow, [this, ackId]
  { return this->pendingAcks_.find(ackId) != this->pendingAcks_.end();});
  if(success)
  {
    pendingAcks_.erase(ackId);
  }
  return success;
}

template<class Rep, class Period>
bool Transport::sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id,
    const ByteVector& data)
{
  return sendDataAcknowledged(timeout, id, data.begin(), data.end());
}

template<class Rep, class Period, class Data>
bool Transport::sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id, const Data& data)
{
  return sendDataAcknowledged(timeout, id, reinterpret_cast<const uint8_t*>(&data),
      reinterpret_cast<const uint8_t*>(&data) + sizeof(Data));
}

template<class Rep, class Period>
bool Transport::waitForData(const std::chrono::duration<Rep, Period>& timeout, uint32_t* id, ByteVector* data)
{
  Datagram datagram;
  bool success = receiveQueue_.popWithTimeout(timeout, &datagram);
  if(!success)
  {
    return false;
  }

  if(id)
  {
    *id = datagram.id_;
  }

  if(data)
  {
    data->swap(datagram.data_);  // no copy :)
  }
  return true;
}

}  // end namespace asctec_comm
