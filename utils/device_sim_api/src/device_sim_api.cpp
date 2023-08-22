/**
 * @file device_sim_api.c
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief API for device simulation between C++ and RUST
 * @version 1.0
 * @date 2023-01-16
 *
 * @copyright Copyright (c) 2023 - BSD-3-clause - FRANCOR e.V.
 *
 */

#ifdef __cplusplus

// Includes -----------------------------------------------------------------------------------------------------------
#include <francor/franklyboot/device_sim_api.h>
#include <francor/franklyboot/handler.h>

#include <cstring>
#include <vector>

// Private typedefs ----------------------------------------------------------------------------------------------------

using namespace franklyboot;

using SimDeviceHandler = Handler<sim_device::FLASH_APP_START_ADDR, sim_device::FLASH_APP_FIRST_PAGE,
                                 sim_device::FLASH_SIZE, sim_device::FLASH_PAGE_SIZE>;

class SimDevice {
 public:
  explicit SimDevice(uint8_t node_id) : _node_id(node_id) {}

  void broadcastMsg(const msg::Msg& msg) {
    _request_msg = msg;
    _new_broadcast_msg = true;
  }

  void nodeMsg(const msg::Msg& msg) {
    _request_msg = msg;
    _new_msg = true;
  }

  void processRequest() {
    if (_new_msg || _new_broadcast_msg) {
      _handler.processRequest(_request_msg);
      _new_response_msg = _new_msg;
      _new_broadcast_response_msg = _new_broadcast_msg;

      _new_msg = false;
      _new_broadcast_msg = false;
    }
  }

  [[nodiscard]] msg::Msg getResponseMsg() {
    _new_response_msg = false;
    _new_broadcast_response_msg = false;
    return _handler.getResponse();
  }

  [[nodiscard]] uint8_t getNodeId() const { return _node_id; }
  [[nodiscard]] bool isBroadcastResponseAvl() const { return _new_broadcast_response_msg; }
  [[nodiscard]] bool isNodeResponseAvl() const { return _new_response_msg; }

 private:
  const uint8_t _node_id;  //!< Node ID of the device

  msg::Msg _request_msg;              //!< Last request msg
  bool _new_msg = {false};            //!< Flag if new request msg is available
  bool _new_broadcast_msg = {false};  //!< Flag if new request msg is a broadcast msg

  bool _new_response_msg = {false};            //!< Flash indicating that new response is avl
  bool _new_broadcast_response_msg = {false};  //!< Flag indicating that new broadcast response is avl

  SimDeviceHandler _handler;  //!< Handler for the device
};

// Private Variables --------------------------------------------------------------------------------------------------
static std::vector<SimDevice> sim_device_lst;

// Public Functions ---------------------------------------------------------------------------------------------------

extern "C" void SIM_reset() { sim_device_lst.clear(); }

extern "C" bool SIM_addDevice(uint8_t node_id) {
  // Check if device already exists
  for (const auto& device : sim_device_lst) {
    if (device.getNodeId() == node_id) {
      return false;
    }
  }

  // Add new device
  sim_device_lst.emplace_back(node_id);

  return true;
}

extern "C" uint32_t SIM_getDeviceCount() { return static_cast<uint32_t>(sim_device_lst.size()); }

extern "C" void SIM_sendBroadcastMsg(uint8_t* const raw_msg_ptr) {
  msg::MsgRaw request_msg_raw = msg::MsgRaw();
  std::memcpy(request_msg_raw.data(), raw_msg_ptr, request_msg_raw.size());
  const auto request_msg = msg::convertBytesToMsg(request_msg_raw);

  // Loop through all devices and send message
  for (auto& device : sim_device_lst) {
    device.broadcastMsg(request_msg);
  }
}

extern "C" void SIM_sendNodeMsg(const uint8_t node_id, uint8_t* const raw_msg_ptr) {
  msg::MsgRaw request_msg_raw = msg::MsgRaw();
  std::memcpy(request_msg_raw.data(), raw_msg_ptr, request_msg_raw.size());
  const auto request_msg = msg::convertBytesToMsg(request_msg_raw);

  // Loop through all devices and send message to specified node
  for (auto& device : sim_device_lst) {
    if (node_id == device.getNodeId()) {
      device.nodeMsg(request_msg);
    }
  }
}

/** \brief Update devices */
extern "C" void SIM_updateDevices() {
  for (auto& device : sim_device_lst) {
    device.processRequest();
  }
}

/** \brief Get broadcast response msg */
extern "C" bool SIM_getBroadcastResponseMsg(uint8_t* node_id, uint8_t* raw_msg_ptr) {
  // Loop through all devices and send message to specified node
  for (auto& device : sim_device_lst) {
    if (device.isBroadcastResponseAvl()) {
      const auto response_msg = device.getResponseMsg();
      const auto response_msg_raw = msg::convertMsgToBytes(response_msg);
      std::memcpy(raw_msg_ptr, response_msg_raw.data(), response_msg_raw.size());
      (*node_id) = device.getNodeId();
      return true;
    }
  }

  return false;
}

/** \brief Get node specific response msg */
extern "C" bool SIM_getNodeResponseMsg(const uint8_t node_id, uint8_t* raw_msg_ptr) {
  // Loop through all devices and send message to specified node
  for (auto& device : sim_device_lst) {
    if (node_id == device.getNodeId() && device.isNodeResponseAvl()) {
      const auto response_msg = device.getResponseMsg();
      const auto response_msg_raw = msg::convertMsgToBytes(response_msg);
      std::memcpy(raw_msg_ptr, response_msg_raw.data(), response_msg_raw.size());
      return true;
    }
  }

  return false;
}

// Device HWI ---------------------------------------------------------------------------------------------------------

void hwi::resetDevice() {}

[[nodiscard]] uint32_t hwi::getVendorID() { return sim_device::VENDOR_ID; }

[[nodiscard]] uint32_t hwi::getProductID() { return sim_device::PRODUCT_ID; }

[[nodiscard]] uint32_t hwi::getProductionDate() { return sim_device::PRODUCTION_DATE; }

[[nodiscard]] uint32_t hwi::getUniqueID() { return 0U; }

uint32_t hwi::calculateCRC(uint32_t src_address, uint32_t num_bytes) {
  (void)src_address;
  (void)num_bytes;
  return 0;
}

bool hwi::eraseFlashPage(uint32_t page_id) {
  (void)page_id;
  return true;
}

bool hwi::writeDataBufferToFlash(uint32_t dst_address, uint32_t dst_page_id, uint8_t* src_data_ptr,
                                 uint32_t num_bytes) {
  (void)dst_address;
  (void)dst_page_id;
  (void)src_data_ptr;
  (void)num_bytes;

  return true;
}

[[nodiscard]] uint8_t franklyboot::hwi::readByteFromFlash(uint32_t flash_src_address) {
  (void)(flash_src_address);
  return 0;
}

void franklyboot::hwi::startApp(uint32_t app_flash_address) {  // Disable interrupts
  (void)app_flash_address;
}

#endif /* __cplusplus */