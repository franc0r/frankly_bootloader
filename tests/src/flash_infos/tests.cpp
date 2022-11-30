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

#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/handler.h>
#include <gtest/gtest.h>

#include <limits>

using namespace franklyboot;  // NOLINT

// Defines / Constexpr ------------------------------------------------------------------------------------------------
constexpr uint32_t FLASH_START = 0x08000000U;
constexpr uint32_t FLASH_APP_FIRST_PAGE = 2U;
constexpr uint32_t FLASH_PAGE_SIZE = 1024U;
constexpr uint32_t FLASH_NUM_PAGES = 16;
constexpr uint32_t FLASH_SIZE = FLASH_NUM_PAGES * FLASH_PAGE_SIZE;

// Test Fixture Class -------------------------------------------------------------------------------------------------

class FlashInfoTests;
FlashInfoTests* g_pTestInstance = nullptr;

/**
 * @brief Test class for simulation of device
 */
class FlashInfoTests : public ::testing::Test {
 public:
  FlashInfoTests() = default;

  void SetUp() final { g_pTestInstance = this; }

  void TearDown() final { g_pTestInstance = nullptr; }

  [[nodiscard]] auto& getHandler() { return _handler; }

 private:
  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> _handler;
};

// Hardware Interface -------------------------------------------------------------------------------------------------

void hwi::resetDevice() {}

[[nodiscard]] uint32_t hwi::getVendorID() { return 0U; }

[[nodiscard]] uint32_t hwi::getProductID() { return 0U; }

[[nodiscard]] uint32_t hwi::getProductionDate() { return 0U; }

[[nodiscard]] uint32_t hwi::getUniqueID() { return 0U; }

[[nodiscard]] uint32_t hwi::calculateCRC(const uint8_t* data_ptr, uint32_t byte_size) {
  (void)data_ptr;
  (void)byte_size;
  return 0U;
}

void hwi::startApp(uint32_t app_flash_address) { (void)app_flash_address; }

[[nodiscard]] uint8_t hwi::readByteFromFlash(uint32_t flash_src_address) {
  (void)flash_src_address;
  return 0U;
}

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(FlashInfoTests, StartAddress) {
  constexpr uint32_t EXPECTED_VALUE = 0x08000000U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_START_ADDR;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(FlashInfoTests, PageSize) {
  constexpr uint32_t EXPECTED_VALUE = 0x400U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_PAGE_SIZE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(FlashInfoTests, NumPages) {
  constexpr uint32_t EXPECTED_VALUE = 16U;
  constexpr msg::RequestType REQUEST = msg::REQ_FLASH_INFO_NUM_PAGES;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(EXPECTED_VALUE),
      static_cast<uint8_t>(EXPECTED_VALUE >> 8U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 16U),
      static_cast<uint8_t>(EXPECTED_VALUE >> 24U),
  };

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}