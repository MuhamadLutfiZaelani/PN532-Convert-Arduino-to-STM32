# PN532-Convert-Arduino-to-STM32

This library contains a port of the PN532 NFC driver for use with STM32 microcontrollers.

The original Arduino `PN532_HSU` implementation has been replaced with a HAL based
UART driver located in `pn532_uart_hal.c`.  Use `pn532_uart_hal_init()` to attach a
`UART_HandleTypeDef` to the driver before creating the `PN532` instance.
