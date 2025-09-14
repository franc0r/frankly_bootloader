/**
 * @file handler.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 */

#ifndef FRANCOR_FRANKLYBOOT_HANDLER_H_
#define FRANCOR_FRANKLYBOOT_HANDLER_H_

#ifdef __cplusplus

#include <francor/franklyboot/franklyboot.h>
#include <francor/franklyboot/hardware_interface.h>
#include <francor/franklyboot/msg.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

/**
 * @brief Groups all definitions of the frankly boot bootloader
 */
namespace franklyboot {

/**
 * @brief Frankly Bootloader Handler
 *
 * @param FLASH_START Start address of flash (e.g. for STM32 0x08000000)
 * @param FLASH_APP_FIRST_PAGE Page idx where application area starts
 * @param FLASH_SIZE Size of complete flash including bootloader
 * @param FLASH_PAGE_SIZE Size of a flash page
 */
template <uint32_t FLASH_START, uint32_t FLASH_APP_FIRST_PAGE, uint32_t FLASH_SIZE, uint32_t FLASH_PAGE_SIZE>
class Handler {
 public:
  enum class CommandBuffer {
    NONE,          //!< Do nothing
    RESET_DEVICE,  //!< Reset device
    START_APP,     //!< Start application
  };

  /**
   * @brief Construct a new Handler object
   */
  Handler();

  /**
   * @brief Process buffered commands
   *
   * Processes buffered commands, which cannot be executed immadetly,
   * because otherwise a response cannot be send. This function will do nothing
   * if no command is buffered.
   */
  void processBufferedCmds();

  /**
   * @brief Processes a bootloader request
   *
   * @param msg Received message from network
   */
  void processRequest(const msg::Msg& msg);

  /**
   * @brief Get the response of the request
   *
   * Every processRequest() will produce a response, which
   * shall be transmitted through the network
   *
   * @return auto Response message
   */
  auto getResponse() const;

  /**
   * @brief Checks if a valid app is available in flash
   *
   * Compares the stored CRC with the calculated CRC of the flash
   * If both are equal the flashed app is valid
   */
  [[nodiscard]] bool isAppValid() const;

  /* Getters */
  [[nodiscard]] auto getFlashStartAddress() const { return FLASH_START; }
  [[nodiscard]] auto getFlashSize() const { return FLASH_SIZE; }
  [[nodiscard]] auto getFlashPageSize() const { return FLASH_PAGE_SIZE; }
  [[nodiscard]] auto getFlashNumPages() const { return FLASH_NUM_PAGES; }
  [[nodiscard]] auto getFlashAppFirstPage() const { return FLASH_APP_FIRST_PAGE; }
  [[nodiscard]] auto getFlashAppAddress() const { return FLASH_APP_ADDRESS; }
  [[nodiscard]] auto getFlashAppNumPages() const { return FLASH_APP_NUM_PAGES; }
  [[nodiscard]] auto getFlashAppCRCValueAddress() const { return FLASH_APP_CRC_VALUE_ADDRESS; }

  [[nodiscard]] auto getByteFromPageBuffer(uint32_t byte_idx) const;

 private:
  /* General requests */
  void handleReqPing();
  void handleReqResetDevice();
  void handleReqStartApp(const msg::Msg& request);

  /* Device information */
  void handleReqInfoBootloaderVer();
  void handleReqInfoBootloaderCRC();
  void handleReqInfoVendorID();
  void handleReqInfoProductID();
  void handleReqInfoProductionDate();
  void handleReqInfoUniqueID(msg::RequestType request);

  /* Flash information */
  void handleReqFlashStartAddress();
  void handleReqFlashPageSize();
  void handleReqFlashNumPages();

  /* App Information */
  void handleReqAppPageIdx();
  void handleReqAppCrcCalc();
  void handleReqAppCrcStrd();

  /* Flash Read commands */
  void handleReqFlashReadWord(const msg::Msg& request);

  /* Page Buffer Commands */
  void handleReqPageBufferClear();
  void handleReqPageBufferReadWord(const msg::Msg& request);
  void handleReqPageBufferWriteWord(const msg::Msg& request);
  void handleReqPageBufferCalcCrc();
  void handleReqPageBufferWriteToFlash(const msg::Msg& request);

  /* Flash Write Commands*/
  void handleReqFlashWriteErasePage(const msg::Msg& request);
  void handleReqFlashWriteAppCrc(const msg::Msg& request);

  [[nodiscard]] uint32_t getPageBufferAddress() const;
  [[nodiscard]] uint32_t calcAppCRC() const;
  [[nodiscard]] uint32_t readAppCRCFromFlash() const;

  /** \brief Command buffer for commands which cannot be processed immediatly */
  CommandBuffer _cmd_buffer = {CommandBuffer::NONE};

  msg::Msg _response = {msg::Msg()};  //!< Response message

  /* Page Buffer */
  std::array<uint8_t, FLASH_PAGE_SIZE> _page_buffer;  //!< Page buffer
  uint32_t _page_buffer_pos = {0U};                   //!< Current write position of page buffer

  /* Static Data */

  /** \brief Number of flash pages */
  static constexpr uint32_t FLASH_NUM_PAGES = {FLASH_SIZE / FLASH_PAGE_SIZE};

  /** \brief App start address in flash */
  static constexpr uint32_t FLASH_APP_ADDRESS = {FLASH_START + FLASH_APP_FIRST_PAGE * FLASH_PAGE_SIZE};

  /** \brief Number of application flash pages */
  static constexpr uint32_t FLASH_APP_NUM_PAGES = {FLASH_NUM_PAGES - FLASH_APP_FIRST_PAGE};

  /** \brief Location of CRC value */
  static constexpr uint32_t FLASH_APP_CRC_VALUE_ADDRESS = {FLASH_START + FLASH_SIZE - 4U};

  /* STATIC ASSERT TESTS */
  static_assert(FLASH_SIZE > 0, "FLASH_SIZE cannot be 0!");
  static_assert(FLASH_SIZE > FLASH_PAGE_SIZE, "FLASH_SIZE cannot be smaller than PAGE_SIZE!");
  static_assert(FLASH_PAGE_SIZE > 0, "FLASH_PAGE_SIZE cannot be 0!");
  static_assert(FLASH_APP_FIRST_PAGE > 0,
                "FLASH_APP_FIRST_PAGE has to be > 0, because otherwise it will overwrite the bootloader!");
  static_assert(FLASH_APP_FIRST_PAGE < FLASH_NUM_PAGES,
                "FLASH_APP_FIRST_PAGE cannot be >= than the maximum page number!");
};

}; /* namespace franklyboot */

#include "handler.tpp"

#endif /* __cplusplus */

#endif /* FRANCOR_FRANKLYBOOT_HANDLER_H_ */
