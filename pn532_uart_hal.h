#ifndef PN532_UART_HAL_H
#define PN532_UART_HAL_H

#include <stdint.h>
#include "pn532_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

typedef struct {
    pn532_interface interface;
    UART_HandleTypeDef *huart;
    uint8_t command;
} pn532_uart_hal;

void pn532_uart_hal_init(pn532_uart_hal *dev, UART_HandleTypeDef *huart);
void pn532_uart_hal_begin(void *ctx);
void pn532_uart_hal_wakeup(void *ctx);
int8_t pn532_uart_hal_write_command(void *ctx, const uint8_t *header, uint8_t hlen,
                                    const uint8_t *body, uint8_t blen);
int16_t pn532_uart_hal_read_response(void *ctx, uint8_t *buf, uint8_t len,
                                     uint16_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* PN532_UART_HAL_H */
