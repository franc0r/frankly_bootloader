# REQ_DEV_INFO_PRD

## Description

Reads the production date of the device. The 32-bit number contains the seconds, which has been passed
since the 1.12.2022. 

## Protocol / Data encoding

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_PRD|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_PRD|RES_OK|0|PRD_0|PRD_1|PRD_2|PRD_3|

*Data encoding*

u32 = (PRD_0) | (PRD_1 << 8) | (PRD_2 << 16) | (PRD_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_PRD = 0x0104U
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: PRD_Value = 0x7EA4 = 32420 sec = 01. Dec. 2022 - 9:00:20
const uint8_t respMsg[] = {0x05, 0x01, 0x01, 0x00, 0xA4, 0x7E, 0x00, 0x00};

```