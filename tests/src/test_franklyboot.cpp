/**
 * @file test_franklyboot.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs FranklyBoot Bootloader
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/handler.h>
#include <gtest/gtest.h>

TEST(FRANKLYBOOT, checkFlashInfoGetFuncions)  // NOLINT
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