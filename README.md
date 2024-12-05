# xiaomi-humidifier-water-sensor-arduino
Xiaomi SmartMi Evaporative Humidifier Water Level Sensor CJXJSQ02ZM, CJXJSQ04ZM for Arduino Nano

Special thanks to Tomas Vilda for his excellent work reverse-engineering the water sensor's serial communication: [GitHub link](https://github.com/tomasvilda/humidifierstart)

This project is based on: [GitHub link](https://github.com/fakcior/xiaomi-humidifier-water-sensor/tree/master?tab=readme-ov-file)

Fixes:

- Added functionality to stop the fan and disk drive when the top of the humidifier is removed (this works when <b>"Air Dry"</b> mode is turned <b>OFF</b>; so please check this switch in the Mi Home app).
- Improved performance by replacing delay(); with millis(); and other optimizations.

This sketch uses the Capacitive Touch Library (CapacitiveSensor.h) to utilize the existing hardware.

To set up:

- Solder a 1MΩ resistor between two pins on your board (in this sketch, pins D2 and D3 are used).
- Connect or solder your board as a direct replacement for the blue sensor module on the power supply PCB (5V, GND, TX, and both probes). Looking from the top of the 2-pin header, the left pin is the sensing pin, and the right one is GND.

This sketch also works for the black sensor module in the CJXJSQ04ZM model (located near the power input connector).

<b>Calibration:</b> calibrate your board under three conditions:

- When the top of the humidifier is removed
- When the humidifier is fully assembled with an empty tank
- When the tank is full.
Adjust the TAKEAWAY, MIN_READING, and MAX_READING definitions accordingly based on the sensor readings (use Serial.println(readingRaw); to monitor values).

Important: It is recommended to calibrate the sensor by slowly filling the tank with water while monitoring the sensor values. Record the maximum value. When the water reaches a certain level, it will short both metal rods, and the measurement will return -2, which is expected behavior.
