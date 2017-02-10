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

#include <stdlib.h>
#include <random>

#include <gtest/gtest.h>

#include <asctec_comm/cobs.h>

using namespace trinity_comm;

// original decode function
namespace trinity_comm
{
namespace cobs
{

ByteVector decode(const ByteVector& in)
{
  ByteVector out;

  const uint8_t* pIn = in.data();
  const uint8_t* pEnd = in.data() + in.size();

  while(pIn < pEnd)
  {
    int32_t z, c = *pIn++;  // c = code, z = zeros

    if(c == StuffingCode::Diff)
    {
      z = 0;
      c--;
    }
    else if(isRunZero(c))
    {
      z = c & 0xF;
      c = 0;
    }
    else if(isDiff2Zero(c))
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
      out.push_back(*pIn);
      ++pIn;
    }

    while(--z >= 0)
    {
      out.push_back(0);
    }
  }

  out.resize(out.size() - 1);

  return out;
}

}
}

TEST(trinity_comm, cobs)
{
  srand(123456);
  int maxSize = 100;
  int nRuns = 100;

  for (int run = 0; run < nRuns; ++run)
  {
    ByteVector dataIn;
    dataIn.resize(rand()%maxSize);

    for (auto& i : dataIn)
    {
      i = rand();
    }

    cobs::Encoder enc;
    enc << dataIn;

    ByteVector dataEnc = enc.getResult();
    ByteVector dataEncMod;
    dataEncMod.push_back(rand());
    dataEncMod.push_back(rand());
    dataEncMod.push_back(rand());
    dataEncMod.insert(dataEncMod.end(), dataEnc.begin(), dataEnc.end());

    ByteVector dataOut1 = cobs::decode(dataEnc);
    ByteVector dataOut2 = cobs::decode(dataEnc.begin(), dataEnc.end());
    ByteVector dataOut3 = cobs::decode(dataEncMod.begin()+3, dataEncMod.end());

    EXPECT_EQ(dataOut1.size(), dataOut2.size());
    EXPECT_EQ(dataOut1.size(), dataOut3.size());
    EXPECT_EQ(dataOut1.size(), dataIn.size());

    for (int i = 0; i < dataOut1.size(); ++i)
    {
      EXPECT_EQ(dataOut1[i], dataOut2[i]) << "at index " << i;
      EXPECT_EQ(dataOut1[i], dataOut3[i]) << "at index " << i;
      EXPECT_EQ(dataOut1[i], dataIn[i]) << "at index " << i;
    }
  }
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
