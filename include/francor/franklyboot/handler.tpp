/**
 * @file handler.tpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Bootloader Handler Class Template Deklarations
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include "handler.h"

#ifndef FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_
#define FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_

namespace franklyboot {

/** \brief Define for the template definition for better readibility */
#define FRANKLYBOOT_HANDLER_TEMPL \
  template <uint32_t FLASH_START, uint32_t FLASH_APP_FIRST_PAGE, uint32_t FLASH_SIZE, uint32_t FLASH_PAGE_SIZE>

/** \brief Prefix of template functions for better readability */
#define FRANKLYBOOT_HANDLER_TEMPL_PREFIX Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE>

FRANKLYBOOT_HANDLER_TEMPL
void Handler<FLASH_START, FLASH_APP_FIRST_PAGE, FLASH_SIZE, FLASH_PAGE_SIZE>::processBufferedCmds() {}

}; /* namespace franklyboot */

#endif /* FRANCOR_FRANKLYBOOT_HANDLER_TPP_H_ */