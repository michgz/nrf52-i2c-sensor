# nrf52-i2c-sensor

A project that's intended to be a "generic" platform for reading I2C sensor data over Bluetooth Low Energy using nRF52 hardware. Sensor readings are taken at regular intervals
and kept in a circular RAM buffer. A host can connected over Bluetooth at any time and collect new data from the circular buffer. It's intended for the
remote end to
be battery-powered and so current consumption is highly optimised.

The sensor that's currently implemented is a Sensirion SHT-40 temperature/humidity sensor, but most of the functionality is completely agnostic to the
sensor that's being used - just swap out the "src/sensor.c" file and everything else should work the same.

The "scripts" directory contains example code for the host end, using Python and the [Bleak](https://pypi.org/project/bleak/) library.

## How to build

Firstly populate the nRF5_SDK_17.1.0_ddde560 directory with v17.1.0 of the SDK downloaded from Nordic (https://www.nordicsemi.com/Products/Development-software/nrf5-sdk/download). Then do:

```
make --directory=src/custom_board/s112/armgcc/ all
```
