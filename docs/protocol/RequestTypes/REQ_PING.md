# REQ_PING

**Description**

Used in multi device networks to check how many devices are connected.

**Protocol / Data encoding**

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_PING|RESP_NONE|0|-|-|-|-|
|Response|REQ_PING|RESP_ACK|0|BOOTL_VER_MAJOR|BOOTL_VER_MINOR|BOOTL_VER_PATCH|-|

**Errors**

No errors possible

**Example**
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_PING = 0x0000
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: Bootloader Version 1.2.10 (MAJOR.MINOR.PATCH)
const uint8_t respMsg[] = {0x00, 0x00, 0x01, 0x00, 0x01, 0x02, 0x0A, 0x00};

```