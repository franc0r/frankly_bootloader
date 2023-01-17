/**
 * @file device_sim_api.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief API for device simulation between C++ and RUST
 * @version 1.0
 * @date 2023-01-16
 *
 * @copyright Copyright (c) 2023 - BSD-3-clause - FRANCOR e.V.
 *
 */

#ifndef DEVICE_SIM_API_H_
#define DEVICE_SIM_API_H_

#include <cstddef>
#ifdef __cplusplus

// Includes -----------------------------------------------------------------------------------------------------------
#include <francor/franklyboot/sim_device_defines.h>
#include <stdint.h>

// Public Functions ---------------------------------------------------------------------------------------------------

/** \brief Reset device list */
extern "C" void SIM_reset();

/** \brief Adds a new device to simulator */
extern "C" bool SIM_addDevice(uint8_t node_id);

/** \brief Get number of devices registered to simulator */
extern "C" uint32_t SIM_getDeviceCount();

/** \brief Send broadcast message to all devices */
extern "C" void SIM_sendBroadcastMsg(uint8_t* const raw_msg_ptr);

/** \brief Send node specific message to device */
extern "C" void SIM_sendNodeMsg(uint8_t node_id, uint8_t* const raw_msg_ptr);

/** \brief Update devices */
extern "C" void SIM_updateDevices();

/** \brief Get broadcast response msg */
extern "C" void SIM_getBroadcastResponseMsg(uint8_t* raw_msg_ptr);

/** \brief Get node specific response msg */
extern "C" void SIM_getNodeResponseMsg(uint8_t node_id, uint8_t* raw_msg_ptr);

#endif /* __cplusplus */

#endif /* DEVICE_SIM_API_H_ */