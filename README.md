# Sentry-V1: Advanced Surveillance RC Car 🚗 

Sentry-V1 is a robust, custom-built surveillance RC car featuring real-time video streaming, remote control, and GPS tracking from anywhere in the world. Designed with an ESP32-CAM and ESP32 Dev Module, this bot ensures seamless monitoring and control over the internet.

---

## 🔥 Features  
- **🖥️ Real-Time Video Streaming**: The ESP32-CAM streams video over the internet using WebSockets (currently tested with PieSocket).  
- **🎮 Remote Control**: A web app with a joystick interface controls the bot via Firebase RTDB.  
- **🔄 Pan-Tilt Camera**: The ESP32-CAM is mounted on a pan-tilt setup with two servos, controlled via Firebase RTDB.  
- **🔋 Battery Monitoring**: The ESP32 Dev Module uploads battery voltages to Firebase RTDB.  
- **📍 GPS Tracking**: A NEO-6M GPS module provides real-time tracking, with location data sent to Firebase RTDB.
- **📡 Wi-Fi Signal Strength & Speed Tracking: The bot will measure theoretical Wi-Fi signal strength and internet speed.**

---

## 🚀 Project Status: Ongoing  
✅ **Implemented**:  
- Remote control via Firebase RTDB.  
- Real-time video streaming using WebSockets.  

🔜 **Upcoming Features**:  
- Custom WebSocket server using JavaScript.  
- LED integration for low-light conditions.  
- Real-world Wi-Fi speed tracking.  
- Unified web app for monitoring and control.  

---

Stay tuned for updates! 🚗💨  
