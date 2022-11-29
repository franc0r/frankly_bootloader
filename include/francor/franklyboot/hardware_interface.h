/**
 * @file hardware_interface.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Hardware Interface Header
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 */

#ifndef FRANCOR_FRANKLYBOOT_HARDWARE_INTERFACE_H_
#define FRANCOR_FRANKLYBOOT_HARDWARE_INTERFACE_H_

#ifdef __cplusplus

#include <cstdint>

/**
 * @brief Hardware interface / Hardware abstraction layer
 *
 * These functions has to be implemented due to integration on the
 * real hardware.
 */
namespace franklyboot::hwi {

/** \brief Performs a complete reset of the device */
void resetDevice();

/** \brief Get 32-bit vendor ID */
[[nodiscard]] uint32_t getVendorID();

/** \brief Get 32-bit product ID */
[[nodiscard]] uint32_t getProductID();

/** \brief Get 32-bit production date (seconds since 1.12.2023) */
[[nodiscard]] uint32_t getProductionDate();

/** \brief Get 32-bit unique ID */
[[nodiscard]] uint32_t getUniqueID();

/** \brief Calculates the 32-Bit crc value over the specified data array */
[[nodiscard]] uint32_t calculateCRC(const uint8_t* data_ptr, uint32_t byte_size);  // NOLINT

/** \brief Erase specified flash pages */
bool eraseFlashPage(uint32_t page_id);

/** \brief Writes a data buffer to flash / Writes are only performed for a complete page */
bool writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id, const uint8_t* src_data_ptr,
                            uint32_t byte_size);  // NOLINT

/** \brief Reads a byte from the desired address*/
[[nodiscard]] uint8_t readByteFromFlash(uint32_t flash_src_address);

/** \brief Starts the application and exits the bootloader */
void startApp(uint32_t app_flash_address);

}  // namespace franklyboot::hwi

#endif /* __cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_HARDWARE_INTERFACE_H_ */
