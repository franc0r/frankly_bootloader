# Integration Guide

This guide describes how to integrate the Frankly Bootloader into a new microcontroller or board using the Hardware Interface abstraction layer.

## Overview

The Frankly Bootloader uses a hardware abstraction layer (HWI) to remain platform-independent. To integrate the bootloader on a new platform, you need to:

1. Define device-specific flash memory parameters
2. Implement the Hardware Interface functions
3. Set up the communication layer (UART, CAN, USB, etc.)
4. Configure the bootloader entry mechanism
5. Adjust linker scripts and memory layout

## Step 1: Device Configuration

Create a `device_defines.h` header file with your platform's flash memory characteristics.

### Example for STM32F303K8

```cpp
#ifndef DEVICE_DEFINES_H_
#define DEVICE_DEFINES_H_

#include <stdint.h>

#ifdef __cplusplus
namespace device {

constexpr uint32_t SYS_TICK = {8000000U};           // 8 MHz system clock

constexpr uint32_t FLASH_START_ADDR = {0x08000000U}; // Flash base address
constexpr uint32_t FLASH_APP_FIRST_PAGE = {4U};      // Bootloader uses first 4 pages
constexpr uint32_t FLASH_SIZE = {64 * 1024U};        // 64 KB total flash
constexpr uint32_t FLASH_PAGE_SIZE = {2048U};        // 2 KB page size

constexpr uint32_t FLASH_APP_START_ADDR = FLASH_START_ADDR + FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;

};  // namespace device
#endif /* __cplusplus */

#endif /* DEVICE_DEFINES_H_ */
```

### Example for RP2040

```cpp
#ifndef DEVICE_DEFINES_H_
#define DEVICE_DEFINES_H_

#include <stdint.h>

#ifdef __cplusplus
namespace device {

constexpr uint32_t SYS_TICK = {125000000U};          // 125 MHz system clock

constexpr uint32_t FLASH_START_ADDR = {0x10000000U}; // XIP flash base
constexpr uint32_t FLASH_APP_FIRST_PAGE = {32U};     // Bootloader uses first 128KB
constexpr uint32_t FLASH_SIZE = {2 * 1024 * 1024U};  // 2 MB total flash
constexpr uint32_t FLASH_SECTOR_SIZE = {4096U};      // 4 KB sector size

constexpr uint32_t FLASH_PAGE_SIZE_BOOT = FLASH_SECTOR_SIZE;
constexpr uint32_t FLASH_APP_START_ADDR = FLASH_START_ADDR + FLASH_APP_FIRST_PAGE * FLASH_SECTOR_SIZE;

};  // namespace device
#endif /* __cplusplus */

#endif /* DEVICE_DEFINES_H_ */
```

### Key Parameters

- **`SYS_TICK`**: System clock frequency in Hz (used for timing operations)
- **`FLASH_START_ADDR`**: Base address of flash memory in the memory map
- **`FLASH_APP_FIRST_PAGE`**: First page index where the application begins (bootloader occupies pages 0 to FLASH_APP_FIRST_PAGE-1)
- **`FLASH_SIZE`**: Total size of flash memory in bytes
- **`FLASH_PAGE_SIZE`**: Size of each flash page/sector in bytes (must match erase granularity)

### Understanding the Application CRC

The bootloader stores a 4-byte CRC-32 checksum at the **end of flash memory** to validate the application firmware before starting it. This mechanism ensures that only valid, complete firmware is executed.

**CRC Storage Location:**
```
FLASH_APP_CRC_VALUE_ADDRESS = FLASH_START_ADDR + FLASH_SIZE - 4
```

For example, with a 64KB flash starting at `0x08000000`:
- Flash range: `0x08000000` to `0x0800FFFF`
- CRC location: `0x0800FFFC` (last 4 bytes)

**How It Works:**

1. **During Flashing:**
   - The host tool calculates a CRC-32 over the entire application binary
   - The application binary is written to flash starting at `FLASH_APP_START_ADDR`
   - The calculated CRC value is written to the last 4 bytes of flash (`FLASH_APP_CRC_VALUE_ADDRESS`)

2. **During Boot:**
   - The bootloader reads the stored CRC from `FLASH_APP_CRC_VALUE_ADDRESS`
   - It calculates a fresh CRC over the application area (`FLASH_APP_START_ADDR` to `FLASH_APP_CRC_VALUE_ADDRESS - 1`)
   - If the stored CRC matches the calculated CRC, the application is valid and can be started
   - If they don't match (corrupted/incomplete flash), the bootloader remains active

