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

#include <iostream>
#include <memory>
#include <stdlib.h>
#include <string.h>

#include <asctec_comm/transport.h>
#include <asctec_comm/uart.h>
#include <asctec_uav_msgs/message_definitions.h>
#include <asctec_comm/helper.h>

bool shutdownRequested = false;
std::shared_ptr<asctec_comm::Transport> transport;

// This thread periodically sends roll, pitch, yawRate, thrust commands to the Trinity module
// Rate: 50Hz
void commandThread()
{
  while(!shutdownRequested)
  {
    asctec_uav_msgs::CommandRollPitchYawrateThrust msg;
    msg.roll = 0;
    msg.pitch = 0;
    msg.yawRate = 0;
    msg.thrust = 1.0;

    transport->sendData(asctec_uav_msgs::MESSAGE_ID_COMMAND_ROLL_PITCH_YAWRATE_THRUST, msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

int parseProgramArgs(int argc, char** argv, std::string* pPort, int* pBaudrate)
{
  if (argc <= 1)
  {
    std::cerr << "Please specify serial port via first argument, e.g. /dev/ttyUSB0 or COM1\n";
    return EXIT_FAILURE;
  }

  *pBaudrate = 921600;
  pPort->assign(argv[1]);

  if (argc >= 3)
  {
    *pBaudrate = std::atoi(argv[2]);
  }

  std::cout << "Using serial port: " << *pPort << " with baudrate: " << *pBaudrate << std::endl;

  return 0;
}

int main(int argc, char** argv)
{
  using namespace asctec_comm;

  int baudrate;
  std::string port;

  // get serial port name and optional baudrate from command line arguments
  int result = parseProgramArgs(argc, argv, &port, &baudrate);
  if (result)
    return result;

  // connect to serial device
  transport = helper::createUartTransport(port, baudrate);
  if(!transport)
    return EXIT_FAILURE;

  // start the demo command thread
  std::thread thread;
  thread = std::thread(commandThread);

  helper::ConfigureMessageRates rates;
  rates.addMessage(asctec_uav_msgs::MESSAGE_ID_IMU, 200);

  if(rates.sendConfiguration(transport, std::chrono::seconds(1)))
  {
    std::cout << "Set IMU data rate ack'ed\n";
  }
  else
  {
    std::cerr << "Set IMU data rate failed\n";
    return EXIT_FAILURE;
  }

  // main loop waits for data from Trinity module and prints IMU Messages
  while(true)
  {
    uint32_t id;
    asctec_comm::ByteVector datagram;

    // wait for data with a timeout of 1s
    bool success = transport->waitForData(std::chrono::milliseconds(1000), &id, &datagram);

    if(!success)
    {
      continue;
    }

    // If this is an IMU message, print some contents
    if(id == asctec_uav_msgs::MESSAGE_ID_IMU)
    {
      if(datagram.size() == sizeof(asctec_uav_msgs::Imu))
      {
        asctec_uav_msgs::Imu* msg = reinterpret_cast<asctec_uav_msgs::Imu*>(datagram.data());
        std::cout << "Imu: ";
        std::cout << "  attitude (w x y z): " << msg->attitude.w << " " << msg->attitude.x << " " << msg->attitude.y
            << " " << msg->attitude.z << std::endl;
      }
    }
  }

  shutdownRequested = true;
  thread.join();

  return EXIT_SUCCESS;
}
