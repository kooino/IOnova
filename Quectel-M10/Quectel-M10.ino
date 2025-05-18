#define TINY_GSM_MODEM_SIM900  // M10 is SIM900-compatible

#include <TinyGsmClient.h>

// SIM card credentials
const char apn[]  = "onomondo";  // Replace with your actual APN
const char user[] = "";
const char pass[] = "";

// ThingsBoard server settings
const char server[] = "als.best";        // Your server
const int  port     = 8085;              // Port (HTTP)
const char token[]  = "A1_TEST_TOKEN";   // Replace with your access token

#define SerialMon Serial
#define SerialAT Serial1    // M10 on pins 18 (TX) and 19 (RX)

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup() {
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("=== M10 Manual Init Test ===");

  // Start serial for modem
  SerialAT.begin(115200);  // Try 115200 if needed
  delay(3000);

  SerialMon.println("Initializing modem (no restart)...");
  if (!modem.init()) {
    SerialMon.println("Modem init failed");
    return;
  }

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print("Connecting to server...");
  if (!client.connect(server, port)) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" OK");

  // Prepare HTTP POST request
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

  SerialMon.println("Data sent. Reading response...");

  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      SerialMon.print(c);
    }
  }

  client.stop();
  SerialMon.println("\nDone.");
}

void loop() {
  // nothing
}
