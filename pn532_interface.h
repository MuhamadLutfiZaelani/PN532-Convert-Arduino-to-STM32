#ifndef PN532_INTERFACE_H
#define PN532_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pn532_interface {
    void (*begin)(void *context);
    void (*wakeup)(void *context);
    int8_t (*write_command)(void *context, const uint8_t *header, uint8_t hlen,
                            const uint8_t *body, uint8_t blen);
    int16_t (*read_response)(void *context, uint8_t *buf, uint8_t len,
                             uint16_t timeout);
    void *context;
} pn532_interface;

#ifdef __cplusplus
}
#endif

#endif /* PN532_INTERFACE_H */
