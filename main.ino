#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>  // Include ArduinoJson library

// Wi-Fi credentials
const char* ssid = "Tanish";  // Replace with your WiFi SSID
const char* password = "02468970";  // Replace with your WiFi Password

// Firebase setup
const char* firebaseHost = "digital-notice-board-6ee3e-default-rtdb.firebaseio.com";
const char* firebaseAuth = "MSxVRpvOBSEwSMkCJPbdvmaJAVuslhEiijljxypA";  // Replace with your Firebase Database Secret

// Define Firebase path
String firebasePath = "/notice_board";

// Setup WiFi client and HTTP client
WiFiClientSecure wifi;
HttpClient client = HttpClient(wifi, firebaseHost, 443);

String lastMessage = "";  // To track the last received message

void setup() {
  Serial.begin(115200);
  wifi.setInsecure();  // Skip SSL certificate validation

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi!");

  // Fetch initial message from Firebase
  fetchDataFromFirebase();
}

void fetchDataFromFirebase() {
  Serial.println("Fetching data from Firebase...");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
  }

  String url = firebasePath + "/message.json?auth=" + firebaseAuth;
  client.get(url);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200) {
    Serial.println("Data fetched successfully!");
    Serial.println("Response: " + response);

    // Parse JSON response
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, response);
    if (!error) {
      String newMessage = doc["message"].as<String>();

      // Check if message is new
      if (newMessage != lastMessage) {
        lastMessage = newMessage;
        Serial.println("New message: " + newMessage);

        // You can later send this message to an LCD display
      }
    
  } else {
    Serial.println("Failed to fetch data. Status code: " + String(statusCode));
    delay(2000);
    fetchDataFromFirebase();  // Retry
  }
}

void loop() {
  fetchDataFromFirebase();
  delay(5000);  // Check for updates every 5 seconds
}
