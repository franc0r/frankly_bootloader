/**
 * @file msg.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Header defining the message protocol of the bootloader
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#ifndef FRANCOR_FRANKLYBOOT_MSG_H_
#define FRANCOR_FRANKLYBOOT_MSG_H_

#ifdef __cplusplus

#include <array>
#include <cstddef>
#include <cstdint>

/**
 * @brief Groups all definitions of the frankly boot bootloader
 */
namespace franklyboot {

/**
 * @brief Groups all message definitions into one namespace
 *
 */
namespace msg {

/**
 * @brief Response send from device to host
 */
enum ResponseType : uint8_t {
  RESP_NONE = 0x00U,           //!< Unused / ignored
  RESP_ACK = 0x01U,            //!< Acknowledge
  RESP_ERR = 0x02U,            //!< General error
  RESP_UNKNOWN_REQ = 0x03U,    //!< Unknow request type
  RESP_ACK_PAGE_FULL = 0x04U,  //!< Acknowledge and info that page buffer is full
  RESP_ERR_PAGE_FULL = 0x05U,  //!< Error, word not writable page buffer is full
};

/**
 * @brief Requests send from host to device
 */
enum RequestType : uint16_t {
  /* General requests */
  REQ_PING = 0x0000U,  //!< Ping device | Response is bootloader version
};

/** \brief Definition of the MsgData representation */
using MsgData = std::array<uint8_t, 4U>;

/**
 * @brief Representation of a bootloader message
 */
struct Msg {
  Msg() = default;
  Msg(RequestType req, ResponseType resp, uint8_t packet_id) : request(req), response(resp), packet_id(packet_id) {}

  RequestType request;
  ResponseType response;
  uint8_t packet_id;
  MsgData data;  // Maybe union and use std::array
};

/**
 * @brief Converts a unsigned int 32-bit value to the message buffer
 *
 * @param data Data to serialzize as word
 * @param msg_data Reference to data container (bytes)
 */
void convertU32ToMsgData(uint32_t data, msg::MsgData& msg_data);

/**
 * @brief Converts the message buffer to a unsigned int 32-bit value
 *
 * @param msg_data msg_data Reference to data container (bytes)
 * @return uint32_t Deserialized word
 */
uint32_t convertMsgDataToU32(const msg::MsgData& msg_data);

}; /* namespace msg */

}; /* namespace franklyboot */

#endif /* __cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_MSG_H_ */