# REQ_FLASH_INFO_NUM_PAGES

## Description


Reads the number of pages of the flash.
The flash is organized with pages, which can have different sizes. 
For example if a flash has a page size of 1024 bytes (1kB) and has 32 pages the total flash size is 1024 * 32 = 32768 bytes (32kB).

## Protocol / Data encoding

| Direction | Request Type | Response Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_FLASH_INFO_NUM_PAGES|RESP_NONE|0|-|-|-|-|
|Response|REQ_FLASH_INFO_NUM_PAGES|RESP_ACK|0|PSIZE_W0|PSIZE_1|PSIZE_2|PSIZE_3|

*Data encoding*

u32 = (PSIZE_0) | (PSIZE_1 << 8) | (PSIZE_2 << 16) | (PSIZE_3 << 24)

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_FLASH_INFO_NUM_PAGES = 0x0203U
// ResponseType: RESP_ACK = 0x01
// Packet-ID: 0
// Data: PSIZE_Value = 0x00000020U
const uint8_t respMsg[] = {0x03, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00};

```