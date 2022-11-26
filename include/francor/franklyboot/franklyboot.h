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

#include <cstddef>
#include <cstdint>

/**
 * @brief Groups all definitions of the frankly boot bootloader
 */
namespace franklyboot {

/** \brief Version of frankly boot bootloader */
namespace version {
    constexpr size_t MAJOR_IDX = {0U}; //!< Array index of major version
    constexpr size_t MINOR_IDX = {1U}; //!< Array index of minor version
    constexpr size_t PATCH_IDX = {2U}; //!< Array index of patch version

    constexpr uint8_t VERSION[3U] = {0U, 1U, 0U}; //!< Array containing bootloader version
}

}; // namespace franklyboot

#endif /*__cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_H_ */
