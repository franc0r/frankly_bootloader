/**
 * @file handler.tpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Bootloader Handler Class Template Deklarations
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include "handler.h"

#ifndef FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_
#define FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_

namespace franklyboot {

/** \brief Define for the template definition for better readibility */
#define FRANKLYBOOT_HANDLER_TEMPL \
  template <uint32_t FLASH_START, uint32_t FLASH_APP_FIRST_PAGE, uint32_t FLASH_SIZE, uint32_t FLASH_PAGE_SIZE>

/** \brief Prefix of template functions for better readability */
#define FRANKLYBOOT_HANDLER_TEMPL_PREFIX Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE>

// Public Functions ---------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
FRANKLYBOOT_HANDLER_TEMPL_PREFIX::Handler() {
  this->_page_buffer.fill({std::numeric_limits<uint8_t>::max()});
  this->_page_buffer_pos = 0U;
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::processBufferedCmds() {
  switch (this->_cmd_buffer) {
    case CommandBuffer::NONE:
      break;
    case CommandBuffer::RESET_DEVICE:
      hwi::resetDevice();
      break;
    case CommandBuffer::START_APP:
      const uint32_t app_flash_address = FLASH_START + FLASH_PAGE_SIZE * FLASH_APP_FIRST_PAGE;
      hwi::startApp(app_flash_address);
      break;
  }

  _cmd_buffer = CommandBuffer::NONE;
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::processRequest(const msg::Msg& msg) {
  /* First response is always an error as long as request is not handeld*/
  this->_response = msg;
  this->_response.result = msg::RES_ERR;

  switch (msg.request) {
    case msg::REQ_PING:
      handleReqPing();
      break;

    case msg::REQ_RESET_DEVICE:
      handleReqResetDevice();
      break;

    case msg::REQ_START_APP:
      handleReqStartApp(msg);
      break;

    case msg::REQ_DEV_INFO_BOOTLOADER_VERSION:
      handleReqInfoBootloaderVer();
      break;

    case msg::REQ_DEV_INFO_BOOTLOADER_CRC:
      handleReqInfoBootloaderCRC();
      break;

    case msg::REQ_DEV_INFO_VID:
      handleReqInfoVendorID();
      break;

    case msg::REQ_DEV_INFO_PID:
      handleReqInfoProductID();
      break;

    case msg::REQ_DEV_INFO_PRD:
      handleReqInfoProductionDate();
      break;

    case msg::REQ_DEV_INFO_UID_1:
      handleReqInfoUniqueID(msg::REQ_DEV_INFO_UID_1);
      break;

    case msg::REQ_DEV_INFO_UID_2:
      handleReqInfoUniqueID(msg::REQ_DEV_INFO_UID_2);
      break;

    case msg::REQ_DEV_INFO_UID_3:
      handleReqInfoUniqueID(msg::REQ_DEV_INFO_UID_3);
      break;

    case msg::REQ_DEV_INFO_UID_4:
      handleReqInfoUniqueID(msg::REQ_DEV_INFO_UID_4);
      break;

    case msg::REQ_FLASH_INFO_START_ADDR:
      handleReqFlashStartAddress();
      break;

    case msg::REQ_FLASH_INFO_PAGE_SIZE:
      handleReqFlashPageSize();
      break;

    case msg::REQ_FLASH_INFO_NUM_PAGES:
      handleReqFlashNumPages();
      break;

    case msg::REQ_APP_INFO_PAGE_IDX:
      handleReqAppPageIdx();
      break;

    case msg::REQ_APP_INFO_CRC_CALC:
      handleReqAppCrcCalc();
      break;

    case msg::REQ_APP_INFO_CRC_STRD:
      handleReqAppCrcStrd();
      break;

    case msg::REQ_FLASH_READ_WORD:
      handleReqFlashReadWord(msg);
      break;

    case msg::REQ_PAGE_BUFFER_CLEAR:
      handleReqPageBufferClear();
      break;

    case msg::REQ_PAGE_BUFFER_READ_WORD:
      handleReqPageBufferReadWord(msg);
      break;

    case msg::REQ_PAGE_BUFFER_WRITE_WORD:
      handleReqPageBufferWriteWord(msg);
      break;

    case msg::REQ_PAGE_BUFFER_CALC_CRC:
      handleReqPageBufferCalcCrc();
      break;

    case msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH:
      handleReqPageBufferWriteToFlash(msg);
      break;

    case msg::REQ_FLASH_WRITE_ERASE_PAGE:
      handleReqFlashWriteErasePage(msg);
      break;

    case msg::REQ_FLASH_WRITE_APP_CRC:
      handleReqFlashWriteAppCrc(msg);
      break;

    default:
      this->_response.result = msg::RES_ERR_UNKNOWN_REQ;
      break;
  };
}

FRANKLYBOOT_HANDLER_TEMPL
auto FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getResponse() const { return this->_response; }

FRANKLYBOOT_HANDLER_TEMPL

[[nodiscard]] auto FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getByteFromPageBuffer(uint32_t byte_idx) const {
  return this->_page_buffer.at(byte_idx);
}

FRANKLYBOOT_HANDLER_TEMPL
[[nodiscard]] bool FRANKLYBOOT_HANDLER_TEMPL_PREFIX::isAppValid() const {
  /* Read CRC value from flash */
  const uint32_t crc_value_stored = this->readAppCRCFromFlash();

  /* Calculate CRC value */
  const uint32_t crc_value_calc = this->calcAppCRC();

  return (crc_value_stored == crc_value_calc);
}

// Basic Info Requests ------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPing() {
  /* Transmit bootloader version as ping response */
  this->_response = msg::Msg(msg::REQ_PING, msg::RES_OK, 0);

  for (auto idx = 0U; idx < this->_response.data.size(); idx++) {
    if (idx < version::VERSION.size()) {
      this->_response.data[idx] = version::VERSION.at(idx);
    } else {
      this->_response.data[idx] = 0;
    }
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqResetDevice() {
  /* Queue reset device command */
  this->_response = msg::Msg(msg::REQ_RESET_DEVICE, msg::RES_OK, 0);
  this->_cmd_buffer = CommandBuffer::RESET_DEVICE;
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqStartApp(const msg::Msg& request) {
  constexpr uint32_t START_APP_UNSAFE_WORD = 0xFFFFFFFFU;

  this->_response = msg::Msg(msg::REQ_START_APP, msg::RES_ERR, 0);

  const bool start_app_safe = (msg::convertMsgDataToU32(request.data) != START_APP_UNSAFE_WORD);
  if (start_app_safe) {
    const bool is_crc_valid = isAppValid();
    if (is_crc_valid) {
      this->_cmd_buffer = CommandBuffer::START_APP;
      this->_response.result = msg::RES_OK;
    } else {
      this->_response.result = msg::RES_ERR_CRC_INVLD;
    }
  } else {
    this->_cmd_buffer = CommandBuffer::START_APP;
    this->_response.result = msg::RES_OK;
    this->_response.data = request.data;
  }
}

// General Info Requests ----------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoBootloaderVer() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_BOOTLOADER_VERSION, msg::RES_OK, 0);
  this->_response.data[0] = version::VERSION[0];
  this->_response.data[1] = version::VERSION[1];
  this->_response.data[2] = version::VERSION[2];
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoBootloaderCRC() {
  const uint32_t bootl_start_addr = FLASH_START;
  const uint32_t bootl_size = FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;
  const uint32_t crc_value = hwi::calculateCRC(bootl_start_addr, bootl_size);

  this->_response = msg::Msg(msg::REQ_DEV_INFO_BOOTLOADER_CRC, msg::RES_OK, 0);
  msg::convertU32ToMsgData(crc_value, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoVendorID() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_VID, msg::RES_OK, 0);
  msg::convertU32ToMsgData(hwi::getVendorID(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoProductID() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_PID, msg::RES_OK, 0);
  msg::convertU32ToMsgData(hwi::getProductID(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoProductionDate() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_PRD, msg::RES_OK, 0);
  msg::convertU32ToMsgData(hwi::getProductionDate(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoUniqueID(const msg::RequestType request) {
  this->_response = msg::Msg(request, msg::RES_OK, 0);
  uint32_t data;
  switch (request) {
    case msg::REQ_DEV_INFO_UID_1:
      data = hwi::getUniqueIDWord(0);
      break;
    case msg::REQ_DEV_INFO_UID_2:
      data = hwi::getUniqueIDWord(1);
      break;
    case msg::REQ_DEV_INFO_UID_3:
      data = hwi::getUniqueIDWord(2);
      break;
    case msg::REQ_DEV_INFO_UID_4:
      data = hwi::getUniqueIDWord(3);
      break;
    default:
      data = 0U;
      break;
  }

  msg::convertU32ToMsgData(data, this->_response.data);
}

// Flash Info Requests ------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashStartAddress() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_START_ADDR, msg::RES_OK, 0);
  msg::convertU32ToMsgData(FLASH_START, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashPageSize() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_PAGE_SIZE, msg::RES_OK, 0);
  msg::convertU32ToMsgData(FLASH_PAGE_SIZE, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashNumPages() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_NUM_PAGES, msg::RES_OK, 0);
  msg::convertU32ToMsgData(FLASH_NUM_PAGES, this->_response.data);
}

// App Info Requests --------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppPageIdx() {
  this->_response = msg::Msg(msg::REQ_APP_INFO_PAGE_IDX, msg::RES_OK, 0);
  msg::convertU32ToMsgData(FLASH_APP_FIRST_PAGE, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppCrcCalc() {
  uint32_t crc_value_calc = this->calcAppCRC();
  this->_response = msg::Msg(msg::REQ_APP_INFO_CRC_CALC, msg::RES_OK, 0);
  msg::convertU32ToMsgData(crc_value_calc, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppCrcStrd() {
  uint32_t crc_value_stored = this->readAppCRCFromFlash();
  this->_response = msg::Msg(msg::REQ_APP_INFO_CRC_STRD, msg::RES_OK, 0);
  msg::convertU32ToMsgData(crc_value_stored, this->_response.data);
}

// Flash Read Requests ------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashReadWord(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_FLASH_READ_WORD, msg::RES_ERR, request.packet_id);

  const uint32_t src_address = msg::convertMsgDataToU32(request.data);
  const bool address_inside_low_limit = (src_address >= FLASH_START);
  const bool address_inside_high_limit = ((src_address + this->_response.data.size()) <= (FLASH_START + FLASH_SIZE));
  const bool address_valid = address_inside_low_limit && address_inside_high_limit;

  if (address_valid) {
    for (auto idx = 0U; idx < this->_response.data.size(); idx++) {
      this->_response.data[idx] = hwi::readByteFromFlash(src_address + idx);
    }
    this->_response.result = msg::RES_OK;
  } else {
    this->_response.result = msg::RES_ERR_INVLD_ARG;
  }
}

// Page Buffer Requests -----------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferClear() {
  this->_page_buffer.fill({std::numeric_limits<uint8_t>::max()});
  this->_page_buffer_pos = 0U;
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_CLEAR, msg::RES_OK, 0);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferReadWord(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_READ_WORD, msg::RES_ERR, request.packet_id);

  const auto byte_idx = msg::convertMsgDataToU32(request.data);
  const auto byte_idx_valid = ((byte_idx + this->_response.data.size()) <= _page_buffer.size());

  if (byte_idx_valid) {
    for (auto idx = 0U; idx < this->_response.data.size(); idx++) {
      this->_response.data[idx] = _page_buffer.at(byte_idx + idx);
    }
    this->_response.result = msg::RES_OK;
  } else {
    this->_response.result = msg::RES_ERR_INVLD_ARG;
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferWriteWord(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_WORD, msg::RES_ERR, request.packet_id);
  this->_response.data = request.data;

  const uint32_t data_size = sizeof(uint32_t);

  const bool packet_id_valid =
      ((this->_page_buffer_pos >> 2U) & std::numeric_limits<uint8_t>::max()) == request.packet_id;

  const bool buffer_overflow = (this->_page_buffer_pos + data_size) > this->_page_buffer.size();

  if (packet_id_valid && !buffer_overflow) {
    for (auto idx = 0U; idx < data_size; idx++) {
      this->_page_buffer[this->_page_buffer_pos] = request.data[idx];
      this->_page_buffer_pos++;
    }

    this->_response.result = msg::RES_OK;
  } else if (buffer_overflow) {
    this->_response.result = msg::RES_ERR_PAGE_FULL;
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferCalcCrc() {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_CALC_CRC, msg::RES_OK, 0);

  uint32_t page_buffer_address = getPageBufferAddress();
  const uint32_t crc_value = hwi::calculateCRC(page_buffer_address, _page_buffer.size());

  msg::convertU32ToMsgData(crc_value, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferWriteToFlash(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH, msg::RES_ERR, 0);
  this->_response.data = request.data;

  const uint32_t page_id = msg::convertMsgDataToU32(request.data);
  const uint32_t address = FLASH_START + FLASH_PAGE_SIZE * page_id;
  const bool address_valid = (address >= FLASH_START && address < (FLASH_START + FLASH_SIZE));

  if (address_valid) {
    const auto erase_result = hwi::eraseFlashPage(page_id);

    if (erase_result) {
      const bool flash_result = hwi::writeDataBufferToFlash(address, page_id, _page_buffer.data(), _page_buffer.size());

      if (flash_result) {
        this->_response.result = msg::RES_OK;
      }
    }
  } else {
    this->_response.result = msg::RES_ERR_INVLD_ARG;
  }
}

// Flash Write Commands -----------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashWriteErasePage(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_FLASH_WRITE_ERASE_PAGE, msg::RES_ERR, request.packet_id);
  this->_response.data = request.data;

  const uint32_t page_id = msg::convertMsgDataToU32(request.data);

  const bool page_id_valid = (page_id >= FLASH_APP_FIRST_PAGE) && (page_id < FLASH_NUM_PAGES);
  if (page_id_valid) {
    const auto erase_result = hwi::eraseFlashPage(page_id);
    this->_response.result = (erase_result) ? msg::RES_OK : msg::RES_ERR;
  } else {
    this->_response.result = msg::RES_ERR_INVLD_ARG;
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashWriteAppCrc(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_FLASH_WRITE_APP_CRC, msg::RES_ERR, request.packet_id);

  /* Read complete page to buffer */
  const uint32_t page_id = FLASH_NUM_PAGES - 1U;
  const uint32_t start_address = FLASH_START + page_id * FLASH_PAGE_SIZE;
  uint32_t byte_address = start_address;
  for (uint32_t byte_idx = 0U; byte_idx < FLASH_PAGE_SIZE; byte_idx++) {
    this->_page_buffer[byte_idx] = hwi::readByteFromFlash(byte_address);
    byte_address++;
  }

  /* Store CRC value to last word in page buffer */
  this->_page_buffer[FLASH_PAGE_SIZE - 4U] = request.data[0];
  this->_page_buffer[FLASH_PAGE_SIZE - 3U] = request.data[1];
  this->_page_buffer[FLASH_PAGE_SIZE - 2U] = request.data[2];
  this->_page_buffer[FLASH_PAGE_SIZE - 1U] = request.data[3];

  /* Erase page */
  const auto erase_result = hwi::eraseFlashPage(page_id);

  if (erase_result) {
    /* Write page to flash */
    const bool flash_result = hwi::writeDataBufferToFlash(start_address, page_id, _page_buffer.data(), FLASH_PAGE_SIZE);

    if (flash_result) {
      this->_response.result = msg::RES_OK;
    }
  }

  /* Read CRC value from flash */
  this->_response.data[0U] = hwi::readByteFromFlash(FLASH_APP_CRC_VALUE_ADDRESS);
  this->_response.data[1U] = hwi::readByteFromFlash(FLASH_APP_CRC_VALUE_ADDRESS + 1U);
  this->_response.data[2U] = hwi::readByteFromFlash(FLASH_APP_CRC_VALUE_ADDRESS + 2U);
  this->_response.data[3U] = hwi::readByteFromFlash(FLASH_APP_CRC_VALUE_ADDRESS + 3U);
}

// Private utils functions --------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
uint32_t FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getPageBufferAddress() const {
  uint32_t page_buffer_address = 0U;

  if constexpr (is64BitSystem()) {
    /* Only used for testing on normal host pcs with 64 - bit*/
    auto ptr_address = reinterpret_cast<uint64_t>(_page_buffer.data());
    page_buffer_address = static_cast<uint32_t>(ptr_address);

  } else {
    page_buffer_address = reinterpret_cast<uint32_t>(_page_buffer.data());
  }

  return page_buffer_address;
}

FRANKLYBOOT_HANDLER_TEMPL
[[nodiscard]] uint32_t FRANKLYBOOT_HANDLER_TEMPL_PREFIX::calcAppCRC() const {
  /* Calculate CRC value */
  const uint32_t app_flash_ptr = FLASH_APP_ADDRESS;
  const uint32_t app_flash_size = (FLASH_APP_NUM_PAGES * FLASH_PAGE_SIZE) - 4U;
  const uint32_t crc_value_calc = hwi::calculateCRC(app_flash_ptr, app_flash_size);
  return crc_value_calc;
}

FRANKLYBOOT_HANDLER_TEMPL
[[nodiscard]] uint32_t FRANKLYBOOT_HANDLER_TEMPL_PREFIX::readAppCRCFromFlash() const {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  /* Read CRC value from flash */
  uint32_t crc_value_stored = 0U;
  for (auto idx = 0U; idx < sizeof(uint32_t); idx++) {
    crc_value_stored |=
        static_cast<uint32_t>(hwi::readByteFromFlash(FLASH_APP_CRC_VALUE_ADDRESS + idx) << (idx * NUM_BITS_PER_BYTE));
  }

  return crc_value_stored;
}

}; /* namespace franklyboot */

#endif /* FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_ */