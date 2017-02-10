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

#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <asctec_comm/types.h>

namespace asctec_comm
{
namespace cobs
{

class Encoder
{
public:
  Encoder();

  /** Reset the encoder of *this. */
  Encoder& reset();

  /** Feed the encoder with a single byte */
  void feed(uint8_t c);

  /** Feed the encoder with a unsigned short */
  void feed(uint16_t in);

  /** Feed the encoder with a vector of bytes */
  void feed(const ByteVector& in);

  /** Streaming operator to feed in data */
  Encoder& operator<<(const ByteVector& in);

  /** Streaming operator to feed in data */
  Encoder& operator<<(uint8_t in);

  /** Streaming operator to feed in data */
  Encoder& operator<<(uint16_t in);

  /** Get the encoding result */
  ByteVector getResult();

  /** Get the worst-case encoded message size of a message of size "inSize" */
  static constexpr size_t getMaxEncodedSize(size_t inSize);

private:
  void finalizeBlock(uint8_t finalCode);

  ByteVector out_;
  uint8_t code_;
  size_t codeIndex_;
};

/// Decode everything at once.
template<class Iterator>
ByteVector decode(const Iterator& first, const Iterator& last);

} // end namespace cobs
} // end namespace asctec_comm

#include <asctec_comm/implementation/cobs_impl.h>
