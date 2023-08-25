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
 * @brief Result of request send as response
 */
enum ResultType : uint8_t {
  RES_NONE = 0x00U,  //!< No result / not specified
  RES_OK = 0x01U,    //!< Message was processed successfully / result ok

  RES_ERR = 0xFEU,                //!< General error
  RES_ERR_UNKNOWN_REQ = 0xFDU,    //!< Unknow request type
  RES_ERR_NOT_SUPPORTED = 0xFCU,  //!< Error, command known but not supported
  RES_ERR_CRC_INVLD = 0xFBU,      //!< Error, CRC check failed
  RES_ERR_PAGE_FULL = 0xFAU,      //!< Error, word not writable page buffer is full
  RES_ERR_INVLD_ARG = 0xF9U,      //!< Error, invalid argument (out of range, ...)
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

  /* 128-Bit unique ID */
  REQ_DEV_INFO_UID_1 = 0x0106U,  //!< Reads the device unique ID bit [0:31]
  REQ_DEV_INFO_UID_2 = 0x0107U,  //!< Reads the device unique ID bit [32:63]
  REQ_DEV_INFO_UID_3 = 0x0108U,  //!< Reads the device unique ID bit [64:95]
  REQ_DEV_INFO_UID_4 = 0x0109U,  //!< Reads the device unique ID bit [96:127]

  /* Flash information */
  REQ_FLASH_INFO_START_ADDR = 0x0201U,  //!< Get the start address of the flash area
  REQ_FLASH_INFO_PAGE_SIZE = 0x0202U,   //!< Get the size in bytes of a page
  REQ_FLASH_INFO_NUM_PAGES = 0x0203U,   //!< Get the number of pages (including bootloader area)

  /* App Information */
  REQ_APP_INFO_PAGE_IDX = 0x0301U,  //!< Get the page idx of app area in flash
  REQ_APP_INFO_CRC_CALC = 0x0302U,  //!< Get the calculate CRC over app flash area
  REQ_APP_INFO_CRC_STRD = 0x0303U,  //!< Get the stored CRC value used for safe startup

  /* Flash Read commands */
  REQ_FLASH_READ_WORD = 0x0401U,  //!< Reads a word from the flash

  /* Page Buffer Commands */
  REQ_PAGE_BUFFER_CLEAR = 0x1001U,           //!< Clears the page buffer (RAM)
  REQ_PAGE_BUFFER_READ_WORD = 0x1002U,       //!< Reads a word to the page buffer (RAM)
  REQ_PAGE_BUFFER_WRITE_WORD = 0x1003U,      //!< Writes a word to the page buffer (RAM)
  REQ_PAGE_BUFFER_CALC_CRC = 0x1004U,        //!< Calculates the CRC over the page buffer
  REQ_PAGE_BUFFER_WRITE_TO_FLASH = 0x1005U,  //!< Write the page buffer to the desired flash page

  /* Flash Write Commands*/
  REQ_FLASH_WRITE_ERASE_PAGE = 0x1101U,  //!< Erases an flash page
  REQ_FLASH_WRITE_APP_CRC = 0x1102U,     //!< Writes the CRC of the app to the flash

};

/** \brief Definition of the MsgData representation */
using MsgRaw = std::array<uint8_t, 8U>;
using MsgData = std::array<uint8_t, 4U>;

/**
 * @brief Representation of a bootloader message
 */
struct Msg {
  Msg() = default;
  Msg(RequestType req, ResultType res, uint8_t packet_id)
      : request(req), result(res), packet_id(packet_id), data({0}) {}

  RequestType request;
  ResultType result;
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

/**
 * @brief Converts a byte array to a message struct
 */
msg::Msg convertBytesToMsg(const msg::MsgRaw& msg_raw);

/**
 * @brief Converts a msg to a byte array
 */
msg::MsgRaw convertMsgToBytes(const msg::Msg& msg);

}; /* namespace msg */

}; /* namespace franklyboot */

#endif /* __cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_MSG_H_ */