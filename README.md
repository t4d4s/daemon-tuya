# 🔌👻 Daemon Tuya

> A lightweight Linux daemon written in C for communicating with Tuya IoT smart devices.

---

## Overview

`Daemon Tuya` is a C-based system daemon that interfaces with the Tuya IoT platform using the official Tuya C SDK. It runs in the background as a persistent system service, enabling local or cloud-connected control of Powered by Tuya (PBT) devices — smart plugs, lights, sensors, and more — directly from a Linux host.

## Features

- Daemon process architecture (runs headlessly as a background service)
- Integrates the official [Tuya IoT C SDK](https://github.com/tuya/tuya-iot-core-sdk)
- Built entirely in C for minimal footprint and high performance
- Simple `make`-based build system

## Repository Structure

```
c-daemon-tuya/
├── src/            # Daemon source code (C)
├── tuya-sdk/       # Tuya IoT C SDK (submodule/vendor)
└── Makefile        # Build configuration
```

## Requirements

- GCC or Clang
- GNU Make
- Linux (Ubuntu 20.04+ recommended)
- CMake (may be required by the Tuya SDK)
- OpenSSL development libraries

On Ubuntu/Debian:

```bash
sudo apt install build-essential cmake libssl-dev
```

## Building

Clone the repository, then build with `make`:

```bash
git clone https://github.com/t4d4s/c-daemon-tuya.git
cd c-daemon-tuya
make
```

## Configuration

Before running, you'll need credentials from the [Tuya IoT Development Platform](https://iot.tuya.com):

| Parameter | Description |
|-----------|-------------|
| **Product ID (PID)** | Your device's product identifier |
| **Device ID** | Unique device UUID |
| **Device Secret / Authorization Key** | Used for cloud authentication |
| **Region** | Cloud region (`us`, `eu`, `cn`) |

Add these values to the relevant configuration file or source header before building.

## Running

Once built, start the daemon:

```bash
./c-daemon-tuya
```

To run it as a persistent background service, consider using a systemd unit file:

```ini
[Unit]
Description=Tuya Device Daemon
After=network.target

[Service]
ExecStart=/usr/local/bin/c-daemon-tuya
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

Install and enable it:

```bash
sudo cp c-daemon-tuya /usr/local/bin/
sudo cp c-daemon-tuya.service /etc/systemd/system/
sudo systemctl enable --now c-daemon-tuya
```

## Tuya SDK

The `tuya-sdk/` directory contains the Tuya IoT Core C SDK, which handles:

- Device activation and registration
- Bidirectional MQTT communication with the Tuya cloud
- OTA firmware update support
- Keepalive and reconnection logic

For more details, see the [official Tuya IoT Core SDK documentation](https://github.com/tuya/tuya-iot-core-sdk).

## Contributions
Contributions are welcome! If you'd like to improve the project or add new features, please submit a pull request.

## License

This project is open-source and available under the MIT License.

## Author

This project is maintained by [Tadas](https://github.com/t4d4s). Feel free to reach out with any questions or feedback.
