#ifndef PN532_C_DRIVER_H
#define PN532_C_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "pn532_interface.h"

/* Commands used by the minimal C driver */
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)

#define PN532_GPIO_VALIDATIONBIT            (0x80)
#define PN532_GPIO_P32                      (2)
#define PN532_GPIO_P34                      (4)

/* Baud rate codes for pn532_read_passive_target_id() */
#define PN532_MIFARE_ISO14443A              (0x00)

typedef struct pn532 {
    pn532_interface *interface;
    uint8_t packet_buffer[64];
} 
pn532;

void pn532_init(pn532 *dev, pn532_interface *interface);
void pn532_begin(pn532 *dev);
uint32_t pn532_get_firmware_version(pn532 *dev);
uint32_t pn532_read_register(pn532 *dev, uint16_t reg);
uint32_t pn532_write_register(pn532 *dev, uint16_t reg, uint8_t val);
bool pn532_write_gpio(pn532 *dev, uint8_t pinstate);
uint8_t pn532_read_gpio(pn532 *dev);
bool pn532_sam_config(pn532 *dev);
bool pn532_set_passive_activation_retries(pn532 *dev, uint8_t maxRetries);
bool pn532_set_rf_field(pn532 *dev, uint8_t autoRFCA, uint8_t rFOnOff);
bool pn532_read_passive_target_id(pn532 *dev, uint8_t cardbaudrate,
                                  uint8_t *uid, uint8_t *uidLength,
                                  uint8_t uid_maxlen, uint16_t timeout);

#endif /* PN532_C_DRIVER_H */
