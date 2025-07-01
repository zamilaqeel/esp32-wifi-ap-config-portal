#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* apSSID = "test_config"; // Access Point SSID
const char* apPass = "12345678"; // Access Point Password 

WebServer server(80); // Create a web server on port 80

String ssid = ""; // Wi-Fi SSID
String password = ""; // Wi-Fi Password

const int ButtonPin = 0; // GPIO pin for the boot button
bool apMode = false; // Flag to indicate if in Access Point mode

void handleRoot() {
  String html = R"rawliteral( 
    <!DOCTYPE html>
    <html>
    <body>
      <h2> Wi-Fi Configuration </h2>
      <form action="/submit">
        SSID:<br>
        <input type="text" name="ssid"><br>
        Password:<br>
        <input type="password" name="password"><br><br>
        <input type="submit" value="Submit">
      </form>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
} // Handle the root URL and serve the HTML form

void handleSubmit() {
  ssid = server.arg("ssid"); // Get the SSID from the form
  password = server.arg("password"); // Get the password from the form

  Serial.println("Received Wi-Fi credentials:");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);

  WiFi.softAPdisconnect(true); // Disconnect from the Access Point if in AP mode
  apMode = false;
  server.stop();

  WiFi.begin(ssid.c_str(), password.c_str()); // Connect to the Wi-Fi network with the provided credentials
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nWiFi connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed!");
  }
}

void enterAccessPointMode() {
  Serial.println("Entering Access Point Mode...");
  apMode = true;

  WiFi.softAP(apSSID, apPass);
  Serial.println("Access Point started.");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot); // Handle the root URL and serve the HTML form
  server.on("/submit", handleSubmit); // Handle the form submission

  server.begin();
}

void setup() {
  pinMode(ButtonPin, INPUT_PULLUP);
  Serial.begin(9600);

  if (ssid.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nWiFi connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nWi-Fi connection failed!");
    }
  } else {
    Serial.println("No SSID configured. Press the button to enter AP mode.");
  }
}

void loop() {
  if (digitalRead(ButtonPin) == LOW) {
    delay(500);
    if (digitalRead(ButtonPin) == LOW && !apMode) { // Check if the button is pressed and not already in AP mode
      enterAccessPointMode();
    }
  }

  if (apMode) {
    server.handleClient(); // Handle incoming client requests in Access Point mode
  }
}
