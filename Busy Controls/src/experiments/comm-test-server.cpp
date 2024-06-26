#include <Arduino.h>
#include <Adafruit_SPIDevice.h>

#define SPIDEVICE_CS 5
Adafruit_SPIDevice spi_dev = Adafruit_SPIDevice(SPIDEVICE_CS, 500000);

void send_test() {
    Serial.print("Sending data ... ");
    uint8_t buffer[4] = {0, 1, 2, 3};
    spi_dev.write_then_read(buffer, 4, buffer, 4);
    Serial.println("done");

    Serial.print("Received: ");
    for (uint8_t i=0; i<4; i++) {
        Serial.print(buffer[i]); Serial.print(", ");
    }
    Serial.println();
}

void query_devices() {
    
}

void setup() {
    Serial.begin(115200);
    Serial.println("SPI device read and write test");

    if (!spi_dev.begin()) {
        Serial.println("Could not initialize SPI device");
        while (1);
    }
}

void loop() {
    send_test();
    delay(1000);
}