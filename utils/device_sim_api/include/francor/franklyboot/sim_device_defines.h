/**
 * @file sim_device_defines.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Frankly Bootloader API Firmware Header Device Definitions
 * @version 1.0
 * @date 2023-01-17
 *
 * @copyright Copyright (c) 2023 - BSD-3-clause - FRANCOR e.V.
 */

#ifndef DEVICE_DEFINES_H_
#define DEVICE_DEFINES_H_

// Includes -----------------------------------------------------------------------------------------------------------
#include <stdint.h>

// Public Functions ---------------------------------------------------------------------------------------------------

#ifdef __cplusplus

namespace sim_device {
constexpr uint32_t BROADCAST_ID = 0x780;

constexpr uint32_t VENDOR_ID = {0x46524352};
constexpr uint32_t PRODUCT_ID = {0x054455354};
constexpr uint32_t PRODUCTION_DATE = {0x00000000U};

constexpr uint32_t FLASH_START_ADDR = {0x08000000U};
constexpr uint32_t FLASH_APP_FIRST_PAGE = {4U};
constexpr uint32_t FLASH_SIZE = {1024 * 1024U};
constexpr uint32_t FLASH_PAGE_SIZE = {2048U};
constexpr uint32_t FLASH_APP_START_ADDR = FLASH_START_ADDR + FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE;
};  // namespace sim_device

#endif /* __cplusplus */

#endif /* DEVICE_DEFINES_H_ */