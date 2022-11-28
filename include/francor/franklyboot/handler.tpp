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

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::processBufferedCmds() {
  switch (this->_cmd_buffer) {
    case CommandBuffer::NONE:
      break;
    case CommandBuffer::RESET_DEVICE:
      franklyboot::hwi::resetDevice();
      break;
    case CommandBuffer::START_APP:
      const uint32_t app_flash_address = FLASH_START + FLASH_PAGE_SIZE * FLASH_APP_FIRST_PAGE;
      franklyboot::hwi::startApp(app_flash_address);
      break;
  }

  _cmd_buffer = CommandBuffer::NONE;
}

FRANKLYBOOT_HANDLER_TEMPL
void FRANKLYBOOT_HANDLER_TEMPL_PREFIX::processRequest(const msg::Msg &msg) {
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

    default:
      this->_response.response = msg::RESP_UNKNOWN_REQ;
      break;
  };
}

FRANKLYBOOT_HANDLER_TEMPL
auto FRANKLYBOOT_HANDLER_TEMPL_PREFIX::getResponse() const { return this->_response; }

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
  /* Transmit bootloader version as ping response */
  this->_response = msg::Msg(msg::REQ_RESET_DEVICE, msg::RESP_ACK, 0);
  this->_cmd_buffer = CommandBuffer::RESET_DEVICE;
}

}; /* namespace franklyboot */

#endif /* FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_ */