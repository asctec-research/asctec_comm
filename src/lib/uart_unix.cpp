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

// Standard includes
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <inttypes.h>
#include <fstream>

// Serial includes
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#ifdef __linux
#include <sys/ioctl.h>
#endif

#include <asctec_comm/macros.h>
#include <asctec_comm/uart.h>

#ifdef __APPLE__

#ifndef B460800
#define B460800 460800
#endif

#ifndef B921600
#define B921600 921600
#endif

#endif

namespace asctec_comm
{

Uart::Uart()
    : fd_(-1)
{
  bzero(&oldConfig_, sizeof(oldConfig_));
}

Uart::~Uart()
{
  this->closePort();
}

bool Uart::connect(const std::string& portName, int baudrate)
{
  //reset values
  fd_ = -1;

  int baudrateConstant;
  if(!getBestBaudrateConstant(baudrate, &baudrateConstant))
    return false;

  //open port
  fd_ = ::open(portName.c_str(), O_RDWR | O_NOCTTY);
  if(fd_ == -1)
  {
    ASCTEC_ERROR_STREAM("Error while opening serial port " << portName << ": " << strerror(errno));
    return false;
  }

  int ret = -1;

  ret = tcgetattr(fd_, &oldConfig_); // save current port settings
  if(ret == -1)
  {
    ASCTEC_ERROR_STREAM("Error during tcgetattr: " << strerror(errno));
    return false;
  }

  if(!isatty(fd_))
  {
    ASCTEC_ERROR_STREAM("File descriptor "<< fd_ <<" is NOT a serial port\n");
    return false;
  }

  struct termios newtio;  //structure for port settings
  bzero(&newtio, sizeof(newtio));
  //set data connection to 8N1, no flow control
  newtio.c_cflag = baudrateConstant | CS8 | CREAD | CLOCAL;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  // set input mode (non-canonical, no echo,...)
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = 1; // inter-character timer used, in 1/10 seconds
  newtio.c_cc[VMIN] = 0; // blocking read

  tcflush(fd_, TCIFLUSH);
  ret = tcsetattr(fd_, TCSANOW, &newtio);
  if(ret == -1)
  {
    ASCTEC_ERROR_STREAM("Error during tcsetattr: " << strerror(errno));
    return false;
  }

  ASCTEC_INFO_STREAM("successfully opened " << portName << " with baudrate define " << baudrate << ". Fd is " << fd_);

  return true;
}

void Uart::closePort()
{
  if(fd_ != -1)
  {
    ::close(fd_);
  }

  tcsetattr(fd_, TCSANOW, &oldConfig_);
}

bool Uart::getBestBaudrateConstant(const int baudrate, int* baudConst)
{
  int baudrates[] = { 9600, 14400, 19200, 38400, 57600, 115200, 230400, 460800, 921600 };
  int best_baudrate = 57600;
  int min_diff = 1e6;

  for(int i = 0; i < sizeof(baudrates) / sizeof(int); i++)
  {
    int diff = abs(baudrates[i] - baudrate);
    if(diff < min_diff)
    {
      min_diff = diff;
      best_baudrate = baudrates[i];
    }
  }

  if(best_baudrate != baudrate)
    ASCTEC_WARN_STREAM("Unsupported baudrate, choosing closest supported baudrate: " << best_baudrate);

  switch(best_baudrate)
  {
    case 1200:
      *baudConst = B1200;
      break;
    case 1800:
      *baudConst = B1800;
      break;
    case 9600:
      *baudConst = B9600;
      break;
    case 19200:
      *baudConst = B19200;
      break;
    case 38400:
      *baudConst = B38400;
      break;
    case 57600:
      *baudConst = B57600;
      break;
    case 115200:
      *baudConst = B115200;
      break;
    case 460800:
      *baudConst = B460800;
      break;
    case 921600:
      *baudConst = B921600;
      break;
    default:
      ASCTEC_ERROR_STREAM("" << best_baudrate << " does not match any baudrate available.");
      return false;
  }

  return true;
}

} // end namespace asctec_comm
