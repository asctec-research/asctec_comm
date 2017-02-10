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

#include <chrono>
#include <thread>
#include <unordered_set>

#include <asctec_comm/datalink.h>
#include <asctec_comm/thread_safe_queue.h>
#include <asctec_comm/types.h>
#include <asctec_uav_msgs/transport_definitions.h>

namespace asctec_comm
{

class Transport
{
public:
  Transport(std::shared_ptr<DataLink> dataLink);
  ~Transport();

  template<class Iterator>
  void sendData(uint32_t id, const Iterator& first, const Iterator& last);

  void sendData(uint32_t id, const ByteVector& data);

  template<class Data>
  void sendData(uint32_t id, const Data& data);

  template<class Rep, class Period, class Iterator>
  bool sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id, const Iterator& first,
      const Iterator& last);

  template<class Rep, class Period>
  bool sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id, const ByteVector& data);

  template<class Rep, class Period, class Data>
  bool sendDataAcknowledged(const std::chrono::duration<Rep, Period>& timeout, uint32_t id, const Data& data);

  template<class Rep, class Period>
  bool waitForData(const std::chrono::duration<Rep, Period>& timeout, uint32_t* id, ByteVector* data);

private:
  enum
  {
    POS_ID = 0, POS_FLAGS = 4, POS_ACK_ID = 6, POS_DATAGRAM = 8
  };

  struct Datagram
  {
    int32_t id_;
    ByteVector data_;
  };

  typedef std::unique_lock<std::mutex> UniqueLock;

  ThreadSafeQueue<ByteVector> sendQueue_;
  ThreadSafeQueue<Datagram> receiveQueue_;

  std::shared_ptr<DataLink> dataLink_;

  std::thread sendThread_;
  std::thread receiveThread_;

  std::mutex ackMutex_;
  std::condition_variable ackCondition_;
  std::unordered_set<uint16_t> pendingAcks_;
  uint16_t nextAckId_;

  bool shutdownRequested_;

  void sendThread();
  void receiveThread();

  template<class Iterator>
  static void serialize(uint32_t id, uint16_t flags, uint16_t ackId, const Iterator& first, const Iterator& last,
      ByteVector* frame);

  static void deserialize(const ByteVector& frame, int32_t* id, uint16_t* flags, uint16_t* ackId, ByteVector* datagram);
};

typedef std::shared_ptr<asctec_comm::Transport> TransportPtr;

} // end namespace asctec_comm

#include <asctec_comm/implementation/transport_impl.h>


