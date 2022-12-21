# REQ_FLASH_INFO_START_ADDR

## Description

Reads the start address of the flash area. For example the flash of a STM32 device starts at 0x0800_0000.
The bootloader starts at this address, because applications has to be placed behind the bootloader.

## Protocol / Data encoding

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_FLASH_INFO_START_ADDR|RES_NONE|0|-|-|-|-|
|Response|REQ_FLASH_INFO_START_ADDR|RES_OK|0|ADDR_W0|ADDR_1|ADDR_2|ADDR_3|

*Data encoding*

u32 = (ADDR_0) | (ADDR_1 << 8) | (ADDR_2 << 16) | (ADDR_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_FLASH_INFO_START_ADDR = 0x0201U
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: ADDR_Value = 0x08000000
const uint8_t respMsg[] = {0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08};

```