# REQ_APP_INFO_CRC_CALC

## Description

Calculates the CRC value of the complete application area, expect the address where the CRC value is stored.

This means if the app does not use the complete flash all the unused areas are used for CRC calculation!

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_APP_INFO_CRC_CALC|RESP_NONE|0|-|-|-|-|
|Response|REQ_APP_INFO_CRC_CALC|RESP_ACK|0|CRC_0|CRC_1|CRC_2|CRC_3|

*Data encoding*

u32 = (CRC_0) | (CRC_1 << 8) | (CRC_2 << 16) | (CRC_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_APP_INFO_CRC_CALC = 0x0302
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: CRC_Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x02, 0x03, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```