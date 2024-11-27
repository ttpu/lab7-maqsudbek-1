#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "xxxxxx";           // Your Wi-Fi SSID
const char* password = "xxxxxxx";   // Your Wi-Fi Password

const char* firmwareUrl = "https://xxxxxxxxxxxx.bin";  // URL to the .bin file
const float currentVersion = 1.0;    // Current firmware version
const char* versionUrl = "https://xxxxxxxxx";  // URL to version text file


void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  // Check for updates on startup
  checkForUpdates();
}

void loop() {
  // Your regular code here
  // For example, periodically check for updates
  static unsigned long lastCheck = 0;
  const unsigned long interval = 10000;  // Check every hour (3600000 milliseconds)

  if (millis() - lastCheck > interval) {
    lastCheck = millis();
    checkForUpdates();
  }

}

void checkForUpdates() {
  Serial.println("Checking for firmware updates...");

  HTTPClient http;

  // Fetch the latest version number from the server
  http.begin(versionUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String newVersionStr = http.getString();
    newVersionStr.trim();  // Remove any whitespace or newline characters
    float newVersion = newVersionStr.toFloat();

    Serial.print("Current firmware version: ");
    Serial.println(currentVersion, 2);
    Serial.print("Available firmware version: ");
    Serial.println(newVersion, 2);

    if (newVersion > currentVersion) {
      Serial.println("New firmware available. Starting update...");
      http.end();  // Close the connection before proceeding
      downloadAndUpdate();
    } else {
      Serial.println("Already on the latest version.");
    }
  } else {
    Serial.print("Failed to check for updates. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}

void downloadAndUpdate() {
  HTTPClient http;

  // Begin downloading the new firmware
  http.begin(firmwareUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();

    if (contentLength > 0) {
      bool canBegin = Update.begin(contentLength);

      if (canBegin) {
        Serial.println("Beginning firmware update...");

        WiFiClient* client = http.getStreamPtr();
        size_t written = Update.writeStream(*client);

        if (written == contentLength) {
          Serial.println("Firmware update successfully written.");
        } else {
          Serial.print("Only ");
          Serial.print(written);
          Serial.print(" out of ");
          Serial.print(contentLength);
          Serial.println(" bytes were written. Update failed.");
          http.end();
          return;
        }

        if (Update.end()) {
          Serial.println("Update finished successfully.");
          if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting...");
            ESP.restart();
          } else {
            Serial.println("Update did not complete. Something went wrong.");
          }
        } else {
          Serial.print("Error Occurred. Error #: ");
          Serial.println(Update.getError());
        }
      } else {
        Serial.println("Not enough space to begin OTA update.");
      }
    } else {
      Serial.println("Content length is not valid.");
    }
  } else {
    Serial.print("Failed to download firmware. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}
