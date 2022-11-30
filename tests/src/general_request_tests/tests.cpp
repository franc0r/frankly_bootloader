/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - General Requests
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
class GeneralRequestTests : public TestHelper {
 public:
  GeneralRequestTests() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

/**
 * @brief Check if the bootloader response is correct for unknown requests
 */
TEST_F(GeneralRequestTests, UnknownReq) {
  constexpr uint16_t INVALID_REQUEST_TYPE = 0xDEAD;

  msg::Msg request_msg;
  request_msg.request = static_cast<msg::RequestType>(INVALID_REQUEST_TYPE);

  getHandle().processRequest(request_msg);

  const auto response = getHandle().getResponse();

  EXPECT_EQ(response.request, INVALID_REQUEST_TYPE);
  EXPECT_EQ(response.response, msg::RESP_UNKNOWN_REQ);
}

/**
 * @brief Check if bootloader response for ping is correct
 *
 */
TEST_F(GeneralRequestTests, ReqPing) {
  constexpr msg::RequestType REQUEST = msg::REQ_PING;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {version::VERSION[0], version::VERSION[1], version::VERSION[2], 0};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

/**
 * @brief Check if reset request is executed
 */
TEST_F(GeneralRequestTests, ReqResetDevice) {
  constexpr msg::RequestType REQUEST = msg::REQ_RESET_DEVICE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(false, this->resetDeviceCalled());
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  getHandle().processBufferedCmds();
  EXPECT_EQ(true, this->resetDeviceCalled());
}

TEST_F(GeneralRequestTests, ReqStartAppUnsafe) {
  constexpr msg::RequestType REQUEST = msg::REQ_START_APP;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {0xFF, 0xFF, 0xFF, 0xFF};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandle().processBufferedCmds();
  EXPECT_EQ(this->startAppCalled(), true);
}

TEST_F(GeneralRequestTests, ReqStartAppCRCInvalid) {
  constexpr msg::RequestType REQUEST = msg::REQ_START_APP;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_CRC_INVLD;
  constexpr msg::MsgData EXPECTED_DATA = {0, 0, 0, 0};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandle().processBufferedCmds();
  EXPECT_EQ(this->startAppCalled(), false);
}

TEST_F(GeneralRequestTests, ReqStartAppCRCValid) {
  constexpr uint32_t CRC_VALUE = 0xDEADBEEF;
  constexpr msg::RequestType REQUEST = msg::REQ_START_APP;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {0, 0, 0, 0};

  /* Add CRC value to flash */
  const uint32_t crc_flash_address = FLASH_START + FLASH_SIZE - 4U;
  for (auto idx = 0U; idx < sizeof(uint32_t); idx++) {
    this->setByteInFlash(crc_flash_address + idx, static_cast<uint8_t>(CRC_VALUE >> (idx * 8U)));
  }
  this->setCRCResult(CRC_VALUE);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandle().processBufferedCmds();
  EXPECT_EQ(this->startAppCalled(), true);
}