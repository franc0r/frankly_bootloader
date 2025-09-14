/**
 * @file msg.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Source file containing utilities for message handling
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include "francor/franklyboot/msg.h"

void franklyboot::msg::convertU32ToMsgData(const uint32_t data, franklyboot::msg::MsgData &msg_data) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  for (uint32_t idx = 0U; idx < msg_data.size(); idx++) {
    msg_data[idx] = static_cast<uint8_t>(data >> (idx * NUM_BITS_PER_BYTE));
  }
}

uint32_t franklyboot::msg::convertMsgDataToU32(const franklyboot::msg::MsgData &msg_data) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  uint32_t value = 0;
  for (auto idx = 0U; idx < sizeof(uint32_t); idx++) {
    value |= (static_cast<uint32_t>(msg_data.at(idx)) << (idx * NUM_BITS_PER_BYTE));
  }

  return value;
}

franklyboot::msg::Msg franklyboot::msg::convertBytesToMsg(const msg::MsgRaw &msg_raw) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  const uint16_t request_raw =
      static_cast<uint16_t>(msg_raw.at(0)) | (static_cast<uint16_t>(msg_raw.at(1)) << NUM_BITS_PER_BYTE);

  msg::Msg msg;
  msg.request = static_cast<msg::RequestType>(request_raw);
  msg.result = static_cast<msg::ResultType>(msg_raw.at(2));
  msg.packet_id = static_cast<uint8_t>(msg_raw.at(3));

  for (auto idx = 0U; idx < msg.data.size(); idx++) {
    msg.data.at(idx) = msg_raw.at(4 + idx);
  }

  return msg;
}

franklyboot::msg::MsgRaw franklyboot::msg::convertMsgToBytes(const msg::Msg &msg) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  msg::MsgRaw msg_raw;

  msg_raw.at(0) = static_cast<uint8_t>(msg.request);
  msg_raw.at(1) = static_cast<uint8_t>(msg.request >> NUM_BITS_PER_BYTE);
  msg_raw.at(2) = static_cast<uint8_t>(msg.result);
  msg_raw.at(3) = static_cast<uint8_t>(msg.packet_id);

  for (auto idx = 0U; idx < msg.data.size(); idx++) {
    msg_raw.at(4 + idx) = msg.data.at(idx);
  }

  return msg_raw;
}