3. **Application Memory Layout:**
```
┌─────────────────────────────────────────────┐ ← FLASH_START_ADDR
│          Bootloader Area                    │
│      (FLASH_APP_FIRST_PAGE pages)           │
├─────────────────────────────────────────────┤ ← FLASH_APP_START_ADDR
│                                             │
│          Application Code & Data            │
│                                             │
│         (CRC calculated over this)          │
│                                             │
├─────────────────────────────────────────────┤ ← FLASH_APP_CRC_VALUE_ADDRESS
│  Stored CRC-32 (4 bytes)                   │
└─────────────────────────────────────────────┘ ← FLASH_START_ADDR + FLASH_SIZE
```

**Important:** The application binary size must not exceed:
```
Max App Size = FLASH_SIZE - (FLASH_APP_FIRST_PAGE × FLASH_PAGE_SIZE) - 4 bytes
```

The 4-byte reduction accounts for the CRC storage at the end of flash.

## Step 2: Hardware Interface Implementation

Implement all required functions from the `franklyboot::hwi` namespace in a `bootloader_api.cpp` file.

### Required Functions

#### Device Management

```cpp
void hwi::resetDevice() {
    // Platform-specific device reset
    NVIC_SystemReset();  // Example for ARM Cortex-M
}
```

#### Device Identification

```cpp
[[nodiscard]] uint32_t hwi::getVendorID() {
    return __DEVICE_IDENT__[DEV_IDENT_VENDOR_ID];
}

[[nodiscard]] uint32_t hwi::getProductID() {
    return __DEVICE_IDENT__[DEV_IDENT_PRODUCT_ID];
}

[[nodiscard]] uint32_t hwi::getProductionDate() {
    return __DEVICE_IDENT__[DEV_IDENT_PRODUCTION_DATE];
}

[[nodiscard]] uint32_t hwi::getUniqueIDWord(const uint32_t idx) {
    // Read from device-specific UID register
    uint32_t* device_uid_ptr = (uint32_t*)(UID_BASE);

    if (idx < 3U) {
        return device_uid_ptr[idx];
    }
    return 0U;
}
```

**Note**: Device identification (VID, PID, production date) should be stored in a special flash section that gets written during production. The unique ID typically comes from hardware registers.

#### CRC Calculation

The bootloader uses CRC-32 to validate the application firmware. The calculated CRC must match the stored CRC value for the application to be considered valid and safe to start.

**Hardware CRC Example (STM32):**

```cpp
uint32_t hwi::calculateCRC(uint32_t src_address, uint32_t num_bytes) {
    // Use hardware CRC peripheral if available
    SET_BIT(CRC->CR, CRC_CR_RESET);

    const uint32_t num_words = num_bytes >> 2u;
    uint32_t* data_ptr = (uint32_t*)src_address;

    for (uint32_t idx = 0u; idx < num_words; idx++) {
        const uint32_t value = *(data_ptr);
        CRC->DR = __REV(value);  // Reverse byte order if needed
        data_ptr++;
    }

    return ~CRC->DR;
}
```

**Software CRC-32 Implementation (for platforms without hardware CRC):**

For platforms like RP2040 that lack a hardware CRC peripheral, use this software implementation:

```cpp
// Software CRC-32 lookup table and initialization
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void init_crc32_table() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;  // CRC-32 polynomial
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_initialized = true;
}

uint32_t hwi::calculateCRC(uint32_t src_address, uint32_t num_bytes) {
    // Initialize lookup table on first use
    if (!crc32_table_initialized) {
        init_crc32_table();
    }

    uint32_t crc = 0xFFFFFFFF;
    uint8_t* data_ptr = (uint8_t*)src_address;

    for (uint32_t i = 0; i < num_bytes; i++) {
        uint8_t byte = data_ptr[i];
        crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFF];
    }

    return ~crc;
}
```

**Important Notes:**
- The CRC algorithm must use the same polynomial (0xEDB88320) as the host flashing tool
- Both hardware and software implementations must produce identical results
- The CRC is calculated over the entire application area (from `FLASH_APP_START_ADDR` to `FLASH_APP_CRC_VALUE_ADDRESS - 1`)

#### Flash Operations

