# Climate Monitor

`climate-monitor` is an ESP-IDF firmware project for a network-connected environmental monitor built around an ESP32-class board. The device reads local climate data from onboard sensors, connects to Wi-Fi, synchronizes time, and exchanges messages with a remote service over a secure WebSocket connection.

The repository is organized around firmware responsibilities rather than a single monolithic application. Internal module boundaries may change over time, but the overall goal stays the same: collect environmental telemetry from the device, publish it to a backend, and respond to remote commands for simple on-device feedback such as status lighting.

## What This Project Does

- Connects the device to a configured Wi-Fi network
- Synchronizes system time for network operations that depend on valid time
- Reads temperature, humidity, and pressure data from a BME280 sensor over I2C
- Publishes telemetry to a backend service over TLS-secured WebSocket messaging
- Accepts remote control messages and applies them to the onboard RGB LED

## Intended Use

This firmware is meant to be the embedded side of a larger climate monitoring system. A typical deployment looks like this:

1. The ESP32 boots and initializes its peripherals.
2. It joins Wi-Fi and brings up its network stack.
3. It establishes a secure connection to an external service.
4. It periodically sends sensor readings upstream.
5. It reacts to selected commands sent back from the server.

The exact internal structure may evolve, but that device behavior is the core of the project.

## Hardware Assumptions

The current firmware targets a board setup with:

- An ESP32-family microcontroller supported by ESP-IDF
- A BME280 environmental sensor connected over I2C
- A single RGB status LED driven by the firmware

If pin assignments, peripherals, or board variants change, the project purpose and integration model remain the same even if the implementation details move around.

## Software Stack

- ESP-IDF build system
- FreeRTOS task-based runtime
- Espressif component dependencies for WebSocket, LED control, and sensor support

## Configuration

At minimum, the firmware expects local Wi-Fi credentials to be configured before flashing. In an ESP-IDF workflow, that is typically done through:

```bash
idf.py menuconfig
```

From there, provide values for the project-specific Wi-Fi settings exposed by the firmware configuration menu.

Depending on how the project evolves, you may also need to review device-specific settings such as target chip, serial port, flash options, or backend connection details.

## Build and Flash

Standard ESP-IDF commands should work for local development:

```bash
idf.py build
idf.py flash
idf.py monitor
```

If your environment is not set up yet, install ESP-IDF first and export the toolchain environment before running those commands.

## Repository Notes

- The codebase is intentionally modular, and files may be renamed, merged, or reorganized over time.
- Some helper functionality may be added or removed during development without changing the project’s overall purpose.
- Certificates, component manifests, and SDK configuration files are part of the firmware build and connectivity setup.
