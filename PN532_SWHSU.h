
#ifndef __PN532_SWHSU_H__
#define __PN532_SWHSU_H__

#include <SoftwareSerial.h>

#include "PN532Interface.h"
#include "Arduino.h"
#include "pn532_interface.h"

#define PN532_SWHSU_DEBUG

#define PN532_SWHSU_READ_TIMEOUT						(1000)

typedef struct {
    pn532_interface interface;
    SoftwareSerial* _serial;
    uint8_t command;
} PN532_SWHSU;

void pn532_swhsu_init(PN532_SWHSU *swhsu, SoftwareSerial &serial);
void pn532_swhsu_begin(void *ctx);
void pn532_swhsu_wakeup(void *ctx);
int8_t pn532_swhsu_write_command(void *ctx, const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
int16_t pn532_swhsu_read_response(void *ctx, uint8_t buf[], uint8_t len, uint16_t timeout);

#endif
