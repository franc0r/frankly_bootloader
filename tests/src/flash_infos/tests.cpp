/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Flash Info Requests
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
class FlashInfoTests : public TestHelper {
 public:
  FlashInfoTests() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(FlashInfoTests, StartAddress) {
  constexpr uint32_t EXPECTED_VALUE = 0x08000000U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_START_ADDR;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(FlashInfoTests, PageSize) {
  constexpr uint32_t EXPECTED_VALUE = 0x400U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_PAGE_SIZE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(FlashInfoTests, NumPages) {
  constexpr uint32_t EXPECTED_VALUE = 16U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_NUM_PAGES;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}