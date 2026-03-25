# smart-queue-watch
# ⌚ Smart Queue Watch

## Overview
This project implements a **Smart Queue Watch** that helps users monitor their position in a queue in real time.  
It uses an **ESP32 microcontroller** to communicate with a mobile app developed in **Xcode (iOS)**, allowing users to receive updates and notifications directly on their watch.

---

## Features
- Real-time queue monitoring  
- Wireless communication via ESP32  
- Mobile app interface for iOS (Xcode)  
- Notifications for queue updates  
- Portable and wearable solution  

---

## Software
- ESP32 firmware (Arduino IDE / PlatformIO)  
- iOS mobile app written in **Xcode** (Swift)  
- Communication via Bluetooth  

---

## How It Works
1. ESP32 tracks the user’s queue position and sends updates.  
2. The mobile app receives these updates and displays them on the watch interface.  
3. Users are notified when their turn is approaching.  

---

## Usage
1. Flash the ESP32 firmware.  
2. Connect the watch to the iOS app via Wi-Fi or Bluetooth.  
3. Open the app to monitor your queue in real time.  
4. Receive notifications when it’s your turn.  
