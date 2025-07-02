#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

const char* apSSID = "test_config"; // Access Point SSID
const char* apPass = "12345678"; // Access Point Password

WebServer server(80); // Web server on port 80

String ssid = ""; // Wi-Fi SSID
String password = ""; // Wi-Fi Password

const int ButtonPin = 0;
bool apMode = false;

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
    <title>Wi-Fi Connecting Portal</title>
    <style>
      html, body {
        margin: 0;
        padding: 0;
        text-align: center;
        background: #000;
        color: #fff;
        height: 100%;
      }
      body {
        min-height: 100vh;
        display: flex;
        flex-direction: column;
      }
      header, footer {
        background: #333;
        color: #fff;
        padding: 10px;
        text-align: center;
        width: 100vw;
        box-sizing: border-box;
      }
      h1 {
        color: #ff5733;
      }
      .content {
        flex: 1 0 auto;
      }
      footer {
        width: 100vw;
        box-sizing: border-box;
        flex-shrink: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row;
      }
      input, select, label, span, button {
        color: #fff;
        background: #222;
        border: 1px solid #444;
        border-radius: 4px;
        padding: 5px;
      }
      input[type="range"] {
        background: #444;
      }
      input[type="checkbox"] {
        accent-color: #ff5733;
      }
      button {
        background:rgb(46, 45, 45);
        color: #fff;
        border: none;
        padding: 10px 20px;
        margin-top: 10px;
        cursor: pointer;
      }
      button:hover {
        background:rgb(115, 114, 114);
      }

      input[type="submit"] {
      background-color: #ff5733;
      color: #fff;
      border: none;
      padding: 10px 20px;
      margin-top: 10px;
      cursor: pointer;
      border-radius: 4px;
      }

      input[type="submit"]:hover {
        background-color: #e14a20;
      }
    </style>
    </head>

    <body>
    <header>
      <br><h1>Connect to Wi-Fi</h1>
    </header>

    <div class="content">
      <h1>Configure Wi-Fi</h1>
      <form action="/submit" method="GET" onsubmit="return prepareForm();">
        SSID:<br>
        <input type="text" name="ssid" id="ssid"><br>
        Password:<br>
        <input type="password" name="password" id="password"><br>
        <button id="encryptToggle" type="button">Encryption Enabled</button>
        <input type="hidden" id="encryptionValue" name="encryption" value="false"><br><br>
        I agree to the terms and conditions <input type="checkbox" name="apMode" id="apModeCheckbox"><br>
        <input type="submit" value="Submit">
      </form>

      <script>
        const encryptBtn = document.getElementById("encryptToggle");
        const encryptValue = document.getElementById("encryptionValue");

        let encryptionEnabled = false;

        encryptBtn.addEventListener("click", function () {
          encryptionEnabled = !encryptionEnabled;
          encryptBtn.textContent = encryptionEnabled ? "Encryption Disabled" : "Encryption Enabled";
          encryptValue.value = encryptionEnabled ? "true" : "false";
        });

        function prepareForm() {
          return true;
        }
      </script>

      <br><br>

      <h3>Rate This Site</h3>
      <form>
        <label for="siteRating">How would you rate this site (out of 10)?</label><br><br>
        <input type="range" id="siteRating" name="siteRating" min="0" max="10" value="0">
        <span id="ratingValue">0</span>/10
        <br><br>
        <label for="improveSelect">Which one would you like to improve?</label><br><br>
        <select id="improveSelect" name="improveSelect">
          <option value="ssid">SSID input</option>
          <option value="password">Password input</option>
          <option value="encryption">Encryption</option>
          <option value="navigation">Portal navigation</option>
        </select>
      </form>
    </div>

    <script>
      const slider = document.getElementById('siteRating');
      const ratingValue = document.getElementById('ratingValue');

      slider.oninput = function () {
        ratingValue.textContent = this.value;
      };
    </script>

    <footer>
      <div style="display: flex; justify-content: center; align-items: center; gap: 10px; margin-top:8px;">
        <img src="/logo.png" alt="Logo" style="height:40px;">
        <span>&copy; 2025 Zamil Aqeel. All rights reserved.</span>
      </div>
    </footer>
    
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html); // Send HTML response
}

void handleSubmit() {
  ssid = server.arg("ssid"); // Get SSID from URL arguments
  password = server.arg("password"); // Get password from URL arguments

  Serial.println("Received URL arguments:"); // Print all URL arguments
  for (int i = 0; i < server.args(); i++) {
    Serial.printf("Arg %s = %s\n", server.argName(i).c_str(), server.arg(i).c_str()); // Print each argument name and value
  }

  if (ssid.length() == 0) {
    Serial.println("Error: SSID is missing");
    server.send(200, "text/plain", "SSID is required.");
    return;
  }

  Serial.println("Received Wi-Fi credentials:");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);

  WiFi.softAPdisconnect(true); // Disconnect from Access Point if connected
  apMode = false;
  server.stop();

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected");
    Serial.print("Connected to SSID: ");
    Serial.println(ssid);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed");
  }
}

void enterAccessPointMode() {
  Serial.println("Entering Access Point Mode...");
  apMode = true;

  WiFi.softAP(apSSID, apPass); // Start Access Point with SSID and Password
  Serial.println("Access Point started.");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot); // Handle root URL
  server.on("/submit", handleSubmit); // Handle form submission
  server.serveStatic("/logo.png", SPIFFS, "/logo.png"); // Serve static logo image from SPIFFS
  server.begin();
}

void setup() {
  pinMode(ButtonPin, INPUT_PULLUP);
  Serial.begin(9600);
  SPIFFS.begin(true);

  if (ssid.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWi-Fi connected");
      Serial.print("Connected to SSID: ");
      Serial.println(ssid);
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nWi-Fi connection failed");
    }
  } else {
    Serial.println("No SSID configured. Press the button to enter AP mode.");
  }
}

void loop() {
  if (digitalRead(ButtonPin) == LOW) {
    delay(500);
    if (digitalRead(ButtonPin) == LOW && !apMode) {
      enterAccessPointMode();
    }
  }

  if (apMode) {
    server.handleClient(); // Handle incoming client requests
  }
}
