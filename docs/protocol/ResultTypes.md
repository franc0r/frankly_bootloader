# Result Types

## Overview

Result types are returned by the bootloader in response to request messages. They indicate the success or failure status of the requested operation, along with specific error codes when applicable.

All result types are 8-bit values (uint8_t) transmitted in the Result Type field of the message protocol.

## Success Results

| Enum | Value (Hex) | Description |
|------|-------------|-------------|
| RES_NONE | 0x00 | No result / not specified (used in requests) |
| RES_OK | 0x01 | Message was processed successfully / result ok |

## Error Results

| Enum | Value (Hex) | Description |
|------|-------------|-------------|
| RES_ERR | 0xFE | General error |
| RES_ERR_UNKNOWN_REQ | 0xFD | Unknown request type |
| RES_ERR_NOT_SUPPORTED | 0xFC | Error, command known but not supported |
| RES_ERR_CRC_INVLD | 0xFB | Error, CRC check failed |
| RES_ERR_PAGE_FULL | 0xFA | Error, word not writable - page buffer is full |
| RES_ERR_INVLD_ARG | 0xF9 | Error, invalid argument (out of range, etc.) |

## Usage

### In Requests
When sending a request to the bootloader, the Result Type field should be set to `RES_NONE` (0x00).

### In Responses
The bootloader will always set the Result Type field to one of the defined values above:
- `RES_OK` (0x01) for successful operations
- One of the error codes (0xF9-0xFE) for failed operations

## Error Handling

### RES_ERR_UNKNOWN_REQ (0xFD)
This error is returned when the bootloader receives a request type that is not implemented or recognized. This can happen when:
- Using an invalid request type ID
- The bootloader version doesn't support the requested command
- Communication corruption resulted in an invalid request type

### RES_ERR_NOT_SUPPORTED (0xFC)
Returned when a known request type is not supported in the current bootloader configuration or state.

### RES_ERR_CRC_INVLD (0xFB)
Indicates that a CRC verification failed. This can occur during:
- Application verification operations
- Flash programming operations with CRC checking enabled

### RES_ERR_PAGE_FULL (0xFA)
Returned when attempting to write to the page buffer when it's already full. The page buffer must be cleared or written to flash before additional data can be added.

### RES_ERR_INVLD_ARG (0xF9)
Indicates that one or more arguments in the request are invalid:
- Address out of valid flash range
- Invalid page number
- Invalid word index for buffer operations

### RES_ERR (0xFE)
General error code for other types of failures not covered by specific error codes.

## Examples

### Successful Response
```cpp
// Request processed successfully
msg.result = franklyboot::msg::RES_OK;  // 0x01
```

### Error Response
```cpp
// Invalid address provided
msg.result = franklyboot::msg::RES_ERR_INVLD_ARG;  // 0xF9
```