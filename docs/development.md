# Development Guide

## Testing Framework

The Frankly Bootloader includes a comprehensive testing suite to ensure reliability and correctness.

### Test Structure

```
tests/
├── CMakeLists.txt              # Test build configuration
├── frankly_test_utils/         # Shared testing utilities
└── src/                        # Test source files
    ├── francor/
    │   └── franklyboot/
    │       ├── test_handler.cpp        # Core handler tests
    │       ├── test_msg.cpp           # Message protocol tests
    │       └── test_integration.cpp    # Integration tests
    └── ...
```

### Running Tests

```bash
# Build and run all tests
cd build
cmake ..
make
ctest

# Run specific test categories
ctest -R "handler"     # Run handler tests only
ctest -R "msg"         # Run message tests only
ctest -V              # Verbose output
```

### Test Categories

#### 1. Unit Tests
- **Handler Tests**: Core bootloader logic validation
- **Message Tests**: Protocol parsing and validation
- **Utility Tests**: Helper function verification

#### 2. Integration Tests
- **End-to-End Communication**: Full request/response cycles
- **Flash Operation Simulation**: Virtual flash memory testing
- **Error Condition Testing**: Comprehensive error handling

#### 3. Hardware Simulation Tests
- **Device Simulator**: Mock hardware interface implementation
- **CRC Validation**: Cryptographic hash verification
- **Memory Layout**: Flash organization testing

## Utility Tools

### Device Simulator API

Located in `utils/device_sim_api/`, this tool provides:

**Purpose**: Software simulation of bootloader hardware for development and testing

**Features**:
- Virtual flash memory simulation
- CRC calculation implementation
- Device information mocking
- Communication interface simulation

**Usage**:
```cpp
#include "device_sim_api.h"

// Create simulated device
DeviceSimulator sim(flash_size, page_size);

// Simulate hardware operations
sim.writeFlashPage(page_id, data);
uint32_t crc = sim.calculateCRC(address, size);
```

### CAN Network Simulator

Located in `utils/can_network_sim/`, provides:

**Purpose**: Simulation of CAN bus communication for multi-device testing

**Features**:
- Multi-node CAN network simulation
- Message routing and filtering
- Timing and latency simulation
- Error injection for robustness testing

**Use Cases**:
- Testing bootloader behavior in multi-device networks
- Protocol validation across different node configurations
- Performance and timing analysis

## Development Workflow

### 1. Setting Up Development Environment

```bash
# Clone repository
git clone <repository-url>
cd frankly-bootloader

# Create build directory
mkdir build && cd build

# Configure with development options
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j$(nproc)

# Run tests
ctest
```

### 2. Code Standards

The project follows strict coding standards:

**C++ Standard**: C++17
**Style**: Configured via `.clang-format`
**Static Analysis**: `.clang-tidy` configuration provided

**Formatting**:
```bash
# Format all source files
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check formatting
find . -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run
```

**Static Analysis**:
```bash
# Run clang-tidy
clang-tidy src/**/*.cpp -- -std=c++17 -I include/
```

### 3. Adding New Features

#### New Request Types

1. **Add enum to `msg.h`**:
```cpp
enum RequestType : uint16_t {
    // ... existing requests ...
    REQ_MY_NEW_REQUEST = 0x2000U,
};
```

2. **Add handler method to `handler.h`**:
```cpp
void handleReqMyNewRequest(const msg::Msg& request);
```

3. **Implement in `handler.tpp`**:
```cpp
template <uint32_t FLASH_START, uint32_t FLASH_APP_FIRST_PAGE,
          uint32_t FLASH_SIZE, uint32_t FLASH_PAGE_SIZE>
void Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE>
    ::handleReqMyNewRequest(const msg::Msg& request) {
    // Implementation
}
```

4. **Add to request dispatch in `processRequest()`**
5. **Create documentation in `docs/protocol/RequestTypes/`**
6. **Add tests in `tests/src/`**

#### New Hardware Interface Functions

1. **Add declaration to `hardware_interface.h`**
2. **Update all platform implementations**
3. **Add simulation support in `utils/device_sim_api/`**
4. **Add tests for new functionality**

### 4. Testing New Features

```cpp
// Example test structure
#include <gtest/gtest.h>
#include <francor/franklyboot/handler.h>

class HandlerTest : public ::testing::Test {
protected:
    using TestHandler = franklyboot::Handler<0x08000000, 32, 131072, 2048>;
    TestHandler handler;
};

TEST_F(HandlerTest, NewRequestHandling) {
    franklyboot::msg::Msg request{
        franklyboot::msg::REQ_MY_NEW_REQUEST,
        franklyboot::msg::RES_NONE,
        0,
        {0, 0, 0, 0}
    };

    handler.processRequest(request);
    auto response = handler.getResponse();

    EXPECT_EQ(response.result, franklyboot::msg::RES_OK);
    // Additional assertions...
}
```

## Debugging

### Debug Build Configuration

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0" ..
```

### Common Debug Scenarios

1. **Message Protocol Issues**:
   - Enable verbose logging in test utilities
   - Use hex dumps to inspect raw message bytes
   - Validate endianness and byte order

2. **Flash Operation Problems**:
   - Use device simulator with debug output
   - Check address calculations and bounds
   - Verify CRC implementations

3. **Template Parameter Issues**:
   - Review static assertions in handler.h
   - Validate memory layout calculations
   - Check for overflow in address arithmetic

### Debug Tools

```cpp
// Debug message printing (in test environment)
void debugPrintMsg(const franklyboot::msg::Msg& msg) {
    printf("Request: 0x%04X, Result: 0x%02X, PacketID: %d\n",
           msg.request, msg.result, msg.packet_id);
    printf("Data: 0x%02X 0x%02X 0x%02X 0x%02X\n",
           msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
}

// Memory dump utility
void debugDumpFlash(uint32_t start, uint32_t size) {
    for (uint32_t i = 0; i < size; i += 16) {
        printf("%08X: ", start + i);
        for (uint32_t j = 0; j < 16 && i + j < size; ++j) {
            printf("%02X ", hwi::readByteFromFlash(start + i + j));
        }
        printf("\n");
    }
}
```

## Continuous Integration

### GitHub Actions

The project includes CI configuration for automated testing:

- **Build Verification**: Multiple compiler versions
- **Test Execution**: Full test suite on each commit
- **Code Quality**: Formatting and static analysis checks
- **Documentation**: Automated documentation generation

### Quality Gates

- All tests must pass
- Code coverage minimum threshold
- No static analysis warnings
- Documentation must be updated for API changes

## Performance Considerations

### Memory Usage

- **Stack Usage**: Handler uses minimal stack space
- **RAM Requirements**: Only page buffer and message structures
- **Flash Usage**: Template instantiation creates single binary

### Timing Constraints

- **Response Time**: Deterministic response generation
- **Flash Operations**: Hardware-dependent timing
- **CRC Calculations**: Linear time complexity

### Optimization Guidelines

1. **Template Specialization**: Optimize for common configurations
2. **Inlining**: Critical path functions should be inlined
3. **Memory Access**: Minimize flash reads during operation
4. **Error Paths**: Fast failure for invalid requests