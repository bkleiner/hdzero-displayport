#include "hdzero.h"

#ifdef DEBUG_HDZERO
#define debug_printf(...) Serial.printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

HDZero::HDZero()
    : resolution(SD_3016)
    , isArmed(false) {
    setFcVariant("BTFL");
    setAllRcChannels(0);
}

void HDZero::begin(Stream &_stream) {
    stream = &_stream;
}

void HDZero::end() {
    stream = nullptr;
}

void HDZero::setFcVariant(const char *variant) {
    int strLen = strlen(variant);
    for (uint8_t i = 0; i < min(MSP_FC_VARIANT_SIZE, strLen); i++) {
        fcVariant[i] = variant[i];
    }
}

void HDZero::setRcChannels(uint16_t values[MSP_RC_CHANNEL_COUNT]) {
    memcpy(rcChannels, values, MSP_RC_CHANNEL_COUNT * sizeof(uint16_t));
}

void HDZero::setRcChannel(uint8_t index, uint16_t value) {
    rcChannels[index] = value;
}

void HDZero::setAllRcChannels(uint16_t value) {
    for (uint8_t i = 0; i < 16; i++) {
        setRcChannel(i, value);
    }
}

void HDZero::setArmed(bool armed) {
    isArmed = armed;
}

void HDZero::setResolution(HDZeroResolution value) {
    resolution = value;

    uint8_t options[2] = {
        0,
        value,
    };
    sendSubCmd(MSP_DP_SUBCMD_CONFIG, options, 2);
}

void HDZero::write(uint8_t fontpage, uint8_t x, uint8_t y, uint8_t *data, uint8_t size) {
    uint8_t buf[size + 3];
    buf[0] = y;
    buf[1] = x;
    buf[2] = fontpage;

    memcpy(buf + 3, data, size);

    sendSubCmd(MSP_DP_SUBCMD_WRITE, buf, size + 3);
}

void HDZero::writeString(uint8_t fontpage, uint8_t x, uint8_t y, const char *string) {
    write(fontpage, x, y, (uint8_t *)string, strlen(string));
}

void HDZero::clear() {
    sendSubCmd(MSP_DP_SUBCMD_CLEAR, nullptr, 0);
}

void HDZero::draw() {
    sendSubCmd(MSP_DP_SUBCMD_DRAW, nullptr, 0);
}

void HDZero::send(uint8_t cmd, uint8_t direction, uint8_t *payload, uint8_t size) {
    if (stream == nullptr) {
        return;
    }

    stream->write('$');
    stream->write('M');
    stream->write(direction);
    stream->write(size);
    stream->write(cmd);

    uint8_t crc = (uint8_t)(size) ^ (uint8_t)(cmd);
    for (uint8_t i = 0; i < size; i++) {
        const uint8_t byte = payload[i];
        stream->write(byte);
        crc ^= byte;
    }

    stream->write(crc);
}

void HDZero::sendResponse(uint8_t cmd, uint8_t *payload, uint8_t size) {
    send(cmd, '>', payload, size);
}

void HDZero::sendError(uint8_t cmd) {
    send(cmd, '!', nullptr, 0);
}

void HDZero::sendSubCmd(uint8_t cmd, uint8_t *data, uint8_t size) {
    uint8_t payload[size + 1];
    payload[0] = cmd;
    memcpy(payload + 1, data, size);
    sendResponse(MSP_DISPLAYPORT, payload, size + 1);
}

void HDZero::handleMsp(uint8_t cmd, uint8_t *payload, uint8_t size) {
    switch (cmd) {
    case MSP_FC_VARIANT:
        sendResponse(cmd, fcVariant, MSP_FC_VARIANT_SIZE);
        break;

    case MSP_STATUS: {
        uint8_t data[22];
        memset(data, 0, 22);

        uint32_t flight_mode = 0;
        if (isArmed) {
            flight_mode |= 0x1;
        }

        data[6] = flight_mode >> 0;
        data[7] = flight_mode >> 8;
        data[8] = flight_mode >> 16;
        data[9] = flight_mode >> 24;

        sendResponse(cmd, data, 22);
        break;
    }

    case MSP_RC: {
        sendResponse(cmd, (uint8_t *)rcChannels, MSP_RC_CHANNEL_COUNT * sizeof(uint16_t));
        break;
    }

    case MSP_VTX_CONFIG: {
        // sendResponse(cmd, (uint8_t *)vtxConfig, sizeof(MSPVTXConfig));
        break;
    }

    default:
        debug_printf("unhandled msp command %d (%d)\r\n", cmd, size);
        sendError(cmd);
        break;
    }
}

void HDZero::run() {
    if (stream == nullptr) {
        return;
    }

    while (true) {
        int magic = stream->read();
        if (magic == -1) {
            return;
        }
        if (magic == '$') {
            break;
        }
        debug_printf("invalid magic 0x%x\r\n", magic);
    }

    int version = stream->read();
    int direction = stream->read();
    if (version != 'M' || direction != '<') {
        return;
    }

    int size = stream->read();
    int cmd = stream->read();
    if (size == -1 || cmd == -1) {
        return;
    }

    uint8_t payload[size];
    uint8_t incoming_crc = (uint8_t)(size) ^ (uint8_t)(cmd);
    for (uint8_t i = 0; i < size; i++) {
        int byte = stream->read();
        if (byte == -1) {
            return;
        }
        incoming_crc ^= (uint8_t)(byte);
        payload[i] = byte;
    }

    uint8_t crc = stream->read();
    if (crc != incoming_crc) {
        debug_printf("invalid crc 0x%x vs 0x%x\r\n", crc, incoming_crc);
        return;
    }

    handleMsp(cmd, payload, size);
}