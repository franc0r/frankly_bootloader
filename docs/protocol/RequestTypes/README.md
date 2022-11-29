# Request Types

The following table shows the supported request types. To get detailed information click the link on the sidebar


| Enum                                  | ID (Hex) | Short Description                                                | Implemented | Tested |
|---------------------------------------|----------|------------------------------------------------------------------|-------------|--------|
| REQ_PING                              | 0x0001   | Network search via sync for multi device networks                | yes         | yes    |
| REQ_RESET_DEVICE                      | 0x0011   | Resets the device                                                | yes         | yes    |
| REQ_START_APP                         | 0x0012   | Starts the application                                           | yes         | yes     |
| **Device Information**                |
| REQ_DEV_INFO_BOOTLOADER_VERSION       | 0x0101   | Reads the used version of the bootloader                         | no          | no     |
| REQ_DEV_INFO_BOOTLOADER_CRC           | 0x0102   | Reads the CRC value of the bootloader (calculated)               | no          | no     |
| REQ_DEV_INFO_VID                      | 0x0103   | Reads the vendor ID                                              | no          | no     |
| REQ_DEV_INFO_PID                      | 0x0104   | Reads the product ID                                             | no          | no     |
| REQ_DEV_INFO_PRD                      | 0x0105   | Reads the production date of the device                          | no          | no     |
| REQ_DEV_INFO_UID                      | 0x0106   | Reads the unique ID of the device                                | no          | no     |
| **Flash Information**                 |
| REQ_FLASH_INFO_START_ADDR             | 0x0201   | Reads the start address of the flash e.g. (0x08000000) for STM   | no          | no     |
| REQ_FLASH_INFO_PAGE_SIZE              | 0x0202   | Reads the page size of the flash                                 | no          | no     |
| REQ_FLASH_INFO_NUM_PAGES              | 0x0203   | Reads the number of flash pages                                  | no          | no     |
| **App Information**                   |
| REQ_APP_INFO_PAGE_START_IDX           | 0x0301   | Reads the first page of the application flash                    | no          | no     |
| REQ_APP_INFO_CALC_CRC                 | 0x0302   | Calculate CRC of application flash                               | no          | no     |
| REQ_APP_INFO_FLASH_CRC                | 0x0303   | Reads the stored CRC value in application flash                  | no          | no     |
| **Flash Read Commands**               |
| REQ_FLASH_READ_WORD                   | 0x0401   | Read a word from flash at desired address                        | no          | no     |
| **Page Buffer Commands**              |
| REQ_PAGE_BUFFER_CLEAR                 | 0x1001   | Clears the page buffer in RAM used for flashing                  | no          | no     |
| REQ_PAGE_BUFFER_WRITE_WORD            | 0x1002   | Writes a word to the page buffer in RAM                          | no          | no     |
| REQ_PAGE_BUFFER_CALC_CRC              | 0x1003   | Calculates the CRC value for the page buffer                     | no          | no     |
| REQ_PAGE_BUFFER_WRITE_TO_FLASH        | 0x1004   | Writes the complete page buffer to the flash                     | no          | no     |
| **Write Commands**                    |
| REQ_WRITE_ERASE_UNSAFE                | 0x1101   | Erase complete arease including bootloader area                  | no          | no     |
| REQ_WRITE_APP_ERASE                   | 0x1102   | Erase the flash area containing the app                          | no          | no     |
| REQ_WRITE_APP_WRITE_CRC               | 0x1103   | Writes the desired CRC value to the flash for app checking       | no          | no     |
