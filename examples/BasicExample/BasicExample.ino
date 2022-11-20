#include <Arduino.h>

#include <hdzero.h>

HDZero hdzero;

void setup() {
    // wait for serial to be ready
    Serial2.begin(115200);
    while (!Serial2)
        ;

    // setup and configure hdzero
    hdzero.begin(Serial2);
    hdzero.setFcVariant("INAV");
    hdzero.setResolution(HD_5018);
}

uint32_t lastRefreshTime = 0;

int8_t pointXDirection = 1;
uint8_t pointX = 0;

int8_t pointYDirection = 1;
uint8_t pointY = 0;

void loop() {
    if (millis() - lastRefreshTime >= 500) {
        hdzero.clear();
        hdzero.writeString(0, 10, 10, "HELLO ARDUINO!");
        hdzero.writeString(0, pointX, pointY, "AHHH");

        pointX += pointXDirection;
        if (pointX + 4 >= 50 || pointX == 0) {
            pointXDirection = -pointXDirection;
        }
        pointY += pointYDirection;
        if (pointY >= 18 || pointY == 0) {
            pointYDirection = -pointYDirection;
        }
        hdzero.draw();

        lastRefreshTime = millis();
    }

    hdzero.run();
}