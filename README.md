# ESP32-WifiJammerDectector
A simple ESP32 based WiFi detector that assists when jamming efforts may be attempted. 

Over time, signal jamming has increasingly become a prevalent tactic used in various scenarios, including criminal activities, governmental operations, and state-sponsored attacks.

Although this device primarily functions as a basic "WiFi Scanner," its underlying principle is based on the expectation of always detecting WiFi signals, Access Points (APs), or Service Set Identifiers (SSIDs) in populated areas. The absence of such signals in areas where they are typically present could potentially indicate active jamming. While this is not a foolproof method of detection, it can serve as an early warning system, alerting users to the possibility of WiFi signals being intentionally disrupted.

# Screenshots
## WiFi SSID Site Survey
<img src="https://github.com/ronaldstoner/ESP32-WifiJammerDectector/blob/main/img/main.jpg?raw=true" align="center" /> 

## Channel Layout
<img src="https://github.com/ronaldstoner/ESP32-WifiJammerDectector/blob/main/img/screen1.jpg?raw=true" align="center" /> 

## No Signal / Jammer May Be Present
<img src="https://github.com/ronaldstoner/ESP32-WifiJammerDectector/blob/main/img/screen2.jpg?raw=true" align="center" /> 

 	
# Install

1. Clone this repo
2. Flash with Arduino IDE

# Usage
1. Power on device and scanning will occur automatically
2. Press and hold **BUTTON_1** when the *Channel Layout* screen cycles to the next view to test the "**No Signal**" screen. 

# Note
The author makes no guarantees on the function or accuracy of this software. Due to the nature of radio signals different users may have different experiences. This code is only a proof of concept and should be integrated into a larger monitoring/alerting system.
