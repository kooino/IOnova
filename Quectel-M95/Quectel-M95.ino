#define TINY_GSM_MODEM_M95

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

const char apn[]  = "onomondo";  // SIM card APN
const char user[] = "";
const char pass[] = "";

// Your server info
const char server[] = "als.best";
const int  port     = 8085;
const char token[]  = "A2_TEST_TOKEN";  // <-- Replace with your actual token

#define SerialMon Serial
SoftwareSerial SerialAT(0, 1); // RX, TX

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup() {
  SerialMon.begin(9600);
  delay(10);
  SerialMon.println("=== ThingsBoard Telemetry Test ===");

  SerialAT.begin(9600);
  delay(2000);

  SerialMon.println("Starting modem...");
  if (!modem.restart()) {
    SerialMon.println("Modem restart failed!");
    return;
  }
  SerialMon.println("Modem ready.");

  SerialMon.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println("Network failed");
    return;
  }
  SerialMon.println("Network connected.");

  SerialMon.println("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println("GPRS failed");
    return;
  }
  SerialMon.println("GPRS connected.");

  SerialMon.println("Connecting to ThingsBoard...");
  if (!client.connect(server, port)) {
    SerialMon.println("Connection to server failed");
    return;
  }
  SerialMon.println("Connected to server.");

  // Build the HTTP POST
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

  SerialMon.println("Telemetry sent. Reading response...");

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
