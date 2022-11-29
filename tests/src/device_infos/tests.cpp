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

class DeviceInfoTests;
DeviceInfoTests* g_pTestInstance = nullptr;

/**
 * @brief Test class for simulation of device
 */
class DeviceInfoTests : public ::testing::Test {
 public:
  DeviceInfoTests() = default;

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

  [[nodiscard]] auto getBootloaderCRC() const { return _bootloader_crc; }
  [[nodiscard]] auto getVendorID() const { return _vendor_id; }
  [[nodiscard]] auto getProductID() const { return _product_id; }
  [[nodiscard]] auto getProductionDate() const { return _production_date; }
  [[nodiscard]] auto getUniqueID() const { return _unique_id; }

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
  const uint32_t _bootloader_crc = {0xB00D1234};
  const uint32_t _vendor_id = {0xDEA0B00F};
  const uint32_t _product_id = {0xEFABCDEF};
  const uint32_t _production_date = {0x29019019};
  const uint32_t _unique_id = {0x12345678};

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

[[nodiscard]] uint32_t hwi::getVendorID() {
  uint32_t result = 0U;
  if (g_pTestInstance != nullptr) {
    result = g_pTestInstance->getVendorID();
  }

  return result;
}

[[nodiscard]] uint32_t hwi::getProductID() {
  uint32_t result = 0U;
  if (g_pTestInstance != nullptr) {
    result = g_pTestInstance->getProductID();
  }

  return result;
}

[[nodiscard]] uint32_t hwi::getProductionDate() {
  uint32_t result = 0U;
  if (g_pTestInstance != nullptr) {
    result = g_pTestInstance->getProductionDate();
  }

  return result;
}

[[nodiscard]] uint32_t hwi::getUniqueID() {
  uint32_t result = 0U;
  if (g_pTestInstance != nullptr) {
    result = g_pTestInstance->getUniqueID();
  }

  return result;
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

TEST_F(DeviceInfoTests, BootloaderVersion) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_BOOTLOADER_VERSION;
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

TEST_F(DeviceInfoTests, BootloaderCRC) {
  constexpr uint32_t CRC_VALUE = 0x1AC0BAAF;
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_BOOTLOADER_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {
      static_cast<uint8_t>(CRC_VALUE),
      static_cast<uint8_t>(CRC_VALUE >> 8U),
      static_cast<uint8_t>(CRC_VALUE >> 16U),
      static_cast<uint8_t>(CRC_VALUE >> 24U),
  };

  this->setCRCResult(CRC_VALUE);

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

TEST_F(DeviceInfoTests, VendorID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_VID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getVendorID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, ProductID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_PID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getProductID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, ProductionDate) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_PRD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getProductionDate() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}

TEST_F(DeviceInfoTests, UniqueID) {
  constexpr msg::RequestType REQUEST = msg::REQ_DEV_INFO_UID;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandler().processRequest(request_msg);
  const auto response = getHandler().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    const auto expected_val = static_cast<uint8_t>(this->getUniqueID() >> (8U * idx));
    EXPECT_EQ(response.data.at(idx), expected_val);
  }
}