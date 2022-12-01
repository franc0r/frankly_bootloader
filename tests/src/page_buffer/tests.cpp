/**
 * @file tests.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 * @brief Unit Tests of FRANCORs Frankly Bootloader - General Requests
 * @version 0.1
 * @date 2022-11-26
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor/frankly_test_utils.h>

#include <limits>

using namespace franklyboot;              // NOLINT
using namespace franklyboot::test_utils;  // NOLINT

// Test Fixture Class -------------------------------------------------------------------------------------------------

/**
 * @brief Test class for simulation of device
 */
class PageBufferTests : public TestHelper {
 public:
  PageBufferTests() = default;
};

// Tests --------------------------------------------------------------------------------------------------------------

TEST_F(PageBufferTests, PageBufferInit) {  // NOLINT
  /* Check if buffer is initialized correctly */
  for (auto idx = 0U; idx < getHandle().getFlashPageSize(); idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(idx), std::numeric_limits<uint8_t>::max());
  }
}

TEST_F(PageBufferTests, PageBufferClear) {
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_CLEAR;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {0U};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }

  /* Check if buffer is cleared */
  for (auto idx = 0U; idx < getHandle().getFlashPageSize(); idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(idx), std::numeric_limits<uint8_t>::max());
  }
}

TEST_F(PageBufferTests, PageBufferReadWord) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_READ_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr uint32_t NUM_MSGS = (FLASH_PAGE_SIZE / 4U);

  /* Create random data for one page */
  std::array<uint8_t, FLASH_PAGE_SIZE> data_lst;
  for (auto& entry : data_lst) {
    entry = rand() % std::numeric_limits<uint8_t>::max();
  }

  /* Write data */
  for (auto data_word_idx = 0U; data_word_idx < NUM_MSGS; data_word_idx++) {
    const auto packet_id = static_cast<uint8_t>(data_word_idx & 0xFF);
    msg::Msg request = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_WORD, msg::RESP_NONE, packet_id);

    request.data[0] = data_lst.at((data_word_idx * 4) + 0U);
    request.data[1] = data_lst.at((data_word_idx * 4) + 1U);
    request.data[2] = data_lst.at((data_word_idx * 4) + 2U);
    request.data[3] = data_lst.at((data_word_idx * 4) + 3U);

    getHandle().processRequest(request);
  }

  /* Read first 4 valid bytes*/
  {
    constexpr uint32_t BYTE_IDX = 0;
    msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
    msg::convertU32ToMsgData(BYTE_IDX, request_msg.data);

    /* Process request and get response */
    getHandle().processRequest(request_msg);
    const auto response = getHandle().getResponse();

    /* Check response */
    EXPECT_EQ(response.request, REQUEST);
    EXPECT_EQ(response.response, EXPECTED_RESPONSE);

    EXPECT_EQ(response.data[0], data_lst.at(0));
    EXPECT_EQ(response.data[1], data_lst.at(1));
    EXPECT_EQ(response.data[2], data_lst.at(2));
    EXPECT_EQ(response.data[3], data_lst.at(3));
  }

  /* Read last 4 valid bytes*/
  {
    constexpr uint32_t BYTE_IDX = FLASH_PAGE_SIZE - 4U;
    msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
    msg::convertU32ToMsgData(BYTE_IDX, request_msg.data);

    /* Process request and get response */
    getHandle().processRequest(request_msg);
    const auto response = getHandle().getResponse();

    /* Check response */
    EXPECT_EQ(response.request, REQUEST);
    EXPECT_EQ(response.response, EXPECTED_RESPONSE);

    EXPECT_EQ(response.data[0], data_lst.at(data_lst.size() - 4U));
    EXPECT_EQ(response.data[1], data_lst.at(data_lst.size() - 3U));
    EXPECT_EQ(response.data[2], data_lst.at(data_lst.size() - 2U));
    EXPECT_EQ(response.data[3], data_lst.at(data_lst.size() - 1U));
  }
}

TEST_F(PageBufferTests, PageBufferReadWordInvalidByteIdx) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_READ_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr uint32_t BYTE_IDX = FLASH_PAGE_SIZE - 3U;

  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  msg::convertU32ToMsgData(BYTE_IDX, request_msg.data);

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);
}