```cpp
bool hwi::eraseFlashPage(uint32_t page_id) {
    // 1. Unlock flash
    // 2. Configure page erase mode
    // 3. Start erase operation
    // 4. Wait for completion
    // 5. Lock flash

    // Example for STM32
    FLASH->KEYR = 0x45670123U;
    FLASH->KEYR = 0xCDEF89ABU;

    SET_BIT(FLASH->CR, FLASH_CR_PER);

    uint32_t addr = device::FLASH_START_ADDR + (page_id * device::FLASH_PAGE_SIZE);
    WRITE_REG(FLASH->AR, addr);

    SET_BIT(FLASH->CR, FLASH_CR_STRT);

    while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_LOCK;

    return true;
}

bool hwi::writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id,
                                  uint8_t* src_data_ptr, uint32_t num_bytes) {
    // 1. Validate data size (must be aligned to write granularity)
    // 2. Unlock flash
    // 3. Enable programming mode
    // 4. Write data
    // 5. Wait for completion
    // 6. Lock flash

    // Check alignment (example: 8-byte alignment for STM32)
    if ((num_bytes % 8) != 0) {
        return false;
    }

    FLASH->KEYR = 0x45670123U;
    FLASH->KEYR = 0xCDEF89ABU;

    FLASH->CR |= FLASH_CR_PG;

    uint16_t* dst_ptr = (uint16_t*)dst_address;
    uint16_t* src_ptr = (uint16_t*)src_data_ptr;

    for (uint32_t i = 0; i < num_bytes / 2; i++) {
        *dst_ptr = *src_ptr;
        while ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
        dst_ptr++;
        src_ptr++;
    }

    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;

    return true;
}

[[nodiscard]] uint8_t hwi::readByteFromFlash(uint32_t flash_src_address) {
    uint8_t* flash_src_ptr = (uint8_t*)flash_src_address;
    return *flash_src_ptr;
}
```

#### Application Start

```cpp
void hwi::startApp(uint32_t app_flash_address) {
    // 1. Disable interrupts
    __disable_irq();

    // 2. Clear pending interrupts
    NVIC->ICPR[0] = 0xFFFFFFFFu;

    // 3. Get application entry point from vector table
    void (*App)(void) = (void (*)(void))(*((uint32_t*)(app_flash_address + 4u)));

    // 4. Disable SysTick
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    // 5. Set Main Stack Pointer
    __set_MSP(*(uint32_t*)app_flash_address);

    // 6. Update Vector Table Offset Register
    SCB->VTOR = app_flash_address;

    // 7. Enable interrupts and jump to app
    __enable_irq();
    App();
}
```

## Step 3: Communication Layer

Implement message transmission and reception for your chosen communication interface.

### UART Example (STM32)

```cpp
static void waitForMessage(msg::Msg& request) {
    std::array<std::uint8_t, 8> buffer;
    uint32_t buffer_idx = 0U;

    while (buffer_idx < buffer.size()) {
        if ((USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE) {
            buffer[buffer_idx] = USART2->RDR;
            buffer_idx++;
        }
    }

    // Decode message
    const uint16_t rx_request_raw = buffer[0U] | (buffer[1U] << 8U);
    request.request = static_cast<msg::RequestType>(rx_request_raw);
    request.result = static_cast<msg::ResultType>(buffer[2U]);
    request.packet_id = buffer[3U];
    request.data[0U] = buffer[4U];
    request.data[1U] = buffer[5U];
    request.data[2U] = buffer[6U];
    request.data[3U] = buffer[7U];
}

static void transmitResponse(const msg::Msg& response) {
    std::array<std::uint8_t, 8> buffer;

    buffer[0U] = static_cast<uint8_t>(response.request);
    buffer[1U] = static_cast<uint8_t>(response.request >> 8U);
    buffer[2U] = static_cast<uint8_t>(response.result);
    buffer[3U] = response.packet_id;
    buffer[4U] = response.data.at(0);
    buffer[5U] = response.data.at(1);
    buffer[6U] = response.data.at(2);
    buffer[7U] = response.data.at(3);

    for (auto byte : buffer) {
        USART2->TDR = byte;
        while (!((USART2->ISR & USART_ISR_TXE) == USART_ISR_TXE));
    }
}
```

### USB CDC Example (RP2040)

For USB communication, use inter-core communication with FIFOs:

```cpp
// Core1 handles USB CDC communication
void core1_main() {
    while (1) {
        // Read from USB and push to RX FIFO
        if (tud_cdc_available()) {
            uint8_t byte = tud_cdc_read_char();
            rxFifoPush(byte);
        }

        // Pop from TX FIFO and write to USB
        uint8_t tx_byte;
        if (txFifoPop(tx_byte)) {
            tud_cdc_write_char(tx_byte);
            tud_cdc_write_flush();
        }

        tud_task();
    }
}

// Core0 handles bootloader logic
void core0_bootloader() {
    // Read from RX FIFO, process, write to TX FIFO
}
```

## Step 4: Bootloader Entry and Auto-Start

### Auto-Start Mechanism

