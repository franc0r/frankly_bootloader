/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Device Info Requests
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
class DeviceInfoTests : public TestHelper {
 public:
  DeviceInfoTests() = default;
};
// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(DeviceInfoTests, BootloaderVersion) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_BOOTLOADER_VERSION;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {version::VERSION[0], version::VERSION[1], version::VERSION[2], 0};

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

TEST_F(DeviceInfoTests, BootloaderCRC) {
  constexpr uint32_t EXPECTED_CRC_SRC_ADDRESS = FLASH_START;
  constexpr uint32_t EXPECTED_CRC_NUM_BYTES = FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;
  constexpr uint32_t CRC_VALUE = 0x1AC0BAAF;
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_BOOTLOADER_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(CRC_VALUE),
      static_cast<uint8_t>(CRC_VALUE >> 8U),
      static_cast<uint8_t>(CRC_VALUE >> 16U),
      static_cast<uint8_t>(CRC_VALUE >> 24U),
  };

  this->setCRCResult(CRC_VALUE);

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

TEST_F(DeviceInfoTests, VendorID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_VID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getVendorID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, ProductID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_PID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getProductID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, ProductionDate) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_PRD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getProductionDate() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, UniqueID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_UID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResultType EXPECTED_RESPONSE = msg::RES_OK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RES_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.result, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getUniqueID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}