TEST_F(PageBufferTests, PageBufferWriteOneWord) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_WORD;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData DATA = {1U, 2U, 3U, 4U};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < DATA.size(); idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(idx), DATA[idx]);
  }
}

TEST_F(PageBufferTests, PageBufferWritePage) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_WORD;
  constexpr uint32_t NUM_MSGS = (FLASH_PAGE_SIZE / 4U);

  /* Create random data for one page */
  std::array<uint8_t, FLASH_PAGE_SIZE> data_lst;
  for (auto& entry : data_lst) {
    entry = rand() % std::numeric_limits<uint8_t>::max();
  }

  for (auto data_word_idx = 0U; data_word_idx < NUM_MSGS; data_word_idx++) {
    const auto packet_id = static_cast<uint8_t>(data_word_idx & 0xFF);
    msg::Msg request = msg::Msg(REQUEST, msg::RESP_NONE, packet_id);

    request.data[0] = data_lst.at((data_word_idx * 4) + 0U);
    request.data[1] = data_lst.at((data_word_idx * 4) + 1U);
    request.data[2] = data_lst.at((data_word_idx * 4) + 2U);
    request.data[3] = data_lst.at((data_word_idx * 4) + 3U);

    getHandle().processRequest(request);

    const msg::Msg response = getHandle().getResponse();
    EXPECT_EQ(response.request, REQUEST);
    EXPECT_EQ(response.packet_id, packet_id);
    EXPECT_EQ(response.data[0], request.data[0]);
    EXPECT_EQ(response.data[1], request.data[1]);
    EXPECT_EQ(response.data[2], request.data[2]);
    EXPECT_EQ(response.data[3], request.data[3]);

    const bool last_word_in_buffer = (data_word_idx >= ((FLASH_PAGE_SIZE / 4) - 1U));
    if (last_word_in_buffer) {
      EXPECT_EQ(response.response, msg::RESP_ACK_PAGE_FULL);
    } else {
      EXPECT_EQ(response.response, msg::RESP_ACK);
    }
  }

  /* Check if data is written correctly to buffer */
  for (auto byte_idx = 0U; byte_idx < FLASH_PAGE_SIZE; byte_idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(byte_idx), data_lst.at(byte_idx));
  }
}

TEST_F(PageBufferTests, PageBufferWritePageOverflow) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_WORD;
  constexpr uint32_t NUM_MSGS = (FLASH_PAGE_SIZE / 4U);

  for (auto data_word_idx = 0U; data_word_idx < (NUM_MSGS + 1U); data_word_idx++) {
    const auto packet_id = static_cast<uint8_t>(data_word_idx & 0xFF);
    msg::Msg request = msg::Msg(REQUEST, msg::RESP_NONE, packet_id);

    getHandle().processRequest(request);

    const msg::Msg response = getHandle().getResponse();
    EXPECT_EQ(response.request, REQUEST);
    EXPECT_EQ(response.packet_id, packet_id);

    const bool last_word_in_buffer = ((data_word_idx + 1U) == NUM_MSGS);
    const bool expect_overflow = ((data_word_idx) == NUM_MSGS);
    if (last_word_in_buffer) {
      EXPECT_EQ(response.response, msg::RESP_ACK_PAGE_FULL);
    } else if (expect_overflow) {
      EXPECT_EQ(response.response, msg::RESP_ERR_PAGE_FULL);

    } else {
      EXPECT_EQ(response.response, msg::RESP_ACK);
    }
  }
}

TEST_F(PageBufferTests, PageBufferPacketIdError) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_WORD;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr uint32_t NUM_MSGS = (FLASH_PAGE_SIZE / 4U);

  const uint32_t packet_error_idx = rand() % NUM_MSGS;

  for (auto data_word_idx = 0U; data_word_idx < NUM_MSGS; data_word_idx++) {
    auto packet_id = static_cast<uint8_t>(data_word_idx & 0xFF);

    const bool set_wrong_packet_id = (packet_error_idx == data_word_idx);
    if (set_wrong_packet_id) {
      packet_id -= 1;
    }

    msg::Msg request = msg::Msg(REQUEST, msg::RESP_NONE, packet_id);

    getHandle().processRequest(request);

    const msg::Msg response = getHandle().getResponse();

    if (set_wrong_packet_id) {
      EXPECT_EQ(response.request, REQUEST);
      EXPECT_EQ(response.response, EXPECTED_RESPONSE);
      EXPECT_EQ(response.packet_id, packet_id);
    }
  }
}

