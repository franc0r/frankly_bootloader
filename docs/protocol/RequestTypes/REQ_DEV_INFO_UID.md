# REQ_DEV_INFO_UID

## Description

Reads the unique ID of the device

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_UID|RESP_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_UID|RESP_ACK|0|UID_0|UID_1|UID_2|UID_3|

*Data encoding*

u32 = (UID_0) | (UID_1 << 8) | (UID_2 << 16) | (UID_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_UID = 0x0104U
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: UID_Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x04, 0x01, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```