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
  this->_response.response = msg::RESP_ERR;

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

    case msg::REQ_DEV_INFO_UID:
      handleReqInfoUniqueID();
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

    default:
      this->_response.response = msg::RESP_UNKNOWN_REQ;
      break;
  };
}

FRANKLYBOOT_HANDLER_TEMPL
auto FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getResponse() const { return this->_response; }

FRANKLYBOOT_HANDLER_TEMPL

[[nodiscard]] auto FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getByteFromPageBuffer(uint32_t byte_idx) const {
  return this->_page_buffer.at(byte_idx);
}

// Basic Info Requests ------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPing() {
  /* Transmit bootloader version as ping response */
  this->_response = msg::Msg(msg::REQ_PING, msg::RESP_ACK, 0);

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
  this->_response = msg::Msg(msg::REQ_RESET_DEVICE, msg::RESP_ACK, 0);
  this->_cmd_buffer = CommandBuffer::RESET_DEVICE;
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqStartApp(const msg::Msg& request) {
  constexpr uint32_t START_APP_UNSAFE_WORD = 0xFFFFFFFFU;

  this->_response = msg::Msg(msg::REQ_START_APP, msg::RESP_ERR, 0);

  const bool start_app_safe = (msg::convertMsgDataToU32(request.data) != START_APP_UNSAFE_WORD);
  if (start_app_safe) {
    const bool is_crc_valid = isAppCRCValid();
    if (is_crc_valid) {
      this->_cmd_buffer = CommandBuffer::START_APP;
      this->_response.response = msg::RESP_ACK;
    } else {
      this->_response.response = msg::RESP_ERR_CRC_INVLD;
    }
  } else {
    this->_cmd_buffer = CommandBuffer::START_APP;
    this->_response.response = msg::RESP_ACK;
    this->_response.data = request.data;
  }
}

// General Info Requests ----------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoBootloaderVer() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_BOOTLOADER_VERSION, msg::RESP_ACK, 0);
  this->_response.data[0] = version::VERSION[0];
  this->_response.data[1] = version::VERSION[1];
  this->_response.data[2] = version::VERSION[2];
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoBootloaderCRC() {
  const uint32_t bootl_start_addr = FLASH_START;
  const uint32_t bootl_size = FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;
  const uint32_t crc_value = hwi::calculateCRC(bootl_start_addr, bootl_size);

  this->_response = msg::Msg(msg::REQ_DEV_INFO_BOOTLOADER_CRC, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(crc_value, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoVendorID() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_VID, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(hwi::getVendorID(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoProductID() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_PID, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(hwi::getProductID(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoProductionDate() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_PRD, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(hwi::getProductionDate(), this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqInfoUniqueID() {
  this->_response = msg::Msg(msg::REQ_DEV_INFO_UID, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(hwi::getUniqueID(), this->_response.data);
}

// Flash Info Requests ------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashStartAddress() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_START_ADDR, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(FLASH_START, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashPageSize() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_PAGE_SIZE, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(FLASH_PAGE_SIZE, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqFlashNumPages() {
  this->_response = msg::Msg(msg::REQ_FLASH_INFO_NUM_PAGES, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(FLASH_NUM_PAGES, this->_response.data);
}

// App Info Requests --------------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppPageIdx() {
  this->_response = msg::Msg(msg::REQ_APP_INFO_PAGE_IDX, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(FLASH_APP_FIRST_PAGE, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppCrcCalc() {
  uint32_t crc_value_calc = this->calcAppCRC();
  this->_response = msg::Msg(msg::REQ_APP_INFO_CRC_CALC, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(crc_value_calc, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqAppCrcStrd() {
  uint32_t crc_value_stored = this->readAppCRCFromFlash();
  this->_response = msg::Msg(msg::REQ_APP_INFO_CRC_STRD, msg::RESP_ACK, 0);
  msg::convertU32ToMsgData(crc_value_stored, this->_response.data);
}

// Page Buffer Requests -----------------------------------------------------------------------------------------------

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferClear() {
  this->_page_buffer.fill({std::numeric_limits<uint8_t>::max()});
  this->_page_buffer_pos = 0U;
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_CLEAR, msg::RESP_ACK, 0);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferReadWord(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_READ_WORD, msg::RESP_ERR, request.packet_id);

  const auto byte_idx = msg::convertMsgDataToU32(request.data);
  const auto byte_idx_valid = ((byte_idx + sizeof(uint32_t)) <= _page_buffer.size());

  if (byte_idx_valid) {
    this->_response.data[0U] = _page_buffer.at(byte_idx);
    this->_response.data[1U] = _page_buffer.at(byte_idx + 1U);
    this->_response.data[2U] = _page_buffer.at(byte_idx + 2U);
    this->_response.data[3U] = _page_buffer.at(byte_idx + 3U);
    this->_response.response = msg::RESP_ACK;
  } else {
    this->_response.response = msg::RESP_ERR_INVLD_ARG;
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferWriteWord(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_WORD, msg::RESP_ERR, request.packet_id);
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

    const bool page_buffer_full = (this->_page_buffer_pos >= this->_page_buffer.size());

    if (page_buffer_full) {
      this->_response.response = msg::RESP_ACK_PAGE_FULL;
    } else {
      this->_response.response = msg::RESP_ACK;
    }
  } else if (buffer_overflow) {
    this->_response.response = msg::RESP_ERR_PAGE_FULL;
  }
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferCalcCrc() {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_CALC_CRC, msg::RESP_ACK, 0);

  uint32_t page_buffer_address = getPageBufferAddress();
  const uint32_t crc_value = hwi::calculateCRC(page_buffer_address, _page_buffer.size());

  msg::convertU32ToMsgData(crc_value, this->_response.data);
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::handleReqPageBufferWriteToFlash(const msg::Msg& request) {
  this->_response = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH, msg::RESP_ERR, 0);
  this->_response.data = request.data;

  const uint32_t page_id = msg::convertMsgDataToU32(request.data);
  const uint32_t address = FLASH_START + FLASH_PAGE_SIZE * page_id;
  const bool address_valid = (address >= FLASH_START && address < (FLASH_START + FLASH_SIZE));

  if (address_valid) {
    uint32_t page_buffer_address = getPageBufferAddress();

    const bool flash_ok = hwi::writeDataBufferToFlash(address, page_id, page_buffer_address, FLASH_PAGE_SIZE);

    if (flash_ok) {
      this->_response.response = msg::RESP_ACK;
    }
  }
}

// Private utils functions
// --------------------------------------------------------------------------------------------

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
  const uint32_t crc_value_calc = hwi::calculateCRC(app_flash_ptr, (FLASH_APP_NUM_PAGES * FLASH_PAGE_SIZE));
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

FRANKLYBOOT_HANDLER_TEMPL
[[nodiscard]] bool FRANKLYBOOT_HANDLER_TEMPL_PREFIX::isAppCRCValid() const {
  /* Read CRC value from flash */
  const uint32_t crc_value_stored = this->readAppCRCFromFlash();

  /* Calculate CRC value */
  const uint32_t crc_value_calc = this->calcAppCRC();

  return (crc_value_stored == crc_value_calc);
}
}; /* namespace franklyboot */

#endif /* FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_ */