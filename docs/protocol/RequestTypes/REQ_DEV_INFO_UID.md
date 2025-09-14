# REQ_DEV_INFO_UID_* (128-bit Unique ID)

## Description

Reads the 128-bit unique ID of the device. The 128-bit UID is split into four 32-bit words, each accessible through separate requests:

- **REQ_DEV_INFO_UID_1** (0x0106): Bits [0:31] of the UID
- **REQ_DEV_INFO_UID_2** (0x0107): Bits [32:63] of the UID
- **REQ_DEV_INFO_UID_3** (0x0108): Bits [64:95] of the UID
- **REQ_DEV_INFO_UID_4** (0x0109): Bits [96:127] of the UID

## Protocol / Data encoding

### REQ_DEV_INFO_UID_1 (Bits 0-31)

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_UID_1|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_UID_1|RES_OK|0|UID[0]|UID[1]|UID[2]|UID[3]|

### REQ_DEV_INFO_UID_2 (Bits 32-63)

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_UID_2|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_UID_2|RES_OK|0|UID[4]|UID[5]|UID[6]|UID[7]|

### REQ_DEV_INFO_UID_3 (Bits 64-95)

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_UID_3|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_UID_3|RES_OK|0|UID[8]|UID[9]|UID[10]|UID[11]|

### REQ_DEV_INFO_UID_4 (Bits 96-127)

| Direction | Request Type | Result Type | Packet ID | Data[0] | Data[1] | Data[2] | Data [3] |
|-|-|-|-|-|-|-|-|
|Request|REQ_DEV_INFO_UID_4|RES_NONE|0|-|-|-|-|
|Response|REQ_DEV_INFO_UID_4|RES_OK|0|UID[12]|UID[13]|UID[14]|UID[15]|

### Data encoding

Each 32-bit word is encoded as:
```
u32 = (UID_byte_n) | (UID_byte_n+1 << 8) | (UID_byte_n+2 << 16) | (UID_byte_n+3 << 24)
```

## Complete 128-bit UID Assembly

To reconstruct the complete 128-bit UID, combine all four 32-bit words:

```cpp
uint32_t uid_word_1 = /* from REQ_DEV_INFO_UID_1 */;  // Bits [0:31]
uint32_t uid_word_2 = /* from REQ_DEV_INFO_UID_2 */;  // Bits [32:63]
uint32_t uid_word_3 = /* from REQ_DEV_INFO_UID_3 */;  // Bits [64:95]
uint32_t uid_word_4 = /* from REQ_DEV_INFO_UID_4 */;  // Bits [96:127]

// Complete 128-bit UID as byte array
uint8_t uid_128[16] = {
    // Word 1 (bits 0-31)
    (uint8_t)(uid_word_1 & 0xFF),
    (uint8_t)((uid_word_1 >> 8) & 0xFF),
    (uint8_t)((uid_word_1 >> 16) & 0xFF),
    (uint8_t)((uid_word_1 >> 24) & 0xFF),
    // Word 2 (bits 32-63)
    (uint8_t)(uid_word_2 & 0xFF),
    (uint8_t)((uid_word_2 >> 8) & 0xFF),
    (uint8_t)((uid_word_2 >> 16) & 0xFF),
    (uint8_t)((uid_word_2 >> 24) & 0xFF),
    // Word 3 (bits 64-95)
    (uint8_t)(uid_word_3 & 0xFF),
    (uint8_t)((uid_word_3 >> 8) & 0xFF),
    (uint8_t)((uid_word_3 >> 16) & 0xFF),
    (uint8_t)((uid_word_3 >> 24) & 0xFF),
    // Word 4 (bits 96-127)
    (uint8_t)(uid_word_4 & 0xFF),
    (uint8_t)((uid_word_4 >> 8) & 0xFF),
    (uint8_t)((uid_word_4 >> 16) & 0xFF),
    (uint8_t)((uid_word_4 >> 24) & 0xFF),
};
```

## Errors

No errors possible for these requests.

## Examples

### Reading UID Word 1 (Bits 0-31)
```cpp
// Request send to device
const uint8_t reqMsg[] = {0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_UID_1 = 0x0106U
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: UID_Word_1 = 0xDEADBEEF
const uint8_t respMsg[] = {0x06, 0x01, 0x01, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};
```

### Reading UID Word 2 (Bits 32-63)
```cpp
// Request send to device
const uint8_t reqMsg[] = {0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Response received from device
// RequestType: REQ_DEV_INFO_UID_2 = 0x0107U
// ResponseType: RES_OK = 0x01
// Packet-ID: 0
// Data: UID_Word_2 = 0x12345678
const uint8_t respMsg[] = {0x07, 0x01, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12};
```

### Complete UID Reading Sequence
```cpp
// Read all four UID words to get complete 128-bit identifier
// This would result in UID: 0x12345678DEADBEEF9ABCDEF0FEDCBA98
// (assuming word 3 = 0x9ABCDEF0, word 4 = 0xFEDCBA98)
```