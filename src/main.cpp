// Include necessary libraries for Arduino framework
#include <Arduino.h>                  // Core Arduino functions (Serial, delay, etc.)
#include <WiFi.h>                     // WiFi functionality for ESP32
#include <WiFiClient.h>               // Required for WebServer (client handling)
#include <WebServer.h>                // Creates a web server on port 80
#include <ESPmDNS.h>                  // Allows access via http://esp32.local
#include <DHT.h>                      // DHT sensor library by Adafruit

// WiFi credentials - CHANGE THESE TO YOUR NETWORK!
const char* ssid = "First floor";     // Your WiFi SSID (network name)
const char* password = "12340809";    // Your WiFi password

// Create web server object on port 80 (standard HTTP port)
WebServer server(80);

// Initialize DHT11 sensor on GPIO26, type DHT11
DHT dht(26, DHT11);                   // Pin 26, DHT11 model

// Function prototypes (tell compiler these functions exist later)
float readDHTTemperature();           // Returns temperature in Celsius
float readDHTHumidity();              // Returns humidity in percentage

// Root webpage handler - runs when someone visits the ESP32 IP
void handleRoot() {
  char msg[1500];                     // Buffer to store full HTML page (1500 bytes)

  // Read current temperature and humidity
  float temperature = readDHTTemperature();  // Get temperature value
  float humidity = readDHTHumidity();        // Get humidity value

  // Build HTML page with live sensor data using snprintf
  snprintf(msg, 1500,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
      html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
      h2 { font-size: 3.0rem; color: #333;}\
      p { font-size: 3.0rem; margin: 20px;}\
      .units { font-size: 1.5rem; }\
      .dht-labels { font-size: 1.8rem; vertical-align:middle; padding-bottom: 15px;}\
      body { background: linear-gradient(to bottom, #87CEEB, #98D8C8); padding: 40px; }\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT11 Monitor</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span><br>\
        <span>%.2f</span>\
        <sup class='units'>°C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span><br>\
        <span>%.2f</span>\
        <sup class='units'>%%</sup>\
      </p>\
      <p><small>Refreshes every 4 seconds</small></p>\
  </body>\
</html>",
           temperature,                // %.2f → insert temperature here
           humidity                    // %.2f → insert humidity here
          );

  // Send the complete HTML page to the browser
  server.send(200, "text/html", msg);  // 200 = OK, content type = HTML
}

// Setup function - runs once when ESP32 starts
void setup(void) {
  Serial.begin(115200);               // Start serial monitor at 115200 baud
  delay(100);                         // Small delay for serial stability
  dht.begin();                        // Initialize DHT sensor (takes time to warm up)

  Serial.println();                   // Print blank line
  Serial.println("Connecting to WiFi...");

  WiFi.mode(WIFI_STA);                // Set ESP32 as a station (client)
  WiFi.begin(ssid, password);         // Connect to WiFi network

  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                       // Wait 500ms
    Serial.print(".");                // Print dot every 500ms
  }

  // Connected successfully!
  Serial.println();                   
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());     // Show the IP (e.g., 192.168.1.100)

  // Start mDNS - allows access via http://esp32.local
  if (MDNS.begin("esp32")) {
    Serial.println("mDNS started: http://esp32.local");
  }

  // Define what happens when someone visits root URL "/"
  server.on("/", handleRoot);         

  // Start the web server
  server.begin();                     
  Serial.println("HTTP server started");
  Serial.println("Open browser and go to IP address above");
}

// Main loop - runs forever
void loop(void) {
  server.handleClient();              // Listen for incoming clients (browsers)
  delay(2);                           // Small delay to allow CPU to breathe
}

// Function to read temperature from DHT11
float readDHTTemperature() {
  float t = dht.readTemperature();    // Read temperature in Celsius
  if (isnan(t)) {                     // isnan = is Not a Number (failed reading)
    Serial.println("Failed to read temperature from DHT sensor!");
    return 0.0;                       // Return 0 if failed
  }
  else {
    Serial.printf("Temperature: %.2f °C\n", t);  // Debug print
    return t;                         // Return valid temperature
  }
}

// Function to read humidity from DHT11
float readDHTHumidity() {
  float h = dht.readHumidity();       // Read humidity percentage
  if (isnan(h)) {
    Serial.println("Failed to read humidity from DHT sensor!");
    return 0.0;                       // Return 0 if failed
  }
  else {
    Serial.printf("Humidity: %.2f %%\n", h);     // Debug print
    return h;                         // Return valid humidity
  }
}