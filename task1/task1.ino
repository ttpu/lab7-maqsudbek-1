#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "xxxxxx";          // Replace with your Wi-Fi SSID
const char* password = "xxxxxx";  // Replace with your Wi-Fi Password

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  // Connect to your Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for the connection to establish
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Configure OTA
  ArduinoOTA.setHostname("esp32-ota");   // Optional: Set a custom hostname
  // ArduinoOTA.setPassword("admin");    // Optional: Set an OTA password

  // OTA Event Handlers (Optional but recommended for debugging)
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nUpdate Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress * 100) / total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error [%u]: ", error);
    if      (error == OTA_AUTH_ERROR)    Serial.println("Authentication Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connection Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");
  });

  // Start the OTA service
  ArduinoOTA.begin();

  Serial.println("Ready for OTA updates.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();
}
