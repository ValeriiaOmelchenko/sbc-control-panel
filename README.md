# sbc-control-panel
Project structure
<pre> 
sbc-control-panel/
├── CMakeLists.txt
├── include/
│   ├── GpioPin.hpp
│   ├── ZmqInterface.hpp
│   ├── LedController.hpp
│   ├── NetworkMonitor.hpp
│   ├── ButtonWatcher.hpp
│   ├── MicrophoneMonitor.hpp
│   └── StateTypes.hpp
├── src/
│   ├── main.cpp
│   ├── GpioPin.cpp
│   ├── ZmqInterface.cpp
│   ├── LedController.cpp
│   ├── NetworkMonitor.cpp
│   ├── ButtonWatcher.cpp
│   └── MicrophoneMonitor.cpp
├── README.md
└── .gitignore
<pre> 

You can find schematic on this path assets/schematic.png

## Installing Dependencies

To build and run this project on Raspberry Pi (tested on Raspberry Pi 4), you need the following dependencies:

- `g++` (C++17 compiler)
- `cmake` (build system)
- `pkg-config` (library helper)
- `libzmq3-dev` (ZeroMQ)
- [`wiringPi`](http://wiringpi.com/) (GPIO control — manually installed)

You can install most of them with the script below:

```
./install_dependencies.sh
```

The wiringPi library is no longer maintained in APT and must be installed manually. To do this:

```
cd ~
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build
```
