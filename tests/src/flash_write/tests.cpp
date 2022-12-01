/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Flash Write Tests
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
class FlashWrite : public TestHelper {
 public:
  FlashWrite() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(FlashWrite, ErasePage) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_ERASE_PAGE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr uint32_t PAGE_ID = 3U;

  setErasePageResult(true);

  /* Init flash with some values */
  for (uint32_t byte_idx = 0U; byte_idx < FLASH_SIZE; byte_idx++) {
    const auto flash_address = FLASH_START + byte_idx;
    const auto value = static_cast<uint8_t>(byte_idx);
    setByteInFlash(flash_address, value);
  }

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(PAGE_ID, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  EXPECT_EQ(true, erasePageCalled());

  /* Check flash */
  constexpr uint32_t FLASH_ADDRESS = FLASH_START + FLASH_PAGE_SIZE * PAGE_ID;
  for (auto idx = 0U; idx < FLASH_PAGE_SIZE; idx++) {
    EXPECT_EQ(readByteFromFlash(FLASH_ADDRESS + idx), 0xFF);
  }
}

TEST_F(FlashWrite, ErasePageHWError) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_ERASE_PAGE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr uint32_t PAGE_ID = 3U;

  setErasePageResult(false);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(PAGE_ID, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  EXPECT_EQ(true, erasePageCalled());
}

TEST_F(FlashWrite, ErasePageBootlArea) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_ERASE_PAGE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr uint32_t PAGE_ID = 1U;

  setErasePageResult(false);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(PAGE_ID, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  EXPECT_EQ(false, erasePageCalled());
}

TEST_F(FlashWrite, ErasePageInvldPageID) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_ERASE_PAGE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr uint32_t PAGE_ID = FLASH_NUM_PAGES;

  setErasePageResult(false);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(PAGE_ID, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  EXPECT_EQ(false, erasePageCalled());
}

TEST_F(FlashWrite, WriteCRC) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_APP_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr uint32_t CRC_PAGE_ADDRESS = FLASH_START + (FLASH_NUM_PAGES - 1U) * FLASH_PAGE_SIZE;
  constexpr uint32_t CRC_VALUE = 0xDEADBEEF;

  setErasePageResult(true);
  setWriteToFlashResult(true);

  /* Init flash with some values */
  for (uint32_t byte_idx = 0U; byte_idx < FLASH_PAGE_SIZE; byte_idx++) {
    const auto flash_address = CRC_PAGE_ADDRESS + byte_idx;
    const auto value = static_cast<uint8_t>(byte_idx);
    setByteInFlash(flash_address, value);
  }

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(CRC_VALUE, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  /* Check data */
  for (uint32_t byte_idx = 0U; byte_idx < (FLASH_PAGE_SIZE - 4U); byte_idx++) {
    const auto flash_address = CRC_PAGE_ADDRESS + byte_idx;
    const auto value = static_cast<uint8_t>(byte_idx);
    EXPECT_EQ(value, readByteFromFlash(flash_address));
  }

  for (auto idx = 0U; idx < sizeof(uint32_t); idx++) {
    const auto expected_value = static_cast<uint8_t>(CRC_VALUE >> (idx * 8U));
    const auto address = getHandle().getFlashAppCRCValueAddress() + idx;

    EXPECT_EQ(expected_value, readByteFromFlash(address));
  }
}

TEST_F(FlashWrite, WriteCRCEraseError) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_APP_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr uint32_t CRC_VALUE = 0xDEADBEEF;

  setErasePageResult(false);
  setWriteToFlashResult(true);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(CRC_VALUE, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
}

TEST_F(FlashWrite, WriteCRCFlashError) {
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_WRITE_APP_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr uint32_t CRC_VALUE = 0xDEADBEEF;

  setErasePageResult(true);
  setWriteToFlashResult(false);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(CRC_VALUE, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
}