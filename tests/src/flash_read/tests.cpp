/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Flash Read
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor/frankly_test_utils.h>

#include <limits>

using namespace franklyboot;              // NOLINT
using namespace franklyboot::test_utils;  // NOLINT

// Test Fixture Class -------------------------------------------------------------------------------------------------

/**
 * @brief Test class for simulation of device
 */
class FlashReadTests : public TestHelper {
 public:
  FlashReadTests() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(FlashReadTests, readByteFromFlash) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_READ_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr uint32_t READ_ADDRESS = 0x08000423U;

  /* Init flash with some values */
  for (uint32_t byte_idx = 0U; byte_idx < FLASH_SIZE; byte_idx++) {
    const auto flash_address = FLASH_START + byte_idx;
    const auto value = static_cast<uint8_t>(byte_idx);
    setByteInFlash(flash_address, value);
  }

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(READ_ADDRESS, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_value = static_cast<uint8_t>((READ_ADDRESS - FLASH_START) + idx);
    EXPECT_EQ(response.data.at(idx), expected_value);
  }
}

TEST_F(FlashReadTests, readByteFromFlashInvldAddress) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_READ_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr uint32_t READ_ADDRESS = 0x08000000U - 1U;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(READ_ADDRESS, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
}

TEST_F(FlashReadTests, readByteFromFlashInvldAddress2) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_READ_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr uint32_t READ_ADDRESS = FLASH_START + FLASH_SIZE - 3U;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(READ_ADDRESS, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
}