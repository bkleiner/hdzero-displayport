# HDZero Arduino Library

Arduino Library to interact with HDZero VTXes via serial port.

![arduino_demo](https://user-images.githubusercontent.com/15615439/202908662-ba035698-c40f-4865-b21f-4e623e36fe30.gif)

## Features

- easy to use api for transmission of fc variant, rc channels and armed state
- simple drawing api

support for setting up VTX parameters (MSP VTX) is planned.

## Example

```cpp
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

void loop() {
    if (millis() - lastRefreshTime >= 500) {
        hdzero.clear(); // clear the screen
        hdzero.writeString(0, 10, 10, "HELLO ARDUINO!"); // write some text
        hdzero.draw(); // draw everything out

        lastRefreshTime = millis();
    }

    // updates the internal state, needs to be run every loop
    hdzero.run();
}
```
