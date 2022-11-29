# REQ_DEV_INFO_BOOTLOADER_CRC

## Description

Calculates the CRC of the bootloader flash area and returns it in the response

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_BOOTLOADER_CRC|RESP_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_BOOTLOADER_CRC|RESP_ACK|0|CRC_0|CRC_1|CRC_2|CRC_3|

*Data encoding*

u32 = (CRC_0) | (CRC_1 << 8) | (CRC_2 << 16) | (CRC_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_BOOTLOADER_CRC = 0x0102U
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: CRC Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x02, 0x01, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```