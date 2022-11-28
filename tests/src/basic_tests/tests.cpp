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

using namespace franklyboot;

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
 */
TEST(BasicTests, convertU32ToMsgData) {
  constexpr uint32_t TEST_U32_VALUE = 0xDEADBEEFU;
  constexpr msg::MsgData EXPECTED_VALUES = {0xEF, 0xBE, 0xAD, 0xDE};

  msg::MsgData msg_data;
  msg::convertU32ToMsgData(TEST_U32_VALUE, msg_data);

  for (auto idx = 0U; idx < EXPECTED_VALUES.size(); idx++) {
    EXPECT_EQ(EXPECTED_VALUES[idx], msg_data[idx]);
  }
}

/**
 * @brief Check if the msg buffer is correctly converted to a unsigned int 32-bit value
 */
TEST(BasicTests, convertMsgDataToU32) {
  constexpr uint32_t EXPECTED_VALUE = 0xDEADBEEFU;
  constexpr msg::MsgData TEST_MSG_DATA = {0xEF, 0xBE, 0xAD, 0xDE};

  const uint32_t msg_value = msg::convertMsgDataToU32(TEST_MSG_DATA);

  EXPECT_EQ(msg_value, EXPECTED_VALUE);
}
