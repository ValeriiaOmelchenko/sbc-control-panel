# sbc-control-panel

## Project structure

```text
sbc-control-panel/
├── assets/
│   ├── schematic.png
│   └── state_diagram.png
├── include/
│   ├── GpioPin.hpp
│   ├── ZmqService.hpp
│   ├── LedController.hpp
│   ├── NetworkMonitor.hpp
│   ├── ButtonWatcher.hpp
│   ├── MicrophoneMonitor.hpp
│   └── StateTypes.hpp
├── src/
│   ├── main.cpp
│   ├── GpioPin.cpp
│   ├── ZmqService.cpp
│   ├── LedController.cpp
│   ├── NetworkMonitor.cpp
│   ├── ButtonWatcher.cpp
│   └── MicrophoneMonitor.cpp
├── CMakeLists.txt
├── build.sh
├── install_dependencies.sh
├── README.md
└── .gitignore
```
## Schematic

![alt](assets/schematic.png)

## State Diagram

You can view the full state diagram (LED & button control logic) here:

[Open Mermaid state diagram](https://www.mermaidchart.com/app/projects/dd48604f-f231-4d96-9801-e5d4dd8abb1a/diagrams/975d63b3-530b-47d8-b501-1a85a7f480c2/version/v0.1/edit)

![alt](assets/state_diagram.png)

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
