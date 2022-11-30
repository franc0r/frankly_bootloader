/**
 * @file test_utils.cpp
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
void TestHelper::setCRCResult(uint32_t value) { _calculated_crc = value; }

void TestHelper::setByteInFlash(uint32_t address, uint8_t value) {
  if (auto search = _flash_simulation.find(address); search != _flash_simulation.end()) {
    search->second = value;
  }
}

// HWI abstraction ----------------------------------------------------------------------------------------------------

void TestHelper::resetDevice() { _resetDeviceCalled = true; }
[[nodiscard]] uint32_t TestHelper::getVendorID() const { return _vendor_id; }
[[nodiscard]] uint32_t TestHelper::getProductID() const { return _product_id; }
[[nodiscard]] uint32_t TestHelper::getProductionDate() const { return _production_date; }
[[nodiscard]] uint32_t TestHelper::getUniqueID() const { return _unique_id; }
[[nodiscard]] uint32_t TestHelper::calculateCRC(const uint8_t* data_ptr, uint32_t byte_size) {
  (void)data_ptr;
  (void)byte_size;
  return _calculated_crc;
}

[[nodiscard]] bool TestHelper::eraseFlashPage(uint32_t page_id) {
  (void)page_id;
  return false;
}

bool TestHelper::writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id, const uint8_t* src_data_ptr,
                                        uint32_t byte_size) {
  (void)dst_address;
  (void)dst_page_id;
  (void)src_data_ptr;
  (void)byte_size;
  return false;
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

[[nodiscard]] uint32_t hwi::calculateCRC(const uint8_t* data_ptr, uint32_t byte_size) {
  uint32_t value = 0U;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->calculateCRC(data_ptr, byte_size);
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

bool hwi::writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id, const uint8_t* src_data_ptr,
                                 uint32_t byte_size) {
  bool value = false;
  if (test_utils::testInstance != nullptr) {
    value = test_utils::testInstance->writeDataBufferToFlash(dst_address, dst_page_id, src_data_ptr, byte_size);
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