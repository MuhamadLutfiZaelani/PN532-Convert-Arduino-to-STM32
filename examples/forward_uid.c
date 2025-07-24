#include "stm32f1xx_hal.h"
#include "pn532.h"
#include "pn532_uart_hal.h"

extern UART_HandleTypeDef huart1; // PN532 UART
extern UART_HandleTypeDef huart2; // secondary UART for output

static pn532_uart_hal pn532_hal;
static pn532 nfc;

void nfc_init(void)
{
    pn532_uart_hal_init(&pn532_hal, &huart1);
    pn532_init(&nfc, &pn532_hal.interface);
    pn532_begin(&nfc);
    pn532_sam_config(&nfc);
}

void nfc_loop(void)
{
    uint8_t uid[7];
    uint8_t uid_len;
    if (pn532_read_passive_target_id(&nfc, PN532_MIFARE_ISO14443A, uid, &uid_len, 1000)) {
        HAL_UART_Transmit(&huart2, uid, uid_len, HAL_MAX_DELAY);
    }
}
