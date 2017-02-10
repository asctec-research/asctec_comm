# asctec_comm

Library and example applications for the companion computer / ground station to
communicate with the AutoPilot SDK.

## Communication Layers

| Layer            | Data Unit       | Description                                        | Implementation           | 
| ---------------- | --------------- | -------------------------------| ----------------- | ------------------------ |
| "Physical"       | raw bytes       | raw byte stream, e.g. serial port, USB, file       | RawBuffer base class     |
| Data link        | frame           | transmission of data frames                        | DataLink                 |
| Transport        | datagram        | reliable transmission, including acknowledgment    | Transport                |
| Application      | data            | Application data                                   | user implementation      |

## Getting the code
The asctec_comm package uses GIT submodules to pull in common message definitations. Either clone this repository recursively with: 

    git clone --recursive https://github.com/asctec-research/asctec_comm.git
    
or, if you already cloned it, pull in all submodules:

    git submodule init
    git submodule update
    
This package uses CMake to generate build files and is compatible with catkin environments.

## Native Linux build
Go into the asctec_comm folder and execute:

    cmake .
    make

## Windows build
Install CMake and start the cmake-gui. Select the asctec_comm folder which you cloned from GIT as source folder. It is common practice to locate build files in a different subfolder. Select asctec_comm/build as location where to build binaries (create this folder if it does not exist). Click _Configure_ and select your preffered generator (tested with Visual Studio 14 2015). Click _Generate_ and _Open Project_. You may now build the entire solution in Visual Studio.

## Example Application
The example program connects to the AscTec UAV and prints out IMU messages. 
It is a reference implementation for data polling which you can extend with further messages.
