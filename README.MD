TTGO V1.0
==============================

Adapted (changed pins for OLED) to the board marked:

TTGO ESP32 LoRa OLED T3_1.6 20180606

## Install the following Arduino libraries:
- [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- [oled-ssd1306](https://github.com/ThingPulse/esp8266-oled-ssd1306)

## Start
- In the Arduino sketches, ensure that the radio frequency you are using is commented/uncommented correctly
    ```
    #define LORA_BAND  433
    //#define LORA_BAND  868
    //#define LORA_BAND  915
    ```

In the Arduino IDE, I chose the board 'TTGO LoRa32-OLED V1', compiled the sketch
to verify it works, and then uploaded to the board.  You'll need two boards,
one to send, and one to receive. Ensure the LORA_BAND matches in both sketches
otherwise they won't be able to communicate.

## References
- https://github.com/YogoGit/TTGO-LORA32-V1.0
- https://github.com/lewisxhe/TTGO-LoRa-Series
- https://github.com/LilyGO/TTGO-LORA32-V2.0
- https://github.com/LilyGO/LORA-ESP32-OLED
- https://github.com/SensorsIot/LoRa-Board-Test-2
