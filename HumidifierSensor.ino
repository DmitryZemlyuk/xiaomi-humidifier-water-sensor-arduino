#include <CapacitiveSensor.h>

#define SENDER_PIN 3
#define SENSOR_PIN 2

#define SAMPLES_NUMBER 232
#define TIMEOUT_MS 500

#define MIN_READING 3900   // for 1M resistor and 1000 samples
#define MAX_READING 13100  // for 1M resistor and 1000 samples
#define PACKET_SIZE 42
#define CHECKSUM_BASE 0xA0
#define FULL_TANK 125
#define FAN_OFF 127
#define TAKEAWAY 2700

unsigned long previousMillis = 0;
const long interval = 200;

byte packet[] = { 0xFA, 0x29, 0x03, 0x00, 0x00, 0x00, 0x00, 0x14, 0x9A, 0x00, 0x00, 0x00, 0x03, 0x77, 0x72, 0x71, 0x03, 0x00, 0x6C, 0x4C, 0x3B, 0x03, 0x2F, 0x15, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x03, 0xE8, 0x00, 0x2c, 0x02, 0x6D, 0x37, 0xD2, 0x00 };

byte checksum;
byte waterLevel;
long readingRaw;

CapacitiveSensor sensor = CapacitiveSensor(SENDER_PIN, SENSOR_PIN);  // 1M resistor between pins 3 & 2, pin 2 is sensor pin

void setup() {
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF);  // turn off autocalibrate
  sensor.set_CS_Timeout_Millis(TIMEOUT_MS);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checksum = 0;
    packet[11] = 0;

    readingRaw = sensor.capacitiveSensorRaw(SAMPLES_NUMBER);

    if (readingRaw == -2) waterLevel = FULL_TANK;  // // tank full, water touches both metal probes, capacitance measurement not possible

    else if (readingRaw > TAKEAWAY && readingRaw < MIN_READING) waterLevel = FAN_OFF;  // turn off fan, available if air dry mode turned off, please check in Mi Home app

    else waterLevel = constrain(map(readingRaw, MIN_READING, MAX_READING, 0, 120), 0, 120);  // map sensor reading

    packet[11] = waterLevel;  // 12th byte represents water level value

    checksum = calculateChecksum(packet, PACKET_SIZE);
    packet[PACKET_SIZE] = checksum;  // last byte checksum

    Serial.write(packet, sizeof(packet));
    // Serial.println(readingRaw); // for calibration - raw value from water tank
    // Serial.println(packet[11]); // for calibration - water level for send to head
  }
}

byte calculateChecksum(byte* packet, int size) {
  byte checksum = 0;
  for (int i = 0; i < size; i++) {
    checksum ^= packet[i];
  }
  checksum ^= CHECKSUM_BASE;
  return checksum;
}
