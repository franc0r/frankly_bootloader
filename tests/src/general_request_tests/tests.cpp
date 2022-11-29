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

#include <limits>

using namespace franklyboot;  // NOLINT

// Defines / Constexpr ------------------------------------------------------------------------------------------------
constexpr uint32_t FLASH_START = 0x08000000U;
constexpr uint32_t FLASH_APP_FIRST_PAGE = 2U;
constexpr uint32_t FLASH_PAGE_SIZE = 1024U;
constexpr uint32_t FLASH_NUM_PAGES = 16;
constexpr uint32_t FLASH_SIZE = FLASH_NUM_PAGES * FLASH_PAGE_SIZE;

// Test Fixture Class -------------------------------------------------------------------------------------------------

class GeneralRequestTests;
GeneralRequestTests* g_pTestInstance = nullptr;

/**
 * @brief Test class for simulation of device
 */
class GeneralRequestTests : public ::testing::Test {
 public:
  GeneralRequestTests() = default;

  void SetUp() final { g_pTestInstance = this; }

  void TearDown() final { g_pTestInstance = nullptr; }

  void addFlashByte(const uint32_t address, const uint8_t value) { _flash_simulation.insert({address, value}); }

  void setCRCResult(const uint32_t crc_result) { _crc_result = crc_result; }

  [[nodiscard]] auto resetDeviceCalled() const { return _resetDeviceCalled; }
  [[nodiscard]] auto startAppCalled() const { return _startAppCalled; }
  [[nodiscard]] auto getCRCResult() const { return _crc_result; }
  [[nodiscard]] auto readByteFromFlash(const uint32_t address) {
    uint8_t value = std::numeric_limits<uint8_t>::max();
    if (auto search = _flash_simulation.find(address); search != _flash_simulation.end()) {
      value = search->second;
    }
    return value;
  }

  [[nodiscard]] auto& getHandler() { return _handler; }

  /* Called by HWI functions */

  void resetDevice() { _resetDeviceCalled = true; }

  [[nodiscard]] uint32_t calculateCRC(const uint8_t* data_ptr, uint32_t byte_size) const {
    (void)data_ptr;
    (void)byte_size;
    return _crc_result;
  }

  void startApp(uint32_t app_flash_address) {
    (void)app_flash_address;
    _startAppCalled = true;
  }

 private:
  bool _resetDeviceCalled = {false};
  bool _startAppCalled = {false};

  uint32_t _crc_result = {0U};

  std::map<uint32_t, uint8_t> _flash_simulation;

  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> _handler;
};

// Hardware Interface -------------------------------------------------------------------------------------------------

void hwi::resetDevice() {
  if (g_pTestInstance != nullptr) {
    g_pTestInstance->resetDevice();
  }
}

[[nodiscard]] uint32_t hwi::calculateCRC(const uint8_t* data_ptr, uint32_t byte_size) {
  uint32_t result = 0;
  if (g_pTestInstance != nullptr) {
    result = g_pTestInstance->calculateCRC(data_ptr, byte_size);
  }

  return result;
}

void hwi::startApp(uint32_t app_flash_address) {
  if (g_pTestInstance != nullptr) {
    g_pTestInstance->startApp(app_flash_address);
  }
}

[[nodiscard]] uint8_t hwi::readByteFromFlash(uint32_t flash_src_address) {
  uint8_t value = std::numeric_limits<uint8_t>::max();
  if (g_pTestInstance != nullptr) {
    value = g_pTestInstance->readByteFromFlash(flash_src_address);
  }
  return value;
}

// Tests --------------------------------------------------------------------------------------------------------------

/**
 * @brief Check if the bootloader response is correct for unknown requests
 */
TEST_F(GeneralRequestTests, UnknownReq) {
  constexpr uint16_t INVALID_REQUEST_TYPE = 0xDEAD;

  msg::Msg request_msg;
  request_msg.request = static_cast<msg::RequestType>(INVALID_REQUEST_TYPE);

  getHandler().processRequest(request_msg);

  const auto response = getHandler().getResponse();

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
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

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
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(false, this->resetDeviceCalled());
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  getHandler().processBufferedCmds();
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
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandler().processBufferedCmds();
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
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandler().processBufferedCmds();
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
    g_pTestInstance->addFlashByte(crc_flash_address + idx, static_cast<uint8_t>(CRC_VALUE >> (idx * 8U)));
  }
  g_pTestInstance->setCRCResult(CRC_VALUE);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(this->startAppCalled(), false);
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  getHandler().processBufferedCmds();
  EXPECT_EQ(this->startAppCalled(), true);
}