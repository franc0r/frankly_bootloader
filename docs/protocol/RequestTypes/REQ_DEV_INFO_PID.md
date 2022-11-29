# REQ_DEV_INFO_PID

## Description

Reads the product ID of the device

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_PID|RESP_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_PID|RESP_ACK|0|PID_0|PID_1|PID_2|PID_3|

*Data encoding*
u32 = (PID_0) | (PID_1 << 8) | (PID_2 << 16) | (PID_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_PID = 0x0104U
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: PID_Value = 0xDEADBEEF
const uint8_t respMsg[] = {0x04, 0x01, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};

```