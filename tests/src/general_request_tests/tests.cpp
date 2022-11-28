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

#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/handler.h>
#include <gtest/gtest.h>

constexpr uint32_t FLASH_START = 0x08000000U;
constexpr uint32_t FLASH_APP_FIRST_PAGE = 2U;
constexpr uint32_t FLASH_PAGE_SIZE = 1024U;
constexpr uint32_t FLASH_NUM_PAGES = 16;
constexpr uint32_t FLASH_SIZE = FLASH_NUM_PAGES * FLASH_PAGE_SIZE;

using namespace franklyboot;  // NOLINT

bool g_resetDeviceCalled = {false};

void hwi::resetDevice() { g_resetDeviceCalled = true; }
void hwi::startApp(uint32_t app_flash_address) { (void)app_flash_address; }

/**
 * @brief Check if the bootloader response is correct for unknown requests
 */
TEST(GeneralRequests, UnknownReq) {
  constexpr uint16_t INVALID_REQUEST_TYPE = 0xDEAD;
  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> handler;

  msg::Msg request_msg;
  request_msg.request = static_cast<msg::RequestType>(INVALID_REQUEST_TYPE);

  handler.processRequest(request_msg);

  const auto response = handler.getResponse();

  EXPECT_EQ(response.request, INVALID_REQUEST_TYPE);
  EXPECT_EQ(response.response, msg::RESP_UNKNOWN_REQ);
}

/**
 * @brief Check if bootloader response for ping is correct
 *
 */
TEST(GeneralRequests, ReqPing) {
  constexpr msg::RequestType REQUEST = msg::REQ_PING;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {version::VERSION[0], version::VERSION[1], version::VERSION[2], 0};
  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> handler;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  handler.processRequest(request_msg);
  const auto response = handler.getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

/**
 * @brief Check if reset request is executed
 *
 */
TEST(GeneralRequests, ReqResetDevice) {
  g_resetDeviceCalled = false;
  constexpr msg::RequestType REQUEST = msg::REQ_RESET_DEVICE;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> handler;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  handler.processRequest(request_msg);
  const auto response = handler.getResponse();

  /* Check response */
  EXPECT_EQ(false, g_resetDeviceCalled);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  handler.processBufferedCmds();
  EXPECT_EQ(true, g_resetDeviceCalled);
}