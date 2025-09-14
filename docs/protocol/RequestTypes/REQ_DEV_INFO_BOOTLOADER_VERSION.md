# REQ_DEV_INFO_BOOTLOADER_VERSION

## Description

Read the bootloader version of device

## Protocol / Data encoding

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_BOOTLOADER_VERSION|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_BOOTLOADER_VERSION|RES_OK|0|BOOTL_VER_MAJOR|BOOTL_VER_MINOR|BOOTL_VER_PATCH|-|

## Errors

No errors possible

## Example
 
```C++
// Request send to device
const uint8_t reqMsg[] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_BOOTLOADER_VERSION = 0x0101U
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: Bootloader Version 1.2.10 (MAJOR.MINOR.PATCH)
const uint8_t respMsg[] = {0x01, 0x01, 0x01, 0x00, 0x01, 0x02, 0x0A, 0x00};

```