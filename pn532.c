#include "pn532.h"
#include <stddef.h>

void pn532_init(pn532 *dev, pn532_interface *interface)
{
    dev->interface = interface;
}

void pn532_begin(pn532 *dev)
{
    dev->interface->begin(dev->interface->context);
    dev->interface->wakeup(dev->interface->context);
}

uint32_t pn532_get_firmware_version(pn532 *dev)
{
    uint32_t response;
    dev->packet_buffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 1, NULL, 0)) {
        return 0;
    }
    int16_t status = dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0);
    if (status < 0) {
        return 0;
    }
    response = dev->packet_buffer[0];
    response <<= 8;
    response |= dev->packet_buffer[1];
    response <<= 8;
    response |= dev->packet_buffer[2];
    response <<= 8;
    response |= dev->packet_buffer[3];
    return response;
}

uint32_t pn532_read_register(pn532 *dev, uint16_t reg)
{
    uint32_t response;
    dev->packet_buffer[0] = PN532_COMMAND_READREGISTER;
    dev->packet_buffer[1] = (reg >> 8) & 0xFF;
    dev->packet_buffer[2] = reg & 0xFF;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 3, NULL, 0)) {
        return 0;
    }
    int16_t status = dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0);
    if (status < 0) {
        return 0;
    }
    response = dev->packet_buffer[0];
    return response;
}

uint32_t pn532_write_register(pn532 *dev, uint16_t reg, uint8_t val)
{
    dev->packet_buffer[0] = PN532_COMMAND_WRITEREGISTER;
    dev->packet_buffer[1] = (reg >> 8) & 0xFF;
    dev->packet_buffer[2] = reg & 0xFF;
    dev->packet_buffer[3] = val;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 4, NULL, 0)) {
        return 0;
    }
    int16_t status = dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0);
    if (status < 0) {
        return 0;
    }
    return 1;
}

bool pn532_write_gpio(pn532 *dev, uint8_t pinstate)
{
    pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);
    dev->packet_buffer[0] = PN532_COMMAND_WRITEGPIO;
    dev->packet_buffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate;
    dev->packet_buffer[2] = 0x00;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 3, NULL, 0)) {
        return false;
    }
    return (0 < dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0));
}

uint8_t pn532_read_gpio(pn532 *dev)
{
    dev->packet_buffer[0] = PN532_COMMAND_READGPIO;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 1, NULL, 0)) {
        return 0;
    }
    dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0);
    return dev->packet_buffer[0];
}

bool pn532_sam_config(pn532 *dev)
{
    dev->packet_buffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    dev->packet_buffer[1] = 0x01;
    dev->packet_buffer[2] = 0x14;
    dev->packet_buffer[3] = 0x01;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 4, NULL, 0)) {
        return false;
    }
    return (0 < dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0));
}

bool pn532_set_passive_activation_retries(pn532 *dev, uint8_t maxRetries)
{
    dev->packet_buffer[0] = PN532_COMMAND_RFCONFIGURATION;
    dev->packet_buffer[1] = 5;
    dev->packet_buffer[2] = 0xFF;
    dev->packet_buffer[3] = 0x01;
    dev->packet_buffer[4] = maxRetries;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 5, NULL, 0)) {
        return false;
    }
    return (0 < dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0));
}

bool pn532_set_rf_field(pn532 *dev, uint8_t autoRFCA, uint8_t rFOnOff)
{
    dev->packet_buffer[0] = PN532_COMMAND_RFCONFIGURATION;
    dev->packet_buffer[1] = 1;
    dev->packet_buffer[2] = 0x00 | autoRFCA | rFOnOff;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 3, NULL, 0)) {
        return false;
    }
    return (0 < dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), 0));
}

bool pn532_read_passive_target_id(pn532 *dev, uint8_t cardbaudrate,
                                  uint8_t *uid, uint8_t *uidLength,
                                  uint8_t uid_maxlen, uint16_t timeout)
{
    dev->packet_buffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    dev->packet_buffer[1] = 1;
    dev->packet_buffer[2] = cardbaudrate;
    if (dev->interface->write_command(dev->interface->context, dev->packet_buffer, 3, NULL, 0)) {
        return false;
    }
    if (dev->interface->read_response(dev->interface->context, dev->packet_buffer, sizeof(dev->packet_buffer), timeout) < 0) {
        return false;
    }
    if (dev->packet_buffer[0] != 1) {
        return false;
    }
    *uidLength = dev->packet_buffer[5];
    if (dev->packet_buffer[5] > uid_maxlen) {
        return false;
    }
    for (uint8_t i = 0; i < dev->packet_buffer[5]; i++) {
        uid[i] = dev->packet_buffer[6 + i];
    }
    return true;
}

