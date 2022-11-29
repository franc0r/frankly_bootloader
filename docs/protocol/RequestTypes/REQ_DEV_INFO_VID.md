# REQ_DEV_INFO_VID

## Description

Reads the vendor ID of the device

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_VID|RESP_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_VID|RESP_ACK|0|VID_0|VID_1|VID_2|VID_3|

*Data encoding*
u32 = (VID_0) | (VID_1 << 8) | (VID_2 << 16) | (VID_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_VID = 0x0103U
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: VID_Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x03, 0x01, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```