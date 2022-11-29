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
  RESP_NONE = 0x00U,  //!< Unused / ignored
  RESP_ACK = 0x01U,   //!< Acknowledge

  RESP_ERR = 0xFEU,                //!< General error
  RESP_UNKNOWN_REQ = 0xFDU,        //!< Unknow request type
  RESP_ERR_NOT_SUPPORTED = 0xFCU,  //!< Error, command known but not supported
  RESP_ERR_CRC_INVLD = 0xFBU,      //!< Error, CRC check failed
  RESP_ACK_PAGE_FULL = 0xFAU,      //!< Acknowledge and info that page buffer is full
  RESP_ERR_PAGE_FULL = 0xF9U,      //!< Error, word not writable page buffer is full

};

/**
 * @brief Requests send from host to device
 */
enum RequestType : uint16_t {
  /* General requests */
  REQ_PING = 0x0001U,          //!< Ping device | Response is bootloader version
  REQ_RESET_DEVICE = 0x0011U,  //!< Resets the device (hardware reset)
  REQ_START_APP = 0x0012U,     //!< Start application and exit bootloader

  /* Device information */
  REQ_DEV_INFO_BOOTLOADER_VERSION = 0x0101U,  //!< Reads the bootloader version
  REQ_DEV_INFO_BOOTLOADER_CRC = 0x0102U,      //!< Calculates the CRC of the bootloader flash area
  REQ_DEV_INFO_VID = 0x0103U,                 //!< Reads the vendor id
  REQ_DEV_INFO_PID = 0x0104U,                 //!< Reads the product id
  REQ_DEV_INFO_PRD = 0x0105U,                 //!< Reads the production date
  REQ_DEV_INFO_UID = 0x0106U,                 //!< Reads the device unique ID

};

/** \brief Definition of the MsgData representation */
using MsgData = std::array<uint8_t, 4U>;

/**
 * @brief Representation of a bootloader message
 */
struct Msg {
  Msg() = default;
  Msg(RequestType req, ResponseType resp, uint8_t packet_id)
      : request(req), response(resp), packet_id(packet_id), data({0}) {}

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