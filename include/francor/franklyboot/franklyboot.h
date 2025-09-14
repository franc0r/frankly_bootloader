/**
 * @file franklyboot.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Main header of FRANCORs - Frankly Boot bootloader
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#ifndef FRANCOR_FRANKLYBOOT_H_
#define FRANCOR_FRANKLYBOOT_H_

#ifdef __cplusplus

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

/**
 * @brief Groups all definitions of the frankly boot bootloader
 */
namespace franklyboot {

/**
 * @brief Check if code is compiled on a 64 bit system
 *
 */
constexpr bool is64BitSystem() {                      // NOLINT
  if constexpr (sizeof(void*) == sizeof(uint64_t)) {  // NOLINT
    return true;                                      // NOLINT
  } else {                                            // NOLINT
    return false;                                     // NOLINT
  }                                                   // NOLINT
}

/** \brief Version of frankly boot bootloader */
namespace version {
constexpr size_t MAJOR_IDX = {0U};  //!< Array index of major version
constexpr size_t MINOR_IDX = {1U};  //!< Array index of minor version
constexpr size_t PATCH_IDX = {2U};  //!< Array index of patch version

constexpr std::array<uint8_t, 3U> VERSION = {0U, 1U, 0U};  //!< Array containing bootloader version

}; /* namespace version */

}; /* namespace franklyboot*/

#endif /*__cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_H_ */
