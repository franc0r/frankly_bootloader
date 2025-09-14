# FRANCOR's - Frankly Bootloader

## License

BSD 3-Clause License see LICENSE

## Overview

This is the embedded device bootloader developed by the non-profit association [FRANCOR - Franconia Open Robotics](https://www.francor.de/). The aim is to provide a very basic, expandable bootloader, which is supporting multiple communication devices e.g. (CAN, Serial, ...).

This repository contains the source files, which has to be included to the embedded device. The application to flash the bootloader is developed in another repository.

## Features

- **Multi-transport Support**: Designed to work with CAN, Serial, and other communication protocols
- **Template-based Architecture**: Configurable flash parameters via C++ templates
- **128-bit Unique Device ID**: Full device identification support
- **CRC Verification**: Built-in application integrity checking
- **Page Buffer Management**: Efficient flash programming via RAM buffer
- **Hardware Abstraction**: Clean separation between bootloader logic and hardware interface

## Architecture

The bootloader consists of several key components:

- **Handler Template**: Core bootloader logic with configurable flash parameters
- **Message Protocol**: Standardized request/response communication
- **Hardware Interface**: Abstract layer for hardware-specific operations
- **Utility Tools**: Device simulation and testing infrastructure

## Quick Start

### Examples Repository

Complete implementation examples for various devices and platforms are available in the [Frankly Bootloader Examples](https://github.com/franc0r/frankly_bootloader_examples) repository. This includes:

- **STM32 implementations** with CAN and UART communication
- **Host applications** for flashing and device management
- **Platform-specific hardware interface implementations**
- **Real-world integration examples** for different microcontrollers

### Integration

1. Include the bootloader headers in your embedded project:
```cpp
#include <francor/franklyboot/handler.h>
#include <francor/franklyboot/msg.h>
```

2. Implement the hardware interface functions in `franklyboot::hwi` namespace
3. Instantiate the Handler template with your flash parameters:
```cpp
// Example for STM32 with 128KB flash, 2KB pages, app starting at page 32
franklyboot::Handler<0x08000000, 32, 131072, 2048> bootloader;
```

4. Process incoming messages:
```cpp
auto message = convertBytesToMsg(received_data);
bootloader.processRequest(message);
auto response = bootloader.getResponse();
// Send response via your communication interface
```

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Testing

The project includes comprehensive test suites:
```bash
cd build
ctest
```

## Documentation

Comprehensive documentation is available in the [docs/](docs/) directory:

- [Communication Protocol](docs/protocol/README.md)
- [Request Types](docs/protocol/RequestTypes/README.md)
- [Result Types](docs/protocol/ResultTypes.md)
- [Implementation Examples](https://github.com/franc0r/frankly_bootloader_examples) (External repository)

## Project Structure

```
├── include/francor/franklyboot/  # Public API headers
├── src/francor/franklyboot/      # Implementation files
├── tests/                        # Test suites
├── utils/                        # Utility tools
│   ├── device_sim_api/          # Device simulator API
│   └── can_network_sim/         # CAN network simulator
└── docs/                         # Documentation
```

## Hardware Interface Requirements

To integrate this bootloader, you must implement the following hardware interface functions:

- `resetDevice()` - Perform system reset
- `getVendorID()`, `getProductID()`, `getProductionDate()` - Device identification
- `getUniqueIDWord(idx)` - 128-bit unique ID access
- `calculateCRC()` - CRC calculation over memory regions
- `eraseFlashPage()`, `writeDataBufferToFlash()` - Flash memory operations
- `readByteFromFlash()` - Flash memory reading
- `startApp()` - Application startup

## Contributing

We welcome contributions! Please see our coding standards:
- Follow the existing C++17 style
- Use clang-format configuration provided
- Add tests for new features
- Update documentation as needed

