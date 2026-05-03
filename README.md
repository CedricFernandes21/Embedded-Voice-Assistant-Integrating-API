# DeepSeek ESP32 Voice Assistant

A real-time, low-latency AI voice assistant built with dual ESP32 microcontrollers, a custom React.js web server, and the DeepSeek AI API. 

## 🚀 Overview
Traditional voice assistants often suffer from high latency and heavy cloud dependency[cite: 2]. This project overcomes these limitations by implementing a fast-response architecture optimized for embedded environments[cite: 2]. By physically splitting the input (listening) and output (speaking) tasks across two microcontrollers and routing requests through a custom web server, the system achieves a highly optimized, portable, and cost-efficient hardware interface[cite: 2]. 

## ✨ Key Features
* **Dual-Core Architecture:** Utilizes two ESP32 microcontrollers communicating via bidirectional UART to separate voice processing from audio output generation[cite: 2].
* **Ultra-Low Latency:** Powered by the DeepSeek API, achieving an average response latency of just 3.47 seconds[cite: 2].
* **Custom Backend:** Features a custom-hosted React.js web application that acts as an intermediary to manage API requests and track performance metrics[cite: 2].
* **Hands-Free Activation:** An onboard IR proximity sensor detects user presence to trigger the listening mode automatically[cite: 2].
* **Portable Power:** Operates on a rechargeable LiPo battery regulated by TP4056 and HT7333 modules for stable 3.3V power distribution[cite: 2].

## 🛠️ Hardware Components
* **Microcontrollers:** 2x DOIT ESP32 DEV Boards[cite: 2].
* **Microphone:** INMP441 MEMS Digital Microphone (I2S Interface)[cite: 2].
* **Audio Output:** MAX98357A I2S Class-D Amplifier and an 8-ohm 2W Speaker[cite: 2].
* **Sensors & Inputs:** IR Proximity Sensor and a manual Push Button[cite: 2].
* **Power Management:** Rechargeable Battery, TP4056 Charging Module, and HT7333/HT7833 Voltage Regulator[cite: 2].

## 💻 System Architecture 
The system is divided into four main operational blocks[cite: 2]:

1. **Voice Input and Sensing:** The INMP441 captures voice input digitally via I2S, while the IR sensor alerts the system to user presence via a GPIO signal[cite: 2].
2. **Processing and Communication:** `ESP32 U1` handles the voice signal processing and Wi-Fi communication to the custom cloud server[cite: 2]. Once the API processes the text, `ESP32 U1` transmits the data to `ESP32 U2` via UART[cite: 2].
3. **Audio Output:** `ESP32 U2` receives the processed data, converts it to an I2S digital audio format, and sends it to the MAX98357A DAC to drive the speaker[cite: 2].
4. **Power Distribution:** A centralized power unit steps down the battery voltage to a stable 3.3V to safely power both microcontrollers and all peripherals[cite: 2].

## 📊 Performance & API Benchmarking
During development, a custom testing framework was built in Visual Studio to compare various LLM APIs[cite: 2]. DeepSeek was selected as the final model due to its superior speed[cite: 2].

| API Model | Latency (Seconds) |
| :--- | :--- |
| **DeepSeek API** | **3.473**[cite: 2] |
| GPT API | 5.392[cite: 2] |
| Hugging Face API | 11.372[cite: 2] |
| LLaMA API | 13.270[cite: 2] |
