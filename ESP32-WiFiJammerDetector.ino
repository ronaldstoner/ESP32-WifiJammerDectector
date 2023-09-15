//
// ESP32-WiFiJammerDetector
// By: Ron Stoner
// ron.stoner.com
//

#include <WiFi.h>
#include "TFT_eSPI.h" // Include the graphics library

#define SCAN_TIME 20 // Time in seconds to perform WiFi scan
#define COL1_WIDTH 160 // Adjust this value based on your screen requirements

#define BUTTON1_PIN 0 // PIN for button 1
#define BUTTON2_PIN 14 // PIN for button 2

TFT_eSPI tft = TFT_eSPI(); // Initialize TFT screen

int zeroNetworksCount = 0; // Counter for times no networks are found

// Function to get the color based on RSSI
uint16_t getColorBasedOnRSSI(int32_t rssi) {
  if (rssi >= -70) {
    return TFT_GREEN; // Green for closer signals
  } else if (rssi >= -80) {
    return TFT_YELLOW; // Yellow for medium signals
  } else {
    return TFT_RED; // Red for bad signals
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Input buttons for debug testing
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  // Initialize TFT screen
  tft.init();
  tft.setRotation(3); // Set the rotation
  tft.fillScreen(TFT_BLACK); // Fill the screen with black

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  int numNetworks = WiFi.scanComplete();

  if (numNetworks == WIFI_SCAN_RUNNING) {
    // WiFi scan is still running
    return;
  }

  if (numNetworks == WIFI_SCAN_FAILED) {
    // WiFi scan failed, restart it
    WiFi.scanNetworks(false, true);
    return;
  }

  // DEBUG for screen testing
  if (digitalRead(BUTTON1_PIN) == LOW) {
    numNetworks = 0;
    zeroNetworksCount = 5; 
  }    

  // If no networks are found, increment the counter
  if (numNetworks == 0) {
    zeroNetworksCount++;
    if (zeroNetworksCount > 3) {
      // If no networks are found more than 3 times, make the screen red and display a warning
      tft.fillScreen(TFT_RED);
      tft.setTextColor(TFT_WHITE, TFT_RED); // Set text color to white and background to black
      tft.setCursor(0, 40); // Set the cursor
      tft.setTextSize(4);
      tft.println("NO WIFI FOUND");
      tft.setTextSize(2);
      tft.println("");
      tft.println("  There is no wifi signal");
      tft.println("or a jammer may be present");
      tft.println("");
      tft.println("      Times failed: " + String(zeroNetworksCount));
      delay(1000 * SCAN_TIME);
      tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color to white and background to black
    }
    return;
  }

  // Reset the counter if networks are found
  zeroNetworksCount = 0;

  //
  // Screen 1
  //
  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Grab and print the temperature
  float tempC = temperatureRead(); // Read the temperature sensor
  char tempCString[6]; // Buffer big enough for 5 chars (including sign, one decimal place and null character at end)
  dtostrf(tempC, 4, 1, tempCString); // Convert to string with one decimal place

  tft.setTextSize(1);
  tft.setCursor(tft.width() - 7 * tft.textWidth("0"), 5);
  tft.println(String(tempCString) + " C");

  // Print the number of networks found
  tft.setTextSize(2);
  //tft.drawRect(110, 0, 100, 30, TFT_WHITE); // Draw a rectangle for the number of networks
  tft.setCursor(0, 5);
  tft.println(String(numNetworks) + " Networks Found");

  // Print each network
  tft.setTextSize(1);
  tft.setCursor(0, 45);

  for (int i = 0; i < numNetworks; i++) {
    String ssid;
    uint8_t encryptionType;
    int32_t rssi;
    uint8_t* bssid;
    int32_t channel;

    if(WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel)) {
      // If no SSID, set it to "unknown"
      if(ssid == "") {
        ssid = "unknown";
      }

      // Cut off SSID if it's too long for column
      if (ssid.length() > COL1_WIDTH) {
        ssid = ssid.substring(0, COL1_WIDTH);
      }
      
      // Print "ssid        rssi db" and color the text based on the signal strength
      uint16_t color = getColorBasedOnRSSI(rssi);
      tft.setTextColor(color, TFT_BLACK);
      
      tft.setCursor(0, 40 + i * 10);
      tft.print(ssid);
      tft.setCursor(COL1_WIDTH, 40 + i * 10);
      tft.println(String(rssi) + " dB");

      String encryptionTypeStr;
      switch(encryptionType) {
          case WIFI_AUTH_OPEN: // 0
              encryptionTypeStr = "Open";
              break;
          case WIFI_AUTH_WEP: // 1
              encryptionTypeStr = "WEP";
              break;
          case WIFI_AUTH_WPA_PSK: // 2
              encryptionTypeStr = "WPA_PSK";
              break;
          case WIFI_AUTH_WPA2_PSK: // 3
              encryptionTypeStr = "WPA2_PSK";
              break;
          case WIFI_AUTH_WPA_WPA2_PSK: // 4
              encryptionTypeStr = "WPA_WPA2_PSK";
              break;
          case WIFI_AUTH_WPA2_ENTERPRISE: // 5
              encryptionTypeStr = "WPA2_ENTERPRISE";
              break;
          default:
              encryptionTypeStr = "UNKNOWN";
      }

      tft.setCursor(COL1_WIDTH + 60, 40 + i * 10);
      tft.println(String(encryptionTypeStr));
      tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color back to white
    }
  }

  delay((1000 * SCAN_TIME)/2); // Delay for Screen 1

  //
  // Screen 2
  //
  // Channel graph parameters
  #define SPECTRUM_Y 110 // Y Position of the spectrum
  #define SPECTRUM_HEIGHT 40 // Height of the spectrum
  #define SPECTRUM_NUM_CHANNELS 14 // Total number of WiFi channels
  #define CHANNEL_WIDTH (tft.width() / SPECTRUM_NUM_CHANNELS) // Width allocated for each channel
 
  int channelCounts[SPECTRUM_NUM_CHANNELS] = {0};

  tft.fillScreen(TFT_BLACK); // Clear the screen

  // Draw the channel numbers
  for (int i = 0; i < SPECTRUM_NUM_CHANNELS; i++) {
    tft.setCursor(i * CHANNEL_WIDTH + CHANNEL_WIDTH / 2 - 3, SPECTRUM_Y + SPECTRUM_HEIGHT);
    tft.print(i + 1); // Channels start at 1
  }

  // Draw line above channel numbers
  tft.drawLine(0, SPECTRUM_Y + SPECTRUM_HEIGHT - 8, tft.width(), SPECTRUM_Y + SPECTRUM_HEIGHT - 8, TFT_WHITE);

  for (int i = 0; i < numNetworks; i++) {
    String ssid;
    uint8_t encryptionType;
    int32_t rssi;
    uint8_t* bssid;
    int32_t channel;

    if(WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel)) {
      // If no SSID, set it to "unknown"
      if(ssid == "") {
        ssid = "unknown";
      }

      // Draw a dot in the spectrum at the channel position
      int dotX = CHANNEL_WIDTH * (channel - 1) + CHANNEL_WIDTH / 2; // Channels start at 1
      int dotY = SPECTRUM_Y + SPECTRUM_HEIGHT / 2 - 20 * channelCounts[channel - 1]; // Raise the dot if the channel has been used before
      uint16_t color = getColorBasedOnRSSI(rssi);
      tft.fillCircle(dotX, dotY, 5, color);

      // Print the first 4 letters of the SSID above the dot
      String ssidFirst4 = ssid.substring(0, 4);
      tft.setCursor(dotX - CHANNEL_WIDTH / 2, dotY - 10);
      tft.print(ssidFirst4);
      channelCounts[channel - 1]++; // Increment the count for this channel
    }
  }

  delay((1000 * SCAN_TIME)/2);  // Delay for Screen 2

  // Restart WiFi scan
  WiFi.scanNetworks(false, true);
}
