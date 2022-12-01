/**
 * @file frankly_test_utils.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Test Utils
 * @version 0.1
 * @date 2022-11-30
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

// Includes -----------------------------------------------------------------------------------------------------------
#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/handler.h>
#include <gtest/gtest.h>

#include <limits>

namespace franklyboot::test_utils {

// Defines / Constexpr ------------------------------------------------------------------------------------------------

constexpr uint32_t FLASH_START = 0x08000000U;
constexpr uint32_t FLASH_APP_FIRST_PAGE = 2U;
constexpr uint32_t FLASH_PAGE_SIZE = 1024U;
constexpr uint32_t FLASH_NUM_PAGES = 16;
constexpr uint32_t FLASH_SIZE = FLASH_NUM_PAGES * FLASH_PAGE_SIZE;

// Test Fixture Class -------------------------------------------------------------------------------------------------

class TestHelper : public ::testing::Test {
 public:
  /** \brief Default constructor */
  TestHelper() = default;

  /** \brief GTest functions */
  void SetUp() final;
  void TearDown() final;

  [[nodiscard]] auto& getHandle() { return _handle; }

  /** \brief Set functions */
  void setVendorID(uint32_t value);
  void setProductID(uint32_t value);
  void setProductionDate(uint32_t value);
  void setUniqueID(uint32_t value);
  void setCRCResult(uint32_t value);

  void setByteInFlash(uint32_t address, uint8_t value);

  /* Help functions */
  void clearPageBuffer();

  /* Check functions */
  [[nodiscard]] bool resetDeviceCalled() const;
  [[nodiscard]] bool startAppCalled() const;
  [[nodiscard]] uint32_t getCalcCRCSrcAddress() const;
  [[nodiscard]] uint32_t getCalcCRCNumBytes() const;

  /* Hardware interface simulation -> called by hwi:: functions */
  void resetDevice();
  [[nodiscard]] uint32_t getVendorID() const;
  [[nodiscard]] uint32_t getProductID() const;
  [[nodiscard]] uint32_t getProductionDate() const;
  [[nodiscard]] uint32_t getUniqueID() const;
  [[nodiscard]] uint32_t calculateCRC(const uint32_t src_address, uint32_t num_bytes);  // NOLINT
  bool eraseFlashPage(uint32_t page_id);
  bool writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id, uint32_t src_address, uint32_t num_bytes);
  [[nodiscard]] uint8_t readByteFromFlash(uint32_t flash_src_address);
  void startApp(uint32_t app_flash_address);

 private:
  void initFlashSim();

  Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> _handle;

  bool _resetDeviceCalled = {false};
  uint32_t _vendor_id = {0U};
  uint32_t _product_id = {0U};
  uint32_t _production_date = {0U};
  uint32_t _unique_id = {0U};

  uint32_t _crc_calc_src_address = {0U};
  uint32_t _crc_calc_num_bytes = {0U};
  uint32_t _crc_calc_result = {0U};

  bool _startAppCalled = {false};

  std::map<uint32_t, uint8_t> _flash_simulation;
};

};  // namespace franklyboot::test_utils