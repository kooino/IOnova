#include <GSM.h>

// SIM settings
#define PINNUMBER ""  // Leave empty if your SIM has no PIN
const char apn[]  = "onomondo";  // Replace with your SIM's APN
const char user[] = "";
const char pass[] = "";

// ThingsBoard server
const char server[] = "als.best";
const int port = 8085;
const char token[] = "A1_TEST_TOKEN";  // Replace with your device token

// GSM/GPRS objects
GSM gsmAccess;
GPRS gprs;
GSMClient client;

unsigned long lastSend = 0;
const unsigned long interval = 1000; // 5 seconds

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing GSM...");

  // Connect to GSM and GPRS
  while (gsmAccess.begin(PINNUMBER) != GSM_READY) {
    Serial.println("GSM not ready. Retrying...");
    delay(1000);
  }

  while (gprs.attachGPRS(apn, user, pass) != GPRS_READY) {
    Serial.println("GPRS not ready. Retrying...");
    delay(1000);
  }

  Serial.println("GSM and GPRS ready.");
}

void loop() {
  if (millis() - lastSend >= interval) {
    sendTelemetry();
    lastSend = millis();
  }
}

void sendTelemetry() {
  String payload = "{\"temperature\":25}";  // Replace with real sensor value if needed
  String path = "/api/v1/" + String(token) + "/telemetry";

  Serial.println("Connecting to ThingsBoard...");

  if (client.connect(server, port)) {
    Serial.println("Connected to server.");

    // Send HTTP POST
    client.println("POST " + path + " HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println("Connection: close");
    client.println();
    client.println(payload);

    // Read response
    Serial.println("Response:");
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    client.stop();
    Serial.println("\nTelemetry sent.\n");

  } else {
    Serial.println("Connection failed.");
  }
}
