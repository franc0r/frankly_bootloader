# Architecture Overview

## System Design

The Frankly Bootloader is designed with a modular, template-based architecture that provides flexibility while maintaining hardware abstraction. The system consists of several key layers:

```
┌─────────────────────────────────────────────┐
│              Application Layer              │
│         (Host PC Communication)            │
└─────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│             Protocol Layer                  │
│     (Message Parsing & Validation)         │
└─────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│            Handler Template                 │
│     (Business Logic & State Machine)       │
└─────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│         Hardware Interface (HWI)            │
│     (Platform-specific Operations)         │
└─────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────┐
│             Hardware Layer                  │
│      (Flash, CRC, Comms, etc.)            │
└─────────────────────────────────────────────┘
```

## Core Components

### 1. Handler Template (`handler.h`)

The `Handler` template class is the heart of the bootloader, parameterized by flash memory characteristics:

```cpp
template <uint32_t FLASH_START, uint32_t FLASH_APP_FIRST_PAGE,
          uint32_t FLASH_SIZE, uint32_t FLASH_PAGE_SIZE>
class Handler
```

**Template Parameters:**
- `FLASH_START`: Base address of flash memory (e.g., 0x08000000 for STM32)
- `FLASH_APP_FIRST_PAGE`: First page index where application starts
- `FLASH_SIZE`: Total flash memory size in bytes
- `FLASH_PAGE_SIZE`: Size of each flash page in bytes

**Key Features:**
- Compile-time validation of flash parameters
- Automatic calculation of derived values (app size, CRC location, etc.)
- State management for multi-step operations
- Page buffer management for efficient flash programming

### 2. Message Protocol (`msg.h`)

Defines the communication protocol structure:

```cpp
struct Msg {
    RequestType request;    // 16-bit request identifier
    ResultType result;      // 8-bit result/error code
    uint8_t packet_id;      // Packet sequence identifier
    MsgData data;          // 4-byte payload
};
```

**Message Flow:**
1. Host sends request with `result = RES_NONE`
2. Bootloader processes request
3. Bootloader sends response with appropriate `result` code
4. Data payload varies by request type

### 3. Hardware Interface (`hardware_interface.h`)

Abstract interface that must be implemented for each target platform:

```cpp
namespace franklyboot::hwi {
    void resetDevice();
    uint32_t getVendorID();
    uint32_t getProductID();
    uint32_t getProductionDate();
    uint32_t getUniqueIDWord(uint32_t idx);
    uint32_t calculateCRC(uint32_t src_address, uint32_t num_bytes);
    bool eraseFlashPage(uint32_t page_id);
    bool writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id,
                               uint8_t* src_data_ptr, uint32_t num_bytes);
    uint8_t readByteFromFlash(uint32_t flash_src_address);
    void startApp(uint32_t app_flash_address);
}
```

## Memory Layout

The bootloader assumes a specific flash memory layout:

```
┌─────────────────────────────────────────────┐ ← FLASH_START
│                                             │
│            Bootloader Code                  │
│                                             │
├─────────────────────────────────────────────┤ ← FLASH_START + (FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE)
│                                             │
│            Application Code                 │
│                                             │
│                                             │
├─────────────────────────────────────────────┤ ← FLASH_START + FLASH_SIZE - 4
│         App CRC (4 bytes)                   │
└─────────────────────────────────────────────┘ ← FLASH_START + FLASH_SIZE
```

**Key Addresses:**
- **Bootloader Area**: `[FLASH_START, FLASH_APP_ADDRESS)`
- **Application Area**: `[FLASH_APP_ADDRESS, FLASH_APP_CRC_VALUE_ADDRESS)`
- **CRC Storage**: `[FLASH_APP_CRC_VALUE_ADDRESS, FLASH_START + FLASH_SIZE)`

Where:
- `FLASH_APP_ADDRESS = FLASH_START + FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE`
- `FLASH_APP_CRC_VALUE_ADDRESS = FLASH_START + FLASH_SIZE - 4`

## State Management

The bootloader maintains several internal states:

### Command Buffer
For operations that cannot be executed immediately (e.g., device reset, app start):
```cpp
enum class CommandBuffer {
    NONE,          // No pending command
    RESET_DEVICE,  // Reset pending
    START_APP,     // App start pending
};
```

### Page Buffer
RAM buffer for staging flash page data before writing:
- Size: `FLASH_PAGE_SIZE` bytes
- Allows partial writes and CRC calculation before flash commit
- Position tracking for incremental data loading

## Request Processing Flow

1. **Message Reception**: Raw bytes converted to `Msg` structure
2. **Request Validation**: Check request type and parameters
3. **Request Routing**: Dispatch to appropriate handler method
4. **Processing**: Execute request-specific logic
5. **Response Generation**: Create response message with result code
6. **Response Transmission**: Convert response to byte array for sending

## Error Handling

The system uses a comprehensive error code system:
- **Success**: `RES_OK` (0x01)
- **Validation Errors**: Unknown requests, invalid arguments
- **Operation Errors**: CRC failures, buffer overflows
- **Hardware Errors**: Flash operation failures

## Platform Integration

### Required Implementations

For each target platform, implement:

1. **Hardware Interface Functions**: All functions in `franklyboot::hwi` namespace
2. **Communication Layer**: Message transport (CAN, UART, etc.)
3. **Bootloader Entry**: Logic to enter/exit bootloader mode
4. **Memory Configuration**: Correct template parameters for flash layout

### Example Integration

For complete, working examples of platform-specific integrations, see the [Frankly Bootloader Examples](https://github.com/franc0r/frankly_bootloader_examples) repository, which includes STM32 implementations and host applications.

```cpp
// Platform-specific configuration
using MyBootloader = franklyboot::Handler<
    0x08000000,    // STM32 flash base
    32,            // App starts at page 32 (64KB offset with 2KB pages)
    131072,        // 128KB total flash
    2048           // 2KB page size
>;

// Instantiate bootloader
MyBootloader bootloader;

// Message processing loop
void processBootloaderMessages() {
    while (bootloader_active) {
        auto raw_msg = receiveMessage();  // Platform-specific
        auto msg = franklyboot::msg::convertBytesToMsg(raw_msg);

        bootloader.processRequest(msg);

        auto response = bootloader.getResponse();
        auto raw_response = franklyboot::msg::convertMsgToBytes(response);

        sendMessage(raw_response);  // Platform-specific

        bootloader.processBufferedCmds();  // Handle deferred commands
    }
}
```

## Design Principles

1. **Hardware Abstraction**: Clean separation between protocol logic and hardware
2. **Template Flexibility**: Compile-time configuration for different flash layouts
3. **Memory Safety**: Bounds checking and validation at all levels
4. **Deterministic Behavior**: Predictable response times and memory usage
5. **Transport Agnostic**: Works with CAN, UART, or other communication methods
6. **Embedded Friendly**: Minimal RAM usage, no dynamic allocation