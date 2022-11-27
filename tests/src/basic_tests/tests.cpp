/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - Basic Tests
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/handler.h>
#include <gtest/gtest.h>

/**
 * @brief Test if static values are calculated correctly from the template arguments
 */
TEST(BasicTests, checkFlashInfoGetFuncions)  // NOLINT
{
  constexpr uint32_t FLASH_START = 0x08000000U;
  constexpr uint32_t FLASH_APP_FIRST_PAGE = 2U;
  constexpr uint32_t FLASH_PAGE_SIZE = 1024U;
  constexpr uint32_t FLASH_NUM_PAGES = 16;
  constexpr uint32_t FLASH_SIZE = FLASH_NUM_PAGES * FLASH_PAGE_SIZE;
  constexpr uint32_t FLASH_APP_NUM_PAGES = 14;

  franklyboot::Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE> handler;

  EXPECT_EQ(handler.getFlashStartAddress(), FLASH_START);
  EXPECT_EQ(handler.getFlashAppFirstPage(), FLASH_APP_FIRST_PAGE);
  EXPECT_EQ(handler.getFlashPageSize(), FLASH_PAGE_SIZE);
  EXPECT_EQ(handler.getFlashSize(), FLASH_SIZE);
  EXPECT_EQ(handler.getFlashNumPages(), FLASH_NUM_PAGES);
  EXPECT_EQ(handler.getFlashAppNumPages(), FLASH_APP_NUM_PAGES);
}

/**
 * @brief Check if 32-bit unsigned int value is packed correctly to the msg buffer
 *
 */
TEST(BasicTests, convertU32ToMsgData) {
  // Implement test
  EXPECT_EQ(true, false);
}

/**
 * @brief Check if the msg buffer is correctly converted to a unsigned int 32-bit value
 *
 */
TEST(BasicTests, convertMsgDataToU32) {
  // Implement test
  EXPECT_EQ(true, false);
}
