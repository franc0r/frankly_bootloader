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

void franklyboot::msg::convertU32ToMsgData(const uint32_t data, franklyboot::msg::MsgData& msg_data) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  for (uint32_t idx = 0U; idx < msg_data.size(); idx++) {
    msg_data[idx] = static_cast<uint8_t>(data >> (idx * NUM_BITS_PER_BYTE));
  }
}

uint32_t franklyboot::msg::convertMsgDataToU32(const franklyboot::msg::MsgData& msg_data) {
  constexpr uint32_t NUM_BITS_PER_BYTE = 8U;

  uint32_t value = 0;
  for (auto idx = 0U; idx < sizeof(uint32_t); idx++) {
    value |= (static_cast<uint32_t>(msg_data.at(idx)) << (idx * NUM_BITS_PER_BYTE));
  }

  return value;
}
