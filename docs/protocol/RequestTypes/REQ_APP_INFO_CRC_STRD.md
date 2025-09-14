# REQ_APP_INFO_CRC_CALC

## Description

Reads the stored CRC value of the application from the flash.
The CRC value is stored at the last 4 bytes of the application flash!

```
APP_CRC_ADDRESS = FLASH_START + FLASH_SIZE - 4U;
```

## Protocol / Data encoding

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_APP_INFO_CRC_CALC|RES_NONE|0|-|-|-|-|
|Response|REQ_APP_INFO_CRC_CALC|RES_OK|0|CRC_0|CRC_1|CRC_2|CRC_3|

*Data encoding*

u32 = (CRC_0) | (CRC_1 << 8) | (CRC_2 << 16) | (CRC_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_APP_INFO_CRC_CALC = 0x0303
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: CRC_Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x03, 0x03, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```