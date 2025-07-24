#ifndef PN532_UART_HAL_H
#define PN532_UART_HAL_H

#include <stdint.h>
#include "pn532_interface.h"

/* Frame constants formerly in PN532Interface.h */
#define PN532_PREAMBLE      (0x00)
#define PN532_STARTCODE1    (0x00)
#define PN532_STARTCODE2    (0xFF)
#define PN532_POSTAMBLE     (0x00)

#define PN532_HOSTTOPN532   (0xD4)
#define PN532_PN532TOHOST   (0xD5)

#define PN532_ACK_WAIT_TIME (10)  /* ms */

/* Error codes used by the UART HAL */
#define PN532_INVALID_ACK   (-1)
#define PN532_TIMEOUT       (-2)
#define PN532_INVALID_FRAME (-3)
#define PN532_NO_SPACE      (-4)

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
