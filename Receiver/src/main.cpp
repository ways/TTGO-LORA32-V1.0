#include <SPI.h>
#include <Wire.h>
#include <SSD1306.h>
#include <LoRa.h>
#include "images.h"

//#define LORA_BAND    433
//#define LORA_BAND    868
#define LORA_BAND    915

#define OLED_SDA    4
#define OLED_SCL    15
#define OLED_RST    16

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

SSD1306 display(0x3c, OLED_SDA, OLED_SCL);

// Forward declarations
void displayLoraData(int packetSize, String packet, String rssi);
void pktRecvTask(void *parm);
void showLogo();

TaskHandle_t recvLedTask;
volatile bool blinkNow;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver");

  // Create a new task on the second core to blink the LED when packets
  // are received.  After creation, give it a bit of time to startup
  xTaskCreatePinnedToCore(
    pktRecvTask,
    "pktRecvTask",
    1000,
    NULL,
    1,
    &recvLedTask,
    1);
    delay(500);

  // Configure OLED by setting the OLED Reset HIGH, LOW, and then back HIGH
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  delay(100);
  digitalWrite(OLED_RST, LOW);
  delay(100);
  digitalWrite(OLED_RST, HIGH);

  display.init();
  display.flipScreenVertically();

  showLogo();
  delay(2000);

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoRa Receiver");
  display.display();
  delay(2000);

  // Configure the LoRA radio
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(LORA_BAND * 1E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.enableCrc();
  Serial.println("init ok");

  // Set the radio into receive mode
  LoRa.receive();
  delay(1500);
}

void loop() {
  static unsigned long lastRecvTime = 0;

  unsigned long now = millis();
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packet = "";
    for (int i = 0; i < packetSize; i++) {
      packet += (char)LoRa.read();
    }
    String rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
    Serial.println(rssi);

    displayLoraData(packetSize, packet, rssi);
    blinkNow = true;
    lastRecvTime = now;
  } else if (now - lastRecvTime > 10 * 1000) {
      displayLoraData(0, "", "NoData: (Timeout)");
  }
}

void displayLoraData(int packetSize, String packet, String rssi) {
  String packSize = String(packetSize, DEC);

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 15 , "Received " + packSize + " bytes");
  display.drawStringMaxWidth(0 , 26 , 128, packet);
  display.drawString(0, 0, rssi);
  display.display();
}

void pktRecvTask(void *parm) {
  static const int BLINK_TIME_MS = 250;
    
  // Run forever
  pinMode(LED_BUILTIN, OUTPUT);
  while (1) {
    // Use the second core to blink the LED when we receive a packet
    if (blinkNow) {
      // toggle the led to give a visual indication the packet was sent
      digitalWrite(LED_BUILTIN, HIGH);
      delay(BLINK_TIME_MS);
      digitalWrite(LED_BUILTIN, LOW);

      // Done blinking, wait for the next packet
      blinkNow = false;
    }
  }
}

void showLogo() {
  uint8_t x_off = (display.getWidth() - logo_width) / 2;
  uint8_t y_off = (display.getHeight() - logo_height) / 2;

  display.clear();
  display.drawXbm(x_off, y_off, logo_width, logo_height, logo_bits);
  display.display();
}
