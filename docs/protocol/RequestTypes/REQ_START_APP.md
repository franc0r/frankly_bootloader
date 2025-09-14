# REQ_START_APP

## Description

Request to exit the bootloader area and start the application.
There are two modes to request an application start:
- Safe-Mode: The application flash is secured with the CRC-32 value. Before startup the CRC-32 value is calculated and compared to the stored value. Only if both values are equal the application is started.
- Unsafe-Mode: The application will start without any CRC-32 check. This can lead to a critical error and an unresponsive device.

## Protocol / Data encoding

### Normal startup (with CRC-32 check):

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_START_APP|RES_NONE|0|-|-|-|-|
|Response|REQ_START_APP|RES_OK|0|-|-|-|-|

#### Errors

- RES_ERR_CRC_INVLD: CRC check failed! Stored and calculated CRC differs!

### Unsafe startup (without CRC-32 check):

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_START_APP|RES_NONE|0|0xFF|0xFF|0xFF|0xFF|
|Response|REQ_START_APP|RES_OK|0|0xFF|0xFF|0xFF|0xFF|

#### Errors

No errors possible

## Example
 
```C++

/* SAFE STARTUP */

// Request send to device
const uint8_t reqMsg[] = {0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_START_APP = 0x0011U
// ResponseType: RES_OK = 0x01
const uint8_t respMsgAck[] = {0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

/* OR */

// Response received from device
// RequestType: REQ_START_APP = 0x0011U
// ResponseType: RES_ERR_CRC_INVLD = 0xFB
const uint8_t respMsgAck[] = {0x12, 0x00, 0xFB, 0x00, 0x00, 0x00, 0x00, 0x00};

/* UNSAFE STARTUP */

// Request send to device
// Send data = 0xFFFFFFFFU
const uint8_t reqMsg[] = {0x12, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};

// Response received from device
// RequestType: REQ_START_APP = 0x0011U
// ResponseType: RES_OK = 0x01
// Data: 0xFFFFFFFFU
const uint8_t respMsgAck[] = {0x12, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};

```