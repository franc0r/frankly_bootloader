/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - App Info Requests
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
class AppInfoTests : public TestHelper {
 public:
  AppInfoTests() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(AppInfoTests, PageIdx) {
  constexpr uint32_t EXPECTED_VALUE = 2U;
  constexpr msg::RequestType REQUEST = msg::REQ_APP_INFO_PAGE_IDX;
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

TEST_F(AppInfoTests, ReadCRCCalc) {
  constexpr uint32_t EXPECTED_CRC_SRC_ADDRESS = FLASH_START + FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;
  constexpr uint32_t EXPECTED_CRC_NUM_BYTES = FLASH_SIZE - (FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE);
  constexpr uint32_t EXPECTED_VALUE = 0xBEEFDEAD;
  constexpr msg::RequestType REQUEST = msg::REQ_APP_INFO_CRC_CALC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Set CRC result*/
  setCRCResult(EXPECTED_VALUE);

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
  EXPECT_EQ(this->getCalcCRCSrcAddress(), EXPECTED_CRC_SRC_ADDRESS);
  EXPECT_EQ(this->getCalcCRCNumBytes(), EXPECTED_CRC_NUM_BYTES);
}

TEST_F(AppInfoTests, ReadCRCStored) {
  constexpr uint32_t EXPECTED_VALUE = 0xBEEFDEAD;
  constexpr uint32_t CRC_FLASH_ADDR = FLASH_START + FLASH_SIZE - 4U;
  constexpr msg::RequestType REQUEST = msg::REQ_APP_INFO_CRC_STRD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Store CRC in flash */
  for (uint32_t idx = 0U; idx < sizeof(uint32_t); idx++) {
    this->setByteInFlash(CRC_FLASH_ADDR + idx, EXPECTED_DATA[idx]);
  }

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