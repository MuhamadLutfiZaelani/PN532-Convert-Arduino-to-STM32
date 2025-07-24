#include "pn532_uart_hal.h"
#include "PN532Interface.h"
#include "PN532_debug.h"
#include <string.h>

static int8_t pn532_uart_hal_readAckFrame(pn532_uart_hal *dev);

void pn532_uart_hal_init(pn532_uart_hal *dev, UART_HandleTypeDef *huart)
{
    dev->huart = huart;
    dev->command = 0;
    dev->interface.begin = pn532_uart_hal_begin;
    dev->interface.wakeup = pn532_uart_hal_wakeup;
    dev->interface.write_command = pn532_uart_hal_write_command;
    dev->interface.read_response = pn532_uart_hal_read_response;
    dev->interface.context = dev;
}

void pn532_uart_hal_begin(void *ctx)
{
    /* UART is expected to be already initialized by HAL */
    (void)ctx;
}

void pn532_uart_hal_wakeup(void *ctx)
{
    pn532_uart_hal *dev = (pn532_uart_hal *)ctx;
    uint8_t frame[] = {0x55, 0x55, 0x00, 0x00, 0x00};
    HAL_UART_Transmit(dev->huart, frame, sizeof(frame), HAL_MAX_DELAY);
}

int8_t pn532_uart_hal_write_command(void *ctx, const uint8_t *header, uint8_t hlen,
                                    const uint8_t *body, uint8_t blen)
{
    pn532_uart_hal *dev = (pn532_uart_hal *)ctx;

    dev->command = header[0];

    uint8_t frame[8 + hlen + blen];
    uint8_t idx = 0;

    frame[idx++] = PN532_PREAMBLE;
    frame[idx++] = PN532_STARTCODE1;
    frame[idx++] = PN532_STARTCODE2;

    uint8_t length = hlen + blen + 1;
    frame[idx++] = length;
    frame[idx++] = (uint8_t)(~length + 1);

    frame[idx++] = PN532_HOSTTOPN532;
    uint8_t sum = PN532_HOSTTOPN532;

    for (uint8_t i = 0; i < hlen; i++) {
        frame[idx++] = header[i];
        sum += header[i];
    }

    for (uint8_t i = 0; i < blen; i++) {
        frame[idx++] = body[i];
        sum += body[i];
    }

    frame[idx++] = (uint8_t)(~sum + 1);
    frame[idx++] = PN532_POSTAMBLE;

    if (HAL_UART_Transmit(dev->huart, frame, idx, HAL_MAX_DELAY) != HAL_OK) {
        return PN532_TIMEOUT;
    }

    return pn532_uart_hal_readAckFrame(dev);
}

int16_t pn532_uart_hal_read_response(void *ctx, uint8_t *buf, uint8_t len,
                                     uint16_t timeout)
{
    pn532_uart_hal *dev = (pn532_uart_hal *)ctx;
    uint8_t tmp[3];

    if (HAL_UART_Receive(dev->huart, tmp, 3, timeout) != HAL_OK) {
        return PN532_TIMEOUT;
    }
    if (tmp[0] != 0 || tmp[1] != 0 || tmp[2] != 0xFF) {
        return PN532_INVALID_FRAME;
    }

    uint8_t length_arr[2];
    if (HAL_UART_Receive(dev->huart, length_arr, 2, timeout) != HAL_OK) {
        return PN532_TIMEOUT;
    }
    if (0 != (uint8_t)(length_arr[0] + length_arr[1])) {
        return PN532_INVALID_FRAME;
    }
    length_arr[0] -= 2;
    if (length_arr[0] > len) {
        return PN532_NO_SPACE;
    }

    uint8_t cmd = dev->command + 1;
    if (HAL_UART_Receive(dev->huart, tmp, 2, timeout) != HAL_OK) {
        return PN532_TIMEOUT;
    }
    if (tmp[0] != PN532_PN532TOHOST || tmp[1] != cmd) {
        return PN532_INVALID_FRAME;
    }

    if (HAL_UART_Receive(dev->huart, buf, length_arr[0], timeout) != HAL_OK) {
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for (uint8_t i = 0; i < length_arr[0]; i++) {
        sum += buf[i];
    }

    if (HAL_UART_Receive(dev->huart, tmp, 2, timeout) != HAL_OK) {
        return PN532_TIMEOUT;
    }
    if ((uint8_t)(sum + tmp[0]) != 0 || tmp[1] != 0x00) {
        return PN532_INVALID_FRAME;
    }

    return length_arr[0];
}

static int8_t pn532_uart_hal_readAckFrame(pn532_uart_hal *dev)
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];

    if (HAL_UART_Receive(dev->huart, ackBuf, sizeof(PN532_ACK), PN532_ACK_WAIT_TIME) != HAL_OK) {
        return PN532_TIMEOUT;
    }

    if (memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) != 0) {
        return PN532_INVALID_ACK;
    }
    return 0;
}