TEST_F(PageBufferTests, PageBufferCheckDataCleared) {  // NOLINT
  constexpr uint8_t TEST_BYTE = 0xBEU;
  constexpr uint32_t NUM_MSGS = (FLASH_PAGE_SIZE / 4U);

  /* Fill page buffer */
  for (auto data_word_idx = 0U; data_word_idx < NUM_MSGS; data_word_idx++) {
    auto packet_id = static_cast<uint8_t>(data_word_idx & 0xFF);
    msg::Msg request = msg::Msg(msg::REQ_PAGE_BUFFER_WRITE_WORD, msg::RESP_NONE, packet_id);
    request.data = msg::MsgData({TEST_BYTE, TEST_BYTE, TEST_BYTE, TEST_BYTE});
    getHandle().processRequest(request);
  }

  /* Check if page buffer contains data */
  for (auto byte_idx = 0U; byte_idx < FLASH_PAGE_SIZE; byte_idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(byte_idx), TEST_BYTE);
  }

  /* Clear page buffer */
  const msg::Msg request = msg::Msg(msg::REQ_PAGE_BUFFER_CLEAR, msg::RESP_NONE, 0);
  getHandle().processRequest(request);
  const msg::Msg response = getHandle().getResponse();

  EXPECT_EQ(response.request, msg::REQ_PAGE_BUFFER_CLEAR);
  EXPECT_EQ(response.response, msg::RESP_ACK);

  /* Check if buffer is cleared */
  for (auto byte_idx = 0U; byte_idx < FLASH_PAGE_SIZE; byte_idx++) {
    EXPECT_EQ(getHandle().getByteFromPageBuffer(byte_idx), std::numeric_limits<uint8_t>::max());
  }
}

TEST_F(PageBufferTests, PageBufferCalcCRC) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_CALC_CRC;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {0x12, 0x34, 0x35, 0x78};

  /* Set CRC value */
  setCRCResult(msg::convertMsgDataToU32(EXPECTED_DATA));

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(PageBufferTests, PageBufferWriteToFlashInvldAddress) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr msg::MsgData EXPECTED_DATA = {0x01, 0x00, 0x00, 0x00};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(PageBufferTests, PageBufferWriteToFlashInvldAddress2) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR_INVLD_ARG;
  constexpr msg::MsgData EXPECTED_DATA = {0xFF, 0x00, 0x00, 0x00};

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(PageBufferTests, PageBufferWriteToFlash) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ACK;
  constexpr msg::MsgData EXPECTED_DATA = {0x04, 0x00, 0x00, 0x00};
  setWriteToFlashResult(true);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}

TEST_F(PageBufferTests, PageBufferWriteToFlashHWError) {  // NOLINT
  constexpr msg::RequestType REQUEST = msg::REQ_PAGE_BUFFER_WRITE_TO_FLASH;
  constexpr uint8_t PACKET_ID = 0;
  constexpr msg::ResponseType EXPECTED_RESPONSE = msg::RESP_ERR;
  constexpr msg::MsgData EXPECTED_DATA = {0x04, 0x00, 0x00, 0x00};
  setWriteToFlashResult(false);

  /* Create request */
  msg::Msg request_msg = msg::Msg(REQUEST, msg::RESP_NONE, PACKET_ID);
  request_msg.data = EXPECTED_DATA;

  /* Process request and get response */
  getHandle().processRequest(request_msg);
  const auto response = getHandle().getResponse();

  /* Check response */
  EXPECT_EQ(response.request, REQUEST);
  EXPECT_EQ(response.response, EXPECTED_RESPONSE);

  for (auto idx = 0U; idx < response.data.size(); idx++) {
    EXPECT_EQ(response.data.at(idx), EXPECTED_DATA.at(idx));
  }
}