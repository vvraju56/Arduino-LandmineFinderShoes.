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

## I2C Configuration

- SDA: GPIO 21
- SCL: GPIO 22

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
