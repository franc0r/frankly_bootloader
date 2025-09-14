# Request Types

The following table shows the supported request types. To get detailed information click the link on the sidebar


| Enum                                  | ID (Hex) | Short Description                                                  | Implemented | Tested |
|---------------------------------------|----------|--------------------------------------------------------------------|-------------|--------|
| REQ_PING                              | 0x0001   | Network search via sync for multi device networks                  | yes         | yes    |
| REQ_RESET_DEVICE                      | 0x0011   | Resets the device                                                  | yes         | yes    |
| REQ_START_APP                         | 0x0012   | Starts the application                                             | yes         | yes    |
| **Device Information**                |  
| REQ_DEV_INFO_BOOTLOADER_VERSION       | 0x0101   | Reads the used version of the bootloader                           | yes         | yes    |
| REQ_DEV_INFO_BOOTLOADER_CRC           | 0x0102   | Reads the CRC value of the bootloader (calculated)                 | yes         | yes    |
| REQ_DEV_INFO_VID                      | 0x0103   | Reads the vendor ID                                                | yes         | yes    |
| REQ_DEV_INFO_PID                      | 0x0104   | Reads the product ID                                               | yes         | yes    |
| REQ_DEV_INFO_PRD                      | 0x0105   | Reads the production date of the device (seconds since 1.12.2023)  | yes         | yes    |
| REQ_DEV_INFO_UID_1                    | 0x0106   | Reads unique ID bits [0:31] of the device                         | yes         | yes    |
| REQ_DEV_INFO_UID_2                    | 0x0107   | Reads unique ID bits [32:63] of the device                        | yes         | yes    |
| REQ_DEV_INFO_UID_3                    | 0x0108   | Reads unique ID bits [64:95] of the device                        | yes         | yes    |
| REQ_DEV_INFO_UID_4                    | 0x0109   | Reads unique ID bits [96:127] of the device                       | yes         | yes    |
| **Flash Information**                 |  
| REQ_FLASH_INFO_START_ADDR             | 0x0201   | Reads the start address of the flash e.g. (0x08000000) for STM     | yes         | yes    |
| REQ_FLASH_INFO_PAGE_SIZE              | 0x0202   | Reads the page size of the flash                                   | yes         | yes    |
| REQ_FLASH_INFO_NUM_PAGES              | 0x0203   | Reads the number of flash pages                                    | yes         | yes    |
| **App Information**                   |  
| REQ_APP_INFO_PAGE_IDX                 | 0x0301   | Reads the first page of the application flash                      | yes         | yes    |
| REQ_APP_INFO_CRC_CALC                 | 0x0302   | Calculate CRC of application flash                                 | yes         | yes    |
| REQ_APP_INFO_CRC_STRD                 | 0x0303   | Reads the stored CRC value in application flash                    | yes         | yes    |
| **Flash Read Commands**               |  
| REQ_FLASH_READ_WORD                   | 0x0401   | Read a word from flash at desired address                          | yes         | yes    |
| **Page Buffer Commands**              |  
| REQ_PAGE_BUFFER_CLEAR                 | 0x1001   | Clears the page buffer in RAM used for flashing                    | yes         | yes    |
| REQ_PAGE_BUFFER_READ_WORD             | 0x1002   | Reads a word from the page buffer in RAM                           | yes         | yes    |
| REQ_PAGE_BUFFER_WRITE_WORD            | 0x1003   | Writes a word to the page buffer in RAM                            | yes         | yes    |
| REQ_PAGE_BUFFER_CALC_CRC              | 0x1004   | Calculates the CRC value for the page buffer                       | yes         | yes    |
| REQ_PAGE_BUFFER_WRITE_TO_FLASH        | 0x1005   | Writes the complete page buffer to the flash                       | yes         | yes    |
| ** Flash Write Commands**                    |  
| REQ_FLASH_WRITE_ERASE_PAGE            | 0x1101   | Erase flash page                                                   | yes         | yes    |
| REQ_FLASH_WRITE_APP_CRC               | 0x1102   | Writes the desired CRC value to the flash for app checking         | yes         | yes    |
  