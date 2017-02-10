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

#include <asctec_comm/cobs.h>

namespace asctec_comm
{
namespace cobs
{

Encoder::Encoder()
{
  reset();
}

constexpr size_t Encoder::getMaxEncodedSize(size_t inSize)
{
  return ((inSize) + (inSize) / 208 + 1);
}

Encoder& Encoder::reset()
{
  out_.clear();

  code_ = StuffingCode::DiffZero;
  out_.push_back(code_);
  codeIndex_ = 0;

  return *this;
}

void Encoder::feed(uint8_t c)
{
  if(c == 0) // If it's a zero, do one of these operations
  {
    if(isRunZero(code_) && code_ < StuffingCode::RunZeroMax) // If in ZRE mode and below max zero count
    {
      code_++;	// increment ZRE count
    }
    else if(code_ == StuffingCode::Diff2Zero)	// If in two zeros and no character state
    {
      code_ = StuffingCode::RunZero;	// switch to ZRE state
    }
    else if(code_ <= maxConvertible) // If in diffZero state and below max char count for ZPE:
    {
      code_ += convertZP; // switch to ZPE mode
    }
    else // cannot convert to ZPE (>31 chars) or above max ZRE (>15 '0's)
    {
      finalizeBlock(code_);
      code_ = StuffingCode::DiffZero;	// start new block by single encoded zero
    }
  }
  else // else, non-zero; do one of these operations
  {
    if(isDiff2Zero(code_))
    {
      finalizeBlock(code_ - convertZP);
      code_ = StuffingCode::DiffZero;
    }
    else if(code_ == StuffingCode::RunZero)
    {
      finalizeBlock(StuffingCode::Diff2Zero);
      code_ = StuffingCode::DiffZero;
    }
    else if(isRunZero(code_))
    {
      finalizeBlock(code_ - 1);
      code_ = StuffingCode::DiffZero;
    }

    out_.push_back(c);

    if(++code_ == StuffingCode::Diff)
    {
      finalizeBlock(code_);
      code_ = StuffingCode::DiffZero;
    }
  }
}

} // end namespace cobs
} // end namespace asctec_comm
