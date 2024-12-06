#include <CapacitiveSensor.h>

#define SENDER_PIN 3
#define SENSOR_PIN 2

const int SAMPLES_NUMBER = 232;
const int TIMEOUT_MS = 500;

const int MIN_READING = 3900;   // for 1M resistor
const int MAX_READING = 13100;  // for 1M resistor
const int PACKET_SIZE = 42;
const byte CHECKSUM_BASE = 0xA0;
const byte FULL_TANK = 125;
const byte FAN_OFF = 127;
const int TAKEAWAY = 2700;

const long INTERVAL = 200;

byte packet[PACKET_SIZE + 1] = { 
  0xFA, 0x29, 0x03, 0x00, 0x00, 0x00, 0x00, 0x14, 0x9A, 0x00, 0x00, 0x00, 
  0x03, 0x77, 0x72, 0x71, 0x03, 0x00, 0x6C, 0x4C, 0x3B, 0x03, 0x2F, 0x15, 
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x03, 0xE8, 
  0x00, 0x2c, 0x02, 0x6D, 0x37, 0xD2, 0x00 
};

unsigned long previousMillis = 0;
byte waterLevel = 0;
long readingRaw = 0;

CapacitiveSensor sensor = CapacitiveSensor(SENDER_PIN, SENSOR_PIN);  // 1M resistor between pins 3 & 2, pin 2 is sensor pin

void setup() {
  sensor.set_CS_AutocaL_Millis(0xFFFFFFFF);  // turn off autocalibrate
  sensor.set_CS_Timeout_Millis(TIMEOUT_MS);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;

    // Reading the raw capacitance value
    readingRaw = sensor.capacitiveSensorRaw(SAMPLES_NUMBER);

    // Determine water level based on readingRaw
    if (readingRaw == -2) {
      waterLevel = FULL_TANK;  // Tank full, water touches both metal probes
    }
    else if (readingRaw > TAKEAWAY && readingRaw < MIN_READING) {
      waterLevel = FAN_OFF;  // Turn off fan (air dry mode off)
    }
    else {
      // Map sensor reading to water level
      waterLevel = constrain(map(readingRaw, MIN_READING, MAX_READING, 0, 120), 0, 120);
    }

    // Update packet with new water level and calculate checksum
    packet[11] = waterLevel;
    packet[PACKET_SIZE] = calculateChecksum(packet, PACKET_SIZE);

    // Send packet over serial
    Serial.write(packet, sizeof(packet));

    // Uncomment for calibration
    // Serial.println(readingRaw); // Raw value from water tank
    // Serial.println(waterLevel); // Water level to send
  }
}

// Calculate checksum for the packet
byte calculateChecksum(const byte* packet, int size) {
  byte checksum = CHECKSUM_BASE;
  for (int i = 0; i < size; i++) {
    checksum ^= packet[i];
  }
  return checksum;
}
