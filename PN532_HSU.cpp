#include "PN532_HSU.h"
#include "PN532_debug.h"

static int8_t pn532_hsu_readAckFrame(PN532_HSU *hsu);
static int8_t pn532_hsu_receive(PN532_HSU *hsu, uint8_t *buf, int len, uint16_t timeout = PN532_HSU_READ_TIMEOUT);

void pn532_hsu_init(PN532_HSU *hsu, HardwareSerial &serial)
{
    hsu->_serial = &serial;
    hsu->command = 0;
    hsu->interface.begin = pn532_hsu_begin;
    hsu->interface.wakeup = pn532_hsu_wakeup;
    hsu->interface.write_command = pn532_hsu_write_command;
    hsu->interface.read_response = pn532_hsu_read_response;
    hsu->interface.context = hsu;
}

void pn532_hsu_begin(void *ctx)
{
    PN532_HSU *hsu = (PN532_HSU *)ctx;
    hsu->_serial->begin(115200);
}

void pn532_hsu_wakeup(void *ctx)
{
    PN532_HSU *hsu = (PN532_HSU *)ctx;
    hsu->_serial->write(0x55);
    hsu->_serial->write(0x55);
    hsu->_serial->write(0);
    hsu->_serial->write(0);
    hsu->_serial->write(0);

    if (hsu->_serial->available()) {
        DMSG("Dump serial buffer: ");
    }
    while (hsu->_serial->available()) {
        uint8_t ret = hsu->_serial->read();
        DMSG_HEX(ret);
    }
}

int8_t pn532_hsu_write_command(void *ctx, const uint8_t *header, uint8_t hlen,
                               const uint8_t *body, uint8_t blen)
{
    PN532_HSU *hsu = (PN532_HSU *)ctx;

    if (hsu->_serial->available()) {
        DMSG("Dump serial buffer: ");
    }
    while (hsu->_serial->available()) {
        uint8_t ret = hsu->_serial->read();
        DMSG_HEX(ret);
    }

    hsu->command = header[0];

    hsu->_serial->write(PN532_PREAMBLE);
    hsu->_serial->write(PN532_STARTCODE1);
    hsu->_serial->write(PN532_STARTCODE2);

    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    hsu->_serial->write(length);
    hsu->_serial->write((uint8_t)(~length + 1));         // checksum of length

    hsu->_serial->write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA

    DMSG("\nWrite: ");

    hsu->_serial->write(header, hlen);
    for (uint8_t i = 0; i < hlen; i++) {
        sum += header[i];
        DMSG_HEX(header[i]);
    }

    hsu->_serial->write(body, blen);
    for (uint8_t i = 0; i < blen; i++) {
        sum += body[i];
        DMSG_HEX(body[i]);
    }

    uint8_t checksum = (uint8_t)(~sum + 1);            // checksum of TFI + DATA
    hsu->_serial->write(checksum);
    hsu->_serial->write(PN532_POSTAMBLE);

    return pn532_hsu_readAckFrame(hsu);
}

int16_t pn532_hsu_read_response(void *ctx, uint8_t buf[], uint8_t len, uint16_t timeout)
{
    PN532_HSU *hsu = (PN532_HSU *)ctx;
    uint8_t tmp[3];

    DMSG("\nRead:  ");

    if (pn532_hsu_receive(hsu, tmp, 3, timeout) <= 0) {
        return PN532_TIMEOUT;
    }
    if (0 != tmp[0] || 0 != tmp[1] || 0xFF != tmp[2]) {
        DMSG("Preamble error");
        return PN532_INVALID_FRAME;
    }

    uint8_t length_arr[2];
    if (pn532_hsu_receive(hsu, length_arr, 2, timeout) <= 0) {
        return PN532_TIMEOUT;
    }
    if (0 != (uint8_t)(length_arr[0] + length_arr[1])) {
        DMSG("Length error");
        return PN532_INVALID_FRAME;
    }
    length_arr[0] -= 2;
    if (length_arr[0] > len) {
        return PN532_NO_SPACE;
    }

    uint8_t cmd = hsu->command + 1;               // response command
    if (pn532_hsu_receive(hsu, tmp, 2, timeout) <= 0) {
        return PN532_TIMEOUT;
    }
    if (PN532_PN532TOHOST != tmp[0] || cmd != tmp[1]) {
        DMSG("Command error");
        return PN532_INVALID_FRAME;
    }

    if (pn532_hsu_receive(hsu, buf, length_arr[0], timeout) != length_arr[0]) {
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for (uint8_t i = 0; i < length_arr[0]; i++) {
        sum += buf[i];
    }

    if (pn532_hsu_receive(hsu, tmp, 2, timeout) <= 0) {
        return PN532_TIMEOUT;
    }
    if (0 != (uint8_t)(sum + tmp[0]) || 0 != tmp[1]) {
        DMSG("Checksum error");
        return PN532_INVALID_FRAME;
    }

    return length_arr[0];
}

static int8_t pn532_hsu_readAckFrame(PN532_HSU *hsu)
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];

    DMSG("\nAck: ");

    if (pn532_hsu_receive(hsu, ackBuf, sizeof(PN532_ACK), PN532_ACK_WAIT_TIME) <= 0) {
        DMSG("Timeout\n");
        return PN532_TIMEOUT;
    }

    if (memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK))) {
        DMSG("Invalid\n");
        return PN532_INVALID_ACK;
    }
    return 0;
}

static int8_t pn532_hsu_receive(PN532_HSU *hsu, uint8_t *buf, int len, uint16_t timeout)
{
    int read_bytes = 0;
    int ret;
    unsigned long start_millis;

    while (read_bytes < len) {
        start_millis = millis();
        do {
            ret = hsu->_serial->read();
            if (ret >= 0) {
                break;
            }
        } while ((timeout == 0) || ((millis() - start_millis) < timeout));

        if (ret < 0) {
            if (read_bytes) {
                return read_bytes;
            } else {
                return PN532_TIMEOUT;
            }
        }
        buf[read_bytes] = (uint8_t)ret;
        DMSG_HEX(ret);
        read_bytes++;
    }
    return read_bytes;
}
