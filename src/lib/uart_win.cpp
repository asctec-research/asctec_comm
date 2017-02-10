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

#include <asctec_comm/uart_win.h>
//#include <asctec_comm/macros.h>

#include <sstream>
#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace asctec_comm
{
  Uart::Uart()
    :readTimeout_(1000), writeTimeout_(1000), hCom_(0)
  {
  }

  Uart::~Uart()
  {
    if (hCom_ != 0)
      CloseHandle(hCom_);
  }

  bool Uart::connect(const std::string & port, int baudrate)
  {
    using namespace std;

    stringstream ss;
    ss << "\\\\.\\" << port;

    hCom_ = CreateFile(ss.str().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
    if (hCom_ == INVALID_HANDLE_VALUE)
    {
      cerr << "Could not open port " << port << endl;
      return false;
    }

    DCB dcbConfig;
    if (GetCommState(hCom_, &dcbConfig))
    {
      dcbConfig.BaudRate = baudrate;
      dcbConfig.ByteSize = 8;
      dcbConfig.Parity = NOPARITY;
      dcbConfig.StopBits = ONESTOPBIT;
      dcbConfig.fBinary = TRUE;
    }
    else
    {
      cerr << "GetCommState error\n";
      return false;
    }

    if (!SetCommState(hCom_, &dcbConfig))
    {
      cerr << "SetCommState error\n";
      return false;
    }

    setTimeouts(readTimeout_, writeTimeout_);  // no timeout

    PurgeComm(hCom_, PURGE_RXCLEAR | PURGE_RXABORT);	// purge old data from input buffer

    return true;
  }

  void Uart::setReadTimeout(uint32_t timeout)
  {
    setTimeouts(timeout, writeTimeout_);
  }

  void Uart::setWriteTimeout(uint32_t timeout)
  {
    setTimeouts(readTimeout_, timeout);
  }

  void Uart::setTimeouts(uint32_t read, uint32_t write)
  {
    readTimeout_ = read;
    writeTimeout_ = write;

    COMMTIMEOUTS commTimeout;
    GetCommTimeouts(hCom_, &commTimeout);
    commTimeout.ReadIntervalTimeout = MAXDWORD;
    commTimeout.ReadTotalTimeoutMultiplier = MAXDWORD;
    commTimeout.ReadTotalTimeoutConstant = read;
    commTimeout.WriteTotalTimeoutMultiplier = 0;
    commTimeout.WriteTotalTimeoutConstant = write;

    SetCommTimeouts(hCom_, &commTimeout);
  }

  int Uart::readBuffer(uint8_t* pData, int size)
  {
    DWORD bytesRead;
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    ReadFile(hCom_, pData, size, NULL, &ov);

    if (!GetOverlappedResult(hCom_, &ov, &bytesRead, TRUE))
    {
      int32_t err = GetLastError();

      std::cerr << "ReadFile error: " << err << std::endl;
      return -1;
    }

    return bytesRead;
  }

  int Uart::writeBuffer(uint8_t* pData, int size)
  {
    DWORD bytesWritten;
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(OVERLAPPED));
    ov.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    WriteFile(hCom_, pData, size, NULL, &ov);

    if (!GetOverlappedResult(hCom_, &ov, &bytesWritten, TRUE))
    {
      int32_t err = GetLastError();

      std::cerr << "WriteFile error: " << err << std::endl;
      return -1;
    }

    return bytesWritten;
  }
}