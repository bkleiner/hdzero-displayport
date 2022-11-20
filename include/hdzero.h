#ifndef hdzero_h
#define hdzero_h

#include "Arduino.h"

#include "msp_protocol.h"

enum HDZeroResolution {
    SD_3016,
    HD_5018,
    HD_3016,
};

class HDZero {
  public:
    HDZero();

    void begin(Stream &stream);
    void end();

    // needs to be run every loop iteration
    void run();

    void setFcVariant(const char *variant);
    void setRcChannels(uint16_t values[MSP_RC_CHANNEL_COUNT]);
    void setRcChannel(uint8_t index, uint16_t value);
    void setAllRcChannels(uint16_t value);

    void setResolution(HDZeroResolution value);
    void setArmed(bool armed);

    void write(uint8_t fontpage, uint8_t x, uint8_t y, uint8_t *data, uint8_t size);
    void writeString(uint8_t fontpage, uint8_t x, uint8_t y, const char *string);
    void clear();
    void draw();

  private:
    void send(uint8_t cmd, uint8_t direction, uint8_t *payload, uint8_t size);
    void sendResponse(uint8_t cmd, uint8_t *payload, uint8_t size);
    void sendSubCmd(uint8_t cmd, uint8_t *payload, uint8_t size);
    void sendError(uint8_t cmd);
    void handleMsp(uint8_t cmd, uint8_t *payload, uint8_t size);

    Stream *stream;
    HDZeroResolution resolution;
    uint8_t fcVariant[MSP_FC_VARIANT_SIZE];
    uint16_t rcChannels[MSP_RC_CHANNEL_COUNT];
    bool isArmed;
    MSPVTXConfig vtxConfig;
};

#endif