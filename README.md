# Landmine Finder Shoes

An ESP32-based wearable system that detects landmines using a metal detector sensor, provides haptic feedback via vibration motor, and logs GPS coordinates and altitude data.

## Features

- **Metal Detection**: Detects metal objects using a metal detector sensor with noise filtering
- **GPS Tracking**: Records latitude and longitude when metal is detected
- **Altitude/Pressure Monitoring**: Uses BMP180 barometric pressure sensor
- **Haptic Alert**: Vibration motor pulses when landmine is detected
- **Real-time Telemetry**: Periodic status output showing sensor readings

## Hardware Requirements

- ESP32 development board
- GPS module (TinyGPS++)
- BMP180 barometric pressure sensor
- Metal detector sensor
- Vibration motor (ERM type)
- External pulldown resistor (~57kΩ)

## Pin Configuration

| Pin | Function |
|-----|----------|
| 16 | GPS RX |
| 17 | GPS TX |
| 32 | Metal detector input |
| 18 | Motor PWM output |
| 21 | I2C SDA |
| 22 | I2C SCL |

## Connection Flowchart

```
                         ┌─────────────┐
                         │    ESP32    │
                         └──────┬──────┘
                                │
          ┌─────────────────────┼─────────────────────┐
          │                     │                     │
          ▼                     ▼                     ▼
   ┌──────────── ┌─────────────┐     ─┐      ┌─────────────┐
   │    GPS     │      │   BMP180    │      │   Metal     │
   │  Module    │      │  Sensor    │      │  Detector   │
   └──────┬──────┘      └──────┬──────┘      └──────┬──────┘
          │                     │                     │
    TX→RX │               I2C   │               INPUT │
   (GPIO17)                  (GPIO21,22)        (GPIO32)
                                  │                     │
                                  │              ┌─────┴─────┐
                                  │              │ 57kΩ      │
                                  │              │ Pulldown   │
                                  │              └───────────┘
                                  │
                                  └────────┐
                                           │
                                    ┌───────▼───────┐
                                    │  Vibration   │
                                    │    Motor      │
                                    └───────┬───────┘
                                            │
                                      PWM   │
                                     (GPIO18)
```

## Wiring Details

### GPS Module
- GPS TX → ESP32 GPIO 16 (RX)
- GPS RX → ESP32 GPIO 17 (TX)
- VCC → 3.3V
- GND → GND

### BMP180 Sensor
- SDA → ESP32 GPIO 21
- SCL → ESP32 GPIO 22
- VCC → 3.3V
- GND → GND

### Metal Detector
- Signal → ESP32 GPIO 32
- GND → GND
- Add 57kΩ pulldown resistor to GPIO 32

### Vibration Motor
- Positive → ESP32 GPIO 18 (via MOSFET)
- Negative → GND

## Installation

1. Install the required libraries:
   - TinyGPS++
   - Adafruit BMP085 (BMP180)
   - Wire (built-in)
   - HardwareSerial (built-in)

2. Upload `LandmineFinderShoes.ino` to your ESP32 board

3. Configure serial monitor at 115200 baud

## How It Works

1. The metal detector sensor outputs HIGH when metal is nearby
2. The system uses majority voting (20 samples) to filter noise
3. When valid detection occurs:
   - GPS coordinates are captured
   - Altitude and pressure are recorded
   - Vibration motor pulses 3 times as haptic alert
4. Cooldown period prevents repeated alerts for the same detection
5. Periodic status updates every second

## License

MIT License
