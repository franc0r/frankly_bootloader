# REQ_APP_INFO_PAGE_IDX

## Description

Reads the first page where the application area in flash is started.
For example:
```
Flash Start = 0x0800_0000
Page Size = 0x400 (1024)
Num Pages = 32

App starts at second page
App page idx = 2

First address in flash where app is started:
App first address = (Flash Start) + (Page Size) * (App page idx) = 0x0800_0000 + 0x400 * 0x02 = 0x0800_0800
```

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_APP_INFO_PAGE_IDX|RESP_NONE|0|-|-|-|-|
|Response|REQ_APP_INFO_PAGE_IDX|RESP_ACK|0|PAGE_IDX_0|PAGE_IDX_1|PAGE_IDX_2|PAGE_IDX_3|

*Data encoding*

u32 = (PAGE_IDX_0) | (PAGE_IDX_1 << 8) | (PAGE_IDX_2 << 16) | (PAGE_IDX_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_APP_INFO_PAGE_IDX = 0x0301
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: PAGE_IDX_Value = 0x02
const uint8_t respMsg[] = {0x01, 0x03, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00};

```