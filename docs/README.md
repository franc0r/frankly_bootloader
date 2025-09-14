# ![FRANCOR-LOGO](./images/francor-logo-small.png) FRANCOR's - Frankly Bootloader

## Overview

This is the embedded device bootloader developed by the non-profit association [FRANCOR - Franconia Open Robotics](https://www.francor.de/). The aim is to provide a very basic, expandable bootloader, which is supporting multiple communication devices e.g. (CAN, Serial, ...).

This repository contains the source files, which has to be included to the embedded device. The application to flash the bootloader is developed in another repository.

## Documentation Structure

This documentation is organized into several key sections:

### [Architecture](./architecture.md)
Comprehensive overview of the bootloader's design, including:
- System architecture and component interaction
- Memory layout and flash organization
- Template-based configuration system
- Hardware abstraction layer design
- Integration guidelines for new platforms

### [Development Guide](./development.md)
Essential information for developers working on the bootloader:
- Testing framework and test execution
- Utility tools (device simulator, CAN network simulator)
- Development workflow and coding standards
- Debugging techniques and performance considerations
- Continuous integration setup

### [Communication Protocol](./protocol/README.md)
Detailed specification of the bootloader communication protocol:
- Message format and encoding
- Request/response patterns
- Complete list of supported operations
- Error handling and result codes
- Protocol examples and usage patterns

## Quick Navigation

- **New to the project?** Start with [Architecture](./architecture.md) to understand the system design
- **Want to contribute?** Check the [Development Guide](./development.md) for setup instructions
- **Integrating the bootloader?** See the [Communication Protocol](./protocol/README.md) documentation
- **Looking for implementation examples?** Visit the [Frankly Bootloader Examples](https://github.com/franc0r/frankly_bootloader_examples) repository
- **Looking for specific requests?** Browse the [Request Types](./protocol/RequestTypes/README.md) reference

## Implementation Examples

The [Frankly Bootloader Examples](https://github.com/franc0r/frankly_bootloader_examples) repository provides complete, working implementations of the bootloader for various platforms:

- **STM32 Family**: Complete bootloader implementations with CAN and UART support
- **Host Applications**: PC-side tools for device flashing and management
- **Hardware Interface Examples**: Platform-specific implementations of the hardware abstraction layer
- **Integration Guides**: Step-by-step integration examples for different microcontroller families

These examples serve as practical references for implementing the bootloader on your specific hardware platform.

## Features Highlights

- **Multi-transport Support**: Works with CAN, Serial, and other communication protocols
- **Template-based Architecture**: Configurable for different flash memory layouts
- **128-bit Device Identification**: Complete unique device ID support
- **Robust Error Handling**: Comprehensive error codes and validation
- **Hardware Abstraction**: Clean separation between protocol and platform code
- **Testing Infrastructure**: Comprehensive test suite with hardware simulation

