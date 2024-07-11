# LoRaWAN Dog Tracker Project

This repository contains the implementation details of our LoRaWAN-based dog tracking project. Our objective was to create a dog tracker using the LoRa and LoRaWAN protocols, localizing the dog's position through triangulation using RSSI values.

## Table of Contents

1. [Introduction](#introduction)
2. [Components and Requirements](#components-and-requirements)
3. [System Architecture](#system-architecture)
4. [Localization Method](#localization-method)
5. [Setup and Usage](#setup-and-usage)
6. [Challenges and Solutions](#challenges-and-solutions)
7. [Future Scope](#future-scope)
8. [Contributors](#contributors)

## Introduction

The main objective of this project is to make a dog tracker using LoRa and LoRaWAN protocols. We implemented a basic triangulation method to localize the dog by calculating linear distances using Received Signal Strength Indicator (RSSI) values.

## Components and Requirements

### Components

1. **Beacon**:
   - Arduino Nano
   - RFM95 LoRa module
   - 9V battery
   - 3D printed case

2. **Tracker**:
   - ESP32
   - RFM95 LoRa module
   - Power supply

3. **Gateway**:
   - LoRaWAN gateway

### Requirements

- Node.js
- Arduino IDE
- LoRaWAN network server
- 3D printer (for the beacon case)

## System Architecture

### LoRa and LoRaWAN

- **LoRa**: Long-range wireless technology for sending small amounts of data over long distances.
- **LoRaWAN**: The network protocol that links the LoRa signal (sensor data) to the application.

### Working of Components

1. **Beacon**: 
   - Pings to the receiver every 5 seconds.
   - Sends RSSI values to the tracker.

2. **Tracker**:
   - Pings to the gateway every 10 seconds.
   - Sends RSSI values received from the beacon to the gateway.

3. **Gateway**:
   - Receives LoRa modulated RF messages from any end device and forwards them to the LoRaWAN network server.

## Localization Method

Instead of using GPS, we use RSSI for localization. The linear distance is calculated using the formula:

$$
\text{Distance} = 10^{\frac{A - \text{RSSI}}{10n}}
$$


Where:
-  ```A```  is the standard RSSI value corresponding to a 1m distance.
-  ```RSSI``` is the received RSSI value.
-  ```n```is the environmental factor.

## Setup and Usage

### Setting Up the Tracker

1. Connect the ESP32 to the RFM95 LoRa module.
2. Program the ESP32 to send and receive data via LoRaWAN.

### Setting Up the Beacon

1. Connect the Arduino Nano to the RFM95 LoRa module.
2. Program the Arduino to send RSSI values to the tracker.

### Running the Gateway

1. Set up the LoRaWAN gateway and connect it to the network server.
2. Use a Node.js script to obtain data from the gateway.

### Node.js Script

```javascript
const request = require('request');

request('http://your-gateway-url/data', (error, response, body) => {
  if (!error && response.statusCode == 200) {
    console.log(body); // Print the RSSI values
  }
});
```

## Challenges and Solutions

### Problems Faced

- **Pin Size Differences**: Soldered pins carefully to fit the RFM95 module.
- **Temperature Sensitivity**: Monitored and regulated soldering temperature.
- **Compact Beacon Design**: Placed components strategically to keep the beacon compact and robust.

## Future Scope

- **Improved Localization**: Implementing Linear Least Squares (LLS) to consider errors in distance calculation.
- **Network Expansion**: Adding more receivers for better accuracy and coverage.

## Contributors

- Autrio Das (2022112007)
- Aryan Garg (2022102074)
- Samkit Jain (2022102062)
