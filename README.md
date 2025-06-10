# HydroKeep - Intelligent Water Level Monitoring System

**HydroKeep** is a project that uses a water pressure sensor to measure the water level in a tank, and it sends data and alerts through applications like LINE and Google Chat. This system allows you to easily monitor water levels and detect low water situations, enabling quick and accurate management.

## Features
- Uses a pressure sensor submerged in water to measure the water level.
- Displays the water level and remaining water volume on an LCD screen.
- Sends alerts when the water level drops below 10% via LINE and Google Chat.
- Sends daily water level reports at 6:00 AM every day.
- Configurable alerts and settings in the `config` file.

## Setup and Installation

### Requirements
1. **WiFi**: Connect the device to a WiFi network.
2. **LINE Messaging API**: Used to send notification messages via LINE.
3. **Google Chat Webhook**: Used to send notification messages to Google Chat.
4. **Water Pressure Sensor**: A sensor that can connect to the analog pin of a microcontroller.

### Setup Instructions
1. Connect to WiFi:
    - Enter your WiFi name and password in the `ssid` and `password` fields in the code.
2. LINE API:
    - Enter your **LINE Token** and **User ID** in the designated fields.
3. Google Chat Webhook:
    - Enter the Google Chat Webhook URL to send data to a group or user.
4. Sensor:
    - Connect the sensor to the analog pin of the microcontroller.

### Library Installation
This project uses the following libraries that need to be installed in the Arduino IDE:

- `WiFi.h`
- `HTTPClient.h`
- `Wire.h`
- `LiquidCrystal_I2C.h`
- `WiFiUdp.h`
- `NTPClient.h`

## Usage

1. Upload the code to your microcontroller (e.g., ESP32 or ESP8266).
2. Check the results on the LCD, which will display the water status and remaining water volume in the tank.
3. Configure alerts to be sent when the water level drops below 10%.
4. Receive daily reports via LINE or Google Chat.

## Customization

- You can customize various settings, such as the tank size, depth, and alert configurations, in the `config` section of the code.
- To add new features, like additional alerts or different display outputs, you can modify the code accordingly.

## Limitations
- The system works only online and requires a WiFi connection to send data.
- The displayed information depends on the accuracy of the sensor being used.

## Contact
- Pakorn Yaothong : [Developer Name]
- kb.h0liday44@gmail.com : [Developer Email]

Thank you for using **HydroKeep**!
