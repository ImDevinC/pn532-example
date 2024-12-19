# Sample App
This is a simple POC designed to showcase an issue with an ESP32 device not being able to read from a PN532 device using [esp-idf ](https://idf.espressif.com/) in SPI mode.

The devices in question:
- [DOIT ESP32 DevKit v1](https://www.circuitstate.com/pinouts/doit-esp32-devkit-v1-wifi-development-board-pinout-diagram-and-reference/)
- [PN532](https://www.makerfabs.com/pn532-nfc-module-v3.html)

Wiring:
- SCLK -> GPIO18
- MISO -> GPIO19
- MOSI -> GPIO23
- SS -> GPIO5

# Building
You will need esp-idf installed: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html
`make build`

# Flashing
`make flash`
This will both flash and monitor your device

# Expected Result
The `GetFirmwareVersion` command should return a proper value

# Actual Result
```
I (1326) PN532: Received Response:
I (1326) PN532: 0x00
I (1326) PN532: 0x00 
I (1326) PN532: 0x00                       
I (1326) PN532: 0x00                                 
I (1326) PN532: 0x00    
I (1326) PN532: 0x00    
I (1336) PN532: 0x00
I (1336) PN532: 0x00
I (1336) PN532: 0xFF
E (1336) PN532: Invalid response from PN532
```

## Addendum
This code was created by ChatGPT as I have very little context of esp-idf and just wanted a simple project to validate some findings
