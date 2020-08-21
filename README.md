# ENMON

[![Travis](https://img.shields.io/travis/hardwario/enmon/master.svg)](https://travis-ci.org/hardwario/enmon)
[![Appveyor](https://ci.appveyor.com/api/projects/status/71uqxwhpcabphs26?svg=true)](https://ci.appveyor.com/project/Hardwario/enmon)
[![Release](https://img.shields.io/github/release/hardwario/enmon.svg)](https://github.com/hardwario/enmon/releases)
[![License](https://img.shields.io/github/license/hardwario/enmon.svg)](https://github.com/hardwario/enmon/blob/master/LICENSE)
[![Twitter](https://img.shields.io/twitter/follow/hardwario_en.svg?style=social&label=Follow)](https://twitter.com/hardwario_en)



## Linux

### Install dependencies
```
sudo apt-get install  build-essential cmake libudev-dev
```

### Add udev rule
```
SUBSYSTEMS=="usb", ACTION=="add", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6030", MODE="0666"
```
