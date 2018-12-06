#Simple and Low cost Pico Balloon Tracker Version 1.0


##Objectives
This Pico balloon tracker is designed to offer an easy to build and low cost entry for people who want to launch small Helium filled balloons with a small tracking payload.

* Low mass: less than 25g
* low cost: less than £15
* Compatible with existing HAB tracking systems: 433MHz RTTY
* Quick build: Use existing off the shelf modules where possible

## Design

* GPS: Beitian BN-280 (maybe BN-220). These low cost (£6) GPS modules are based on the uBlox M8N chip which is suitable for HAB use. The modules contain the uBlox G8030 chip with a TCXO, battery, external flash, LNA and SAW filter. The plan is to remove the metal shield and to replace the ceramic antenna and replace it with a guitar wire antenna with ground plane. Initial mass is 13.5g, stripped it is 2.5g

* 433MHz Radio and processor: HC12 Si4463 based radio with STM8S003F3 processor. This low cost radio module(£3) is available preprogrammed for remote control and monitoring applications. Though normally used with the preprogrammed code for a serial interface they can be reprogrammed with different code for our tracker. The STM8S0003F3 is fairly powerful, but has limited flash memory(8K) and RAM(1K) so fitting the code is challenge.  Mass is 1g.

* Battery: The battery supply is based on a previous design.  [BatteryAAA](https://github.com/ribbotson/rlabTelemetryTx/tree/master/Hardware/BatteryAAA) . Which uses a AAA lithium primary cell via a boost converter to provide 3.3V to the GPS, Radio and Processor. Mass battery 8g, battery holder and boost converter 3.5g

* Tracker Body: The body of the tracker will be either 3D printed or made from polystyrene foam. The guitar wire antenna will be at the top for the GPS and the bottom for the 433MHz radio. The mass of the body is around 8g now.

* Firmware: The firmware will be derived from existing firmware created and developed by  [Bristol SEDS](http://www.bristol-seds.co.uk/pico-tracker/) The firmware will require substantial changes to support the much more limited processor.

* Firmware Development: The firmware is developed using the [IAR Embedded Workbench Free Code limited version](https://www.iar.com/iar-embedded-workbench/#!?architecture=STM8&device=STM8S003F3). The processor is programmed and debugged using the [ST-Link/V2](https://www.st.com/en/development-tools/st-link-v2.html) . Clones are available on eBay for around £4.

## Design Challenges
* Power consumption: None of the modules is especially designed for low power operation and no power switching is available. We need to determine how we can reduce the power consumption of each module using firmware
* Low temperature: The GPS appears to have a Temperature Compensated Crystal Oscillator (TXC0), the radio a crystal, and the processor runs from internal clock. Some ingenuity and testing will be required to allow operation at outside temperatures of -50degC.

##Status
* Done: Prototype build, Processor initialization, Radio code, RTTY and pips

* Working on: GPS and message string creation

* To Do:  Power Management, Temperature issues, final packaging, Testing

##License

Hardware is Licensed under CERN OHL v.1.2 or later https://www.ohwr.org/documents/294 No warranty is provided for this documentation implied or otherwise.

Software is licensed under MIT License.

MIT License

Copyright (c) 2018 Imperial College Space Society

Derived Software  Copyright (c) 2014  Richard Meadows <richardeoin>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.






