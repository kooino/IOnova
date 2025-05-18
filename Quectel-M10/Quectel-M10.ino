#define TINY_GSM_MODEM_SIM900  // M10 is AT-compatible with SIM900

#include <TinyGsmClient.h>

// SIM credentials
const char apn[]  = "onomondo";  // Replace with your SIM's APN
const char user[] = "";
const char pass[] = "";

// ThingsBoard credentials
const char server[] = "als.best";        // Your ThingsBoard server
const int  port     = 8085;              // HTTP port
const char token[]  = "A1_TEST_TOKEN";   // Replace with your device access token

#define SerialMon Serial     // USB Serial Monitor
#define SerialAT  Serial1    // M10 modem on Serial1 (pins 18/19)

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup() {
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("=== Arduino Due + M10 + ThingsBoard ===");

  // Start communication with modem
  SerialAT.begin(115200);  // Try 115200 if 9600 doesn't work
  delay(3000);

  SerialMon.println("Restarting modem...");
  if (!modem.restart()) {
    SerialMon.println("Modem restart failed!");
    return;
  }

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" failed");
    return;
  }
  SerialMon.println(" connected");

  SerialMon.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" failed");
    return;
  }
  SerialMon.println(" connected");

  SerialMon.print("Connecting to ThingsBoard...");
  if (!client.connect(server, port)) {
    SerialMon.println(" connection failed");
    return;
  }
  SerialMon.println(" connected");

  // Prepare HTTP POST
  String url = String("/api/v1/") + token + "/telemetry";
  String payload = "{\"temperature\":25}";

  client.print(String("POST ") + url + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: ");
  client.print(payload.length());
  client.print("\r\n\r\n");
  client.print(payload);

  SerialMon.println("Data sent. Waiting for response...");

  // Read response
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      SerialMon.print(c);
    }
  }

  client.stop();
  SerialMon.println("\nDisconnected.");
}

void loop() {
  // Nothing here for now
}
