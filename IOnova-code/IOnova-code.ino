#define TINY_GSM_MODEM_M95

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

const char apn[]  = "onomondo";   // Replace with your APN
const char user[] = "";
const char pass[] = "";

// Your server details
const char server[] = "als.best";  // Replace with your domain or IP
const int  port     = 8085;                 // Use 443 for HTTPS (not recommended with M95)
const char resource[] = "A1_TEST_TOKEN"; // Example: "/api/test"

// Serial interfaces
#define SerialMon Serial
SoftwareSerial SerialAT(7, 8); // RX, TX

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

void setup() {
  SerialMon.begin(115200);
  delay(10);

  SerialMon.println("=== Quectel M95 -> Custom Server Test ===");

  // Start modem communication
  SerialAT.begin(9600);
  delay(3000);

  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" connected");

  SerialMon.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" connected");

  SerialMon.print("Connecting to server: ");
  SerialMon.println(server);

  if (!client.connect(server, port)) {
    SerialMon.println("Connection failed");
    return;
  }

  SerialMon.println("Connected to server");

  // Create basic HTTP GET request
  client.print(String("GET ") + resource + " HTTP/1.1\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  // Read and print the response
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      SerialMon.print(c);
    }
  }

  client.stop();
  SerialMon.println("\nDisconnected");
}

void loop() {
  // Nothing to do here
}
