# Communication Protocol

This section describes the communication protocol used between the bootloader and the host pc. 

The communication of the bootloader is based on the CAN protocol which allows 8 bytes of payload data and one 11-bit message identifier. To keep the bootloader protocol open for other bus systems it only cares about the payload but not about node or message identification within a bus system. This has to be done by the hardware layer.

## Message Layout

A message consists of four elements:

| Type          | Byte-Size | Description                                          |
|---------------|-----------|------------------------------------------------------|
| Request Type  | 2         | What does the bootloader have to do?                 |
| Result Type | 1         | What has the bootloader done with the request        |
| Packet ID     | 1         | ID of the packet if multiple data is send / received |
| Data          | 4         | Payload data                                         |


The message looks always the same for a request (host to bootloader) and a response (bootloader to host).
Important: The communication is always triggered by the host! The bootloader will never send messages without a request!

### Example

An example how a simple request and response looks like:
```
// '-' means that the data is sent, but content is not cared
| Direction        | Request Type | Result Type | Packet-ID | Data               |
|------------------|--------------|---------------|-----------|--------------------|
| (Host -> Client) | REQ_PING     | -             | -         | -                  |
| (Client -> Host) | REQ_PING     | RESP_ACK      | 0         | BOOTLOADER-VERSION | 
```

This shall give a basic understanding of the communication protocol. Detailed information is located within the sub-chapters, which can be accessed from within the sidebar.

### Packaging / Data Layout

The data layout is done as followed:
```C++
uint16_t RequestType  = 0x1020U;        // Example request
uint8_t  ResponseType = 0x02U;          // Example response
uint8_t  PacketID     = 0x00U;          // Example packet ID
uint32_t DataWord     = 0xDEADBEEF;     // Example data word
uint8_t  DataByteArray[4] = {0xEF, 0xBE, 0xAD, 0xDE}; // Alternative to data word

// Packaging in message
uint8_t msg[8U];
msg[0] = static_cast<uint8_t>(RequestType);
msg[1] = static_cast<uint8_t>(RequestType >> 8U);
msg[2] = static_cast<uint8_t>(ResponseType);
msg[3] = static_cast<uint8_t>(PacketID);

// Packaging of DataWord
msg[4] = static_cast<uint8_t>(DataWord);
msg[5] = static_cast<uint8_t>(DataWord >>  8U);
msg[6] = static_cast<uint8_t>(DataWord >> 16U);
msg[7] = static_cast<uint8_t>(DataWord >> 24U);

// Packaging of DataByteArray
msg[4] = DataByteArray[0U];
msg[5] = DataByteArray[1U];
msg[6] = DataByteArray[2U];
msg[7] = DataByteArray[3U];

// Array Data of MSG
{0x20, 0x10, 0x02, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};
```
