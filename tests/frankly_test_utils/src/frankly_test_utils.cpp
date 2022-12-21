/**
 * @file frankly_test_utils.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Test Utils
 * @version 0.1
 * @date 2022-11-30
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor/frankly_test_utils.h>

#include <limits>

namespace franklyboot::test_utils {

// Global Variable for HWI --------------------------------------------------------------------------------------------
static TestHelper* testInstance = {nullptr};  // NOLINT

// GTest SetUp / Teardown ---------------------------------------------------------------------------------------------

void TestHelper::SetUp() {
  testInstance = this;
  initFlashSim();
}

void TestHelper::TearDown() {
  _flash_simulation.clear();
  testInstance = nullptr;
}

// Set Functions ------------------------------------------------------------------------------------------------------

void TestHelper::setVendorID(uint32_t value) { _vendor_id = value; }

void TestHelper::setProductID(uint32_t value) { _product_id = value; }
void TestHelper::setProductionDate(uint32_t value) { _production_date = value; }
void TestHelper::setUniqueID(uint32_t value) { _unique_id = value; }
void TestHelper::setCRCResult(uint32_t value) { _crc_calc_result = value; }

void TestHelper::setByteInFlash(uint32_t address, uint8_t value) {
  if (auto search = _flash_simulation.find(address); search != _flash_simulation.end()) {
    search->second = value;
  }
}

[[nodiscard]] auto TestHelper::readByteFromFlash(uint32_t address) const {
  uint8_t value = 0U;
  if (auto search = _flash_simulation.find(address); search != _flash_simulation.end()) {
    value = search->second;
  }

  return value;
}

void TestHelper::setWriteToFlashResult(bool result) { _write_to_flash_result = result; }
void TestHelper::setErasePageResult(bool result) { _erase_page_result = result; }

// Help Functions -----------------------------------------------------------------------------------------------------

void TestHelper::clearPageBuffer() {
  const msg::Msg clear_request = msg::Msg(msg::REQ_PAGE_BUFFER_CLEAR, msg::RES_NONE, 0);
  this->_handle.processRequest(clear_request);
  this->_handle.processBufferedCmds();
}

// Check Functions ----------------------------------------------------------------------------------------------------

[[nodiscard]] bool TestHelper::resetDeviceCalled() const { return _resetDeviceCalled; }
[[nodiscard]] bool TestHelper::startAppCalled() const { return _startAppCalled; }
[[nodiscard]] uint32_t TestHelper::getCalcCRCSrcAddress() const { return _crc_calc_src_address; }
[[nodiscard]] uint32_t TestHelper::getCalcCRCNumBytes() const { return _crc_calc_num_bytes; }
[[nodiscard]] bool TestHelper::writeToFlashCalled() const { return _write_to_flash_called; }
[[nodiscard]] bool TestHelper::erasePageCalled() const { return _erase_page_called; }

// HWI abstraction ----------------------------------------------------------------------------------------------------

void TestHelper::resetDevice() { _resetDeviceCalled = true; }
[[nodiscard]] uint32_t TestHelper::getVendorID() const { return _vendor_id; }
[[nodiscard]] uint32_t TestHelper::getProductID() const { return _product_id; }
[[nodiscard]] uint32_t TestHelper::getProductionDate() const { return _production_date; }
[[nodiscard]] uint32_t TestHelper::getUniqueID() const { return _unique_id; }
[[nodiscard]] uint32_t TestHelper::calculateCRC(const uint32_t src_address, const uint32_t num_bytes) {
  _crc_calc_src_address = src_address;
  _crc_calc_num_bytes = num_bytes;
  return _crc_calc_result;
}

[[nodiscard]] bool TestHelper::eraseFlashPage(const uint32_t page_id) {
  const uint32_t page_address = FLASH_START + FLASH_PAGE_SIZE * page_id;

  for (auto idx = 0U; idx < FLASH_PAGE_SIZE; idx++) {
    setByteInFlash(page_address + idx, std::numeric_limits<uint8_t>::max());
  }

  _erase_page_called = true;
  return _erase_page_result;
}

bool TestHelper::writeDataBufferToFlash(const uint32_t dst_address, const uint32_t dst_page_id, uint8_t* src_data_ptr,
                                        const uint32_t num_bytes) {
  (void)dst_page_id;

  for (auto idx = 0U; idx < num_bytes; idx++) {
    setByteInFlash(dst_address + idx, src_data_ptr[idx]);
  }

  _write_to_flash_called = true;

  return _write_to_flash_result;
}

[[nodiscard]] uint8_t TestHelper::readByteFromFlash(uint32_t flash_src_address) {
  uint8_t value = std::numeric_limits<uint8_t>::max();
  if (auto search = _flash_simulation.find(flash_src_address); search != _flash_simulation.end()) {
    value = search->second;
  }
  return value;
}

void TestHelper::startApp(uint32_t app_flash_address) {
  (void)app_flash_address;
  _startAppCalled = true;
}

// Private functions --------------------------------------------------------------------------------------------------

void TestHelper::initFlashSim() {
  uint32_t flash_address = _handle.getFlashStartAddress();
  const uint32_t flash_size = _handle.getFlashSize();
  for (uint32_t idx = 0U; idx < flash_size; idx++) {
    _flash_simulation.insert({flash_address, std::numeric_limits<uint32_t>::max()});
    flash_address++;
  }
}

};  // namespace franklyboot::test_utils

// HWI calls ----------------------------------------------------------------------------------------------------------

namespace franklyboot {

void hwi::resetDevice() {
  if (test_utils::testInstance != nullptr) {
    test_utils::testInstance->resetDevice();
  }
}

[[nodiscard]] uint32_t hwi::getVendorID() {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->getVendorID();
  }

  return value;
}

[[nodiscard]] uint32_t hwi::getProductID() {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->getProductID();
  }

  return value;
}

[[nodiscard]] uint32_t hwi::getProductionDate() {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->getProductionDate();
  }

  return value;
}

[[nodiscard]] uint32_t hwi::getUniqueID() {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->getUniqueID();
  }

  return value;
}

[[nodiscard]] uint32_t hwi::calculateCRC(const uint32_t src_address, const uint32_t num_bytes) {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->calculateCRC(src_address, num_bytes);
  }

  return value;
}

[[nodiscard]] bool hwi::eraseFlashPage(const uint32_t page_id) {
  bool value = false;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->eraseFlashPage(page_id);
  }

  return value;
}

bool hwi::writeDataBufferToFlash(const uint32_t dst_address, const uint32_t dst_page_id, uint8_t* src_data_ptr,
                                 const uint32_t num_bytes) {
  bool value = false;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->writeDataBufferToFlash(dst_address, dst_page_id, src_data_ptr, num_bytes);
  }

  return value;
}

[[nodiscard]] uint8_t hwi::readByteFromFlash(uint32_t flash_src_address) {
  uint8_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->readByteFromFlash(flash_src_address);
  }

  return value;
}

void hwi::startApp(uint32_t app_flash_address) {
  if (test_utils::testInstance != nullptr) {
    test_utils::testInstance->startApp(app_flash_address);
  }
}

}  // namespace franklyboot