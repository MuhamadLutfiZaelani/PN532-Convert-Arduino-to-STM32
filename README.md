# PN532 STM32 HAL Driver

This repository contains a small C implementation of the PN532 NFC controller driver adapted for STM32 microcontrollers.  The code is derived from the Arduino library but exposes a C API so it can be used in bare-metal or STM32Cube projects.  A thin C++ wrapper (`PN532.h`/`PN532.cpp`) is also provided for compatibility.

The driver operates through a generic `pn532_interface`.  `pn532_uart_hal.c` implements this interface using STM32 HAL UART calls so the PN532 can be controlled over a hardware USART.

## HAL dependencies

The following HAL components must be enabled in your project:

- `HAL_UART_Transmit` and `HAL_UART_Receive` for sending and receiving frames.
- `HAL_GetTick` for timeout handling.
- `printf` (optional) when `PN532_DEBUG` is defined in `PN532_debug.h`.

## Example (STM32 Blackpill)

The snippet below shows a minimal setup using USART1 on a Blackpill board.  The UART is configured by CubeMX or your startup code before the driver is initialized.

```c
#include "stm32f1xx_hal.h"
#include "pn532.h"
#include "pn532_uart_hal.h"

extern UART_HandleTypeDef huart1;  // configured elsewhere

static pn532_uart_hal pn532_hal;
static pn532 nfc;

void nfc_init(void)
{
    pn532_uart_hal_init(&pn532_hal, &huart1);
    pn532_init(&nfc, &pn532_hal.interface);
    pn532_begin(&nfc);
    pn532_sam_config(&nfc);  // enable ISO14443A reader mode
}

void nfc_poll(void)
{
    uint8_t uid[7];
    uint8_t uid_len;
    if (pn532_read_passive_target_id(&nfc, PN532_MIFARE_ISO14443A,
                                     uid, &uid_len, 1000)) {
        // UID successfully read
    }
}
```

## Compiler and HAL configuration

1. Add all `.c` files from this repository to your STM32 project.
2. Compile with `USE_HAL_DRIVER` and your MCU family define (e.g. `STM32F1xx`).
3. Ensure the UART peripheral used by the PN532 is initialized before calling `pn532_uart_hal_init()`.
4. SysTick must be enabled so that `HAL_GetTick()` returns increasing values for timeout management.
5. (Optional) define `PN532_DEBUG` to enable `printf` logging from `PN532_debug.h`.

With these steps the driver can be used to communicate with the PN532 module from an STM32 Blackpill or any other board using the HAL.


