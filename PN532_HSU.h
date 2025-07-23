
#ifndef __PN532_HSU_H__
#define __PN532_HSU_H__

#include "PN532Interface.h"
#include "Arduino.h"
#include "pn532_interface.h"

#define PN532_HSU_DEBUG

#define PN532_HSU_READ_TIMEOUT						(1000)

typedef struct {
    pn532_interface interface;
    HardwareSerial* _serial;
    uint8_t command;
} PN532_HSU;

void pn532_hsu_init(PN532_HSU *hsu, HardwareSerial &serial);
void pn532_hsu_begin(void *ctx);
void pn532_hsu_wakeup(void *ctx);
int8_t pn532_hsu_write_command(void *ctx, const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
int16_t pn532_hsu_read_response(void *ctx, uint8_t buf[], uint8_t len, uint16_t timeout);

#endif
