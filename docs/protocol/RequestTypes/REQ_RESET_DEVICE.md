# REQ_RESET_DEVICE

## Description

Request/force a hardware device reset.

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_RESET_DEVICE|RESP_NONE|0|-|-|-|-|
|Response|REQ_RESET_DEVICE|RESP_ACK|0|-|-|-|-|

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_RESET_DEVICE = 0x0011U
// ResponseType: RESP_ACK = 0x01
const uint8_t respMsg[] = {0x11, 0x00, 0x01, 0x00, 0x01, 0x02, 0x0A, 0x00};

```