#define TINY_GSM_MODEM_SIM7600

#include <TinyGsmClient.h>
#include <HardwareSerial.h>

// SIM & network
const char apn[]  = "internet";  // Replace with your SIM's APN
const char user[] = "";
const char pass[] = "";

// ThingsBoard server info
const char server[] = "als.best";
const int port = 8080;
const char token[] = "A1_TEST_TOKEN";  // Replace with your token

#define SerialMon Serial
HardwareSerial SerialAT(2);  // Use UART2 (GPIO16/17 on ESP32)

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup() {
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("=== ESP32 + A7670E LTE Telemetry ===");

  SerialAT.begin(115200, SERIAL_8N1, 16, 17);  // RX, TX
  delay(3000);

  // Initialize modem (no restart needed)
  SerialMon.println("Initializing modem...");
  if (!modem.init()) {
    SerialMon.println("Modem init failed");
    return;
  }

  SerialMon.print("Modem Info: ");
  SerialMon.println(modem.getModemInfo());

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println("Network failed");
    return;
  }
  SerialMon.println("Network OK");

  SerialMon.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println("GPRS failed");
    return;
  }
  SerialMon.println("GPRS connected");

  SerialMon.print("Connecting to ThingsBoard...");
  if (!client.connect(server, port)) {
    SerialMon.println("Server connection failed");
    return;
  }
  SerialMon.println("Connected!");

  // Send telemetry
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

  SerialMon.println("Telemetry sent, reading response...");

  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      SerialMon.write(c);
    }
  }

  client.stop();
  SerialMon.println("\nDisconnected.");
}

void loop() {
  // Do nothing or repeat every few minutes
}
