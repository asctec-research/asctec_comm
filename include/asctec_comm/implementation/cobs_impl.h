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

#include <asctec_comm/cobs.h>

namespace asctec_comm {

namespace cobs {

namespace StuffingCode {
enum
{
  Unused = 0x00,        // Unused (framing character placeholder)

  DiffZero = 0x01,      // Range 0x01 - 0xD1:
  DiffZeroMax = 0xD1,   // n-1 explicit characters plus a zero
  Diff = 0xD2,          // 209 explicit characters, no added zero

  RunZero = 0xD3,       // Range 0xD3 - 0xDF:
  RunZeroMax = 0xDF,    // 3-15 zeroes (Zero Run Encoding, ZRE)

  Diff2Zero = 0xE0,     // Range 0xE0 - 0xFF:
  Diff2ZeroMax = 0xFF,  // 0-31 explicit characters plus 2 zeroes (Zero Pair Encoding, ZPE)
};
}

// Check if the code indicates ZPE
constexpr bool isDiff2Zero(uint8_t x)
{
  return (x >= StuffingCode::Diff2Zero);
}

// Check if the code indicates ZRE
constexpr bool isRunZero(uint8_t x)
{
  return (x >= StuffingCode::RunZero && x <= StuffingCode::RunZeroMax);
}

// Convert from single-zero code to corresponding double-zero code
static constexpr uint8_t convertZP = (StuffingCode::Diff2Zero - StuffingCode::DiffZero);

// Highest single-zero code with a corresponding double-zero code
static constexpr uint8_t maxConvertible = (StuffingCode::Diff2ZeroMax - convertZP);

inline void Encoder::finalizeBlock(uint8_t finalCode)
{
  out_[codeIndex_] = finalCode;  // save code to this' block code position
  codeIndex_ = out_.size();  // store code position for new block
  out_.push_back(0);
}

inline void Encoder::feed(uint16_t in)
{
  const uint8_t* ptr = (const uint8_t*) &in;
  feed(*ptr);
  ++ptr;
  feed(*ptr);
}

inline void Encoder::feed(const ByteVector& in)
{
  for (auto& c : in)
  {
    feed(c);
  }
}

inline Encoder& Encoder::operator<<(const ByteVector& in)
{
  feed(in);
  return *this;
}

inline Encoder& Encoder::operator<<(uint8_t in)
{
  feed(in);
  return *this;
}

inline Encoder& Encoder::operator<<(uint16_t in)
{
  feed(in);
  return *this;
}

inline ByteVector Encoder::getResult()
{
  out_[codeIndex_] = code_;
  return out_;
}

template<class Iterator>
ByteVector
decode(const Iterator& first, const Iterator& last)
{
  std::vector < uint8_t > out;

  Iterator it = first;

  while(it < last)
  {
    int32_t z, c = *it++;  // c = code, z = zeros

    if(c == StuffingCode::Diff)
    {
      z = 0;
      c--;
    }
    else if(isRunZero (c))
    {
      z = c & 0xF;
      c = 0;
    }
    else if(isDiff2Zero (c))
    {
      z = 2;
      c &= 0x1F;
    }
    else
    {
      z = 1;
      c--;
    }

    while(--c >= 0)
    {
      out.push_back (*it);
      ++it;
    }

    while(--z >= 0)
    {
      out.push_back (0);
    }
  }

  if(!out.empty ())
  {
    out.pop_back ();
  }

  return out;
}

}  // end namespace cobs
}  // end namespace asctec_comm
