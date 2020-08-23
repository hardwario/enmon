# Repository: enmon

[![Travis](https://img.shields.io/travis/hardwario/enmon/master.svg)](https://travis-ci.org/hardwario/enmon)
[![Appveyor](https://ci.appveyor.com/api/projects/status/71uqxwhpcabphs26?svg=true)](https://ci.appveyor.com/project/Hardwario/enmon)
[![Release](https://img.shields.io/github/release/hardwario/enmon.svg)](https://github.com/hardwario/enmon/releases)
[![License](https://img.shields.io/github/license/hardwario/enmon.svg)](https://github.com/hardwario/enmon/blob/master/LICENSE)
[![Twitter](https://img.shields.io/twitter/follow/hardwario_en.svg?style=social&label=Follow)](https://twitter.com/hardwario_en)

This repository contains source code for the **Environmental Monitoring Toolkit**.

The result is a CLI utility (called `enmon`) that reads hardware sensor data via **HARDWARIO Bridge Module** such as:

* Temperature (from **Humidity Tag** or **Climate Module**)
* Humidity (from **Humidity Tag** or **Climate Module**)
* Illuminance (from **Lux Meter Tag** or **Climate Module**)
* Pressure (from **Barometer Tag** or **Climate Module**)
* Altitude (from **Barometer Tag** or **Climate Module**)

The read values are printed to the console (via `stdout`) and can be easily piped to other tools.

This tool is also integrated with **HARDWARIO Playground** so you can easily use it altogether with **Node-RED**.

> HARDWARIO Bridge Module does not need any drivers as it uses USB HID for communication.


## Requirements

* Host with Windows, macOS, or Linux operating system
* HARDWARIO Bridge Module
* Micro USB cable


## Build

You will need CMake + C compiler (GCC, LLVM Clang, or MSVC) to build the binary.

Just follow these steps:

1. Open the Terminal app or Command Prompt.

1. Clone the repository:

       $ git clone https://github.com/hardwario/enmon.git

1. Go to the repository:

       $ cd enmon

1. Create the `build` directory:

       $ mkdir build

1. Go to the `build` directory:

       $ cd build

1. Let CMake create the build script:

       $ cmake ..

1. Start the actual build:

       $ cmake --build .

The result is the single `enmon` (or `enmon.exe`) executable in the `build` directory. Feel free to copy it where you want.


## Usage

You can invoke the help:

    $ ./enmon --help

Run without arguments will read the sensor data, print them, and exit:

    $ ./enmon
    @SENSOR: "Temperature",27.20
    @SENSOR: "Humidity",50.2
    @SENSOR: "Illuminance",25
    @SENSOR: "Pressure",95306
    @SENSOR: "Altitude",514.2

You can force the program to continuously read the sensors with a specified delay:

    $ ./enmon --loop --delay=5
    @SENSOR: "Temperature",27.20
    @SENSOR: "Humidity",50.2
    @SENSOR: "Illuminance",25
    @SENSOR: "Pressure",95306
    @SENSOR: "Altitude",514.2
    @SENSOR: "Temperature",27.20
    @SENSOR: "Humidity",50.2
    @SENSOR: "Illuminance",25
    @SENSOR: "Pressure",95306
    @SENSOR: "Altitude",514.2


## Linux

On Linux, it is important to get the `udev` properly configured so you won't need the admin right to access the device. Please, follow these steps:

1. Install the dependencies:

       # apt install -y build-essential cmake libudev-dev

1. Create the file `/etc/udev/rules.d/99-enmon.rules`:

       SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6030", MODE="0666"

1. Reload the `udev` rules:

       # udevadm control --reload


## License

This project is licensed under the [**MIT License**](https://opensource.org/licenses/MIT/) - see the [**LICENSE**](LICENSE) file for details.

---

Made with ❤️ by [**HARDWARIO s.r.o.**](https://www.hardwario.com/) in the heart of Europe.