```cpp
void FRANKLYBOOT_Run(void) {
    Handler<device::FLASH_START_ADDR, device::FLASH_APP_FIRST_PAGE,
            device::FLASH_SIZE, device::FLASH_PAGE_SIZE> hBootloader;

    // Check if autostart should be disabled
    const bool autostart_disable = checkAutoStartOverride();

    // Autostart is possible if valid app exists
    autostart_possible = hBootloader.isAppValid() && !autostart_disable;

    for (;;) {
        msg::Msg request;
        hBootloader.processBufferedCmds();
        waitForMessage(request);

        // Abort autostart on bootloader ping
        if (autostart_possible &&
            (request.request == msg::REQ_PING ||
             request.request == msg::REQ_DEV_INFO_BOOTLOADER_VERSION)) {
            autostart_possible = false;
        }

        hBootloader.processRequest(request);
        const auto response = hBootloader.getResponse();
        transmitResponse(response);
    }
}

void FRANKLYBOOT_autoStartISR(void) {
    if (autostart_possible) {
        req_autostart = true;  // Set flag to start app
    }
}
```

### Auto-Start Override

Allow the application to disable auto-start by writing a key to backup RAM:

```cpp
constexpr uint32_t AUTOBOOT_DISABLE_KEY = 0xDEADBEEFU;

// In application code:
void disableBootloaderAutoStart() {
    RTC->BKP0R = AUTOBOOT_DISABLE_KEY;
    NVIC_SystemReset();
}

// In bootloader:
const bool autostart_disable = (RTC->BKP0R == AUTOBOOT_DISABLE_KEY);
RTC->BKP0R = 0;  // Clear flag
```

## Step 5: Linker Script Configuration

### Bootloader Linker Script

Reserve flash space for the bootloader and define the device identification section:

```ld
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 8K   /* Bootloader area */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 12K
}

SECTIONS
{
    /* Device identification section at fixed location */
    ._dev_ident 0x08001F00 :
    {
        KEEP(*(.\_dev_ident))
    } > FLASH

    .text :
    {
        /* ... standard text section ... */
    } > FLASH

    /* ... other sections ... */
}
```

### Application Linker Script

Configure the application to start after the bootloader:

```ld
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08002000, LENGTH = 56K  /* Start after bootloader */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 12K
}

SECTIONS
{
    /* Reserve last 4 bytes for CRC */
    .text :
    {
        /* ... vectors and code ... */
    } > FLASH

    /* CRC stored at end of flash */
    ._app_crc :
    {
        KEEP(*(.\_app_crc))
    } > FLASH
}
```

## Step 6: Bootloader Initialization

### Main Function

```cpp
extern "C" void FRANKLYBOOT_Init(void) {
    // Initialize hardware peripherals
    // - Configure clocks
    // - Initialize communication interface (UART/CAN/USB)
    // - Enable CRC peripheral if available
    // - Configure SysTick for auto-start timing
}

extern "C" void FRANKLYBOOT_Run(void) {
    // See Step 4 for implementation
}

extern "C" uint32_t FRANKLYBOOT_getDevSysTickHz(void) {
    return device::SYS_TICK;
}

int main(void) {
    FRANKLYBOOT_Init();
    FRANKLYBOOT_Run();  // Never returns
    return 0;
}
```

## Platform-Specific Considerations

### STM32 Platforms

- Use HAL or register-based flash programming
- Hardware CRC peripheral typically available (use STM32 CRC32)
- UID available at `UID_BASE` (usually 96 bits = 3 words)
- Backup registers (RTC->BKPxR) for persistent flags

### RP2040 Platform

- Use Pico SDK flash functions (`flash_range_erase`, `flash_range_program`)
- Implement software CRC-32
- UID available via `pico_unique_id` API (64 bits stored in flash)
- Use watchdog scratch registers for persistent flags
- Flash operations must disable interrupts and may require core synchronization

### ARM Cortex-M General

- Vector table at application start must contain valid stack pointer and reset handler
- Use `SCB->VTOR` to relocate vector table before jumping to application
- Clear all pending interrupts before application start
- Disable SysTick to prevent bootloader timer from interfering

## Example Projects

Complete reference implementations are available in the [frankly-bootloader-examples](https://github.com/franc0r/frankly-bootloader-examples) repository:

- **STM32F303K8** - UART-based bootloader for Nucleo-32 board
- **STM32G431RB** - UART-based bootloader for Nucleo-64 board
- **STM32L431KB** - CAN-based bootloader for custom board
- **RP2040 Pico** - USB CDC-based bootloader with dual-core architecture

Each example includes:
- Complete `device_defines.h` configuration
- Full `bootloader_api.cpp` implementation
- Linker scripts for bootloader and application
- Example application demonstrating bootloader override

## Further Resources

- [Architecture Overview](./architecture.md) - Detailed bootloader architecture
- [Communication Protocol](./protocol/README.md) - Protocol specification
- [Development Guide](./development.md) - Building and testing
