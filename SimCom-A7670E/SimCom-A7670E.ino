#include <GSM.h>

// PIN Number for the SIM card
#define PINNUMBER ""

// APN settings (change if needed)
const char apn[]  = "onomondo"; // Replace with your provider's APN
const char user[] = "";         // Usually blank
const char pass[] = "";         // Usually blank

// Server details
const char server[] = "als.best"; // Public test server
const int  port     = 8085;
const char path[]   = "/";

// Initialize the GSM and GPRS classes
GSM gsmAccess;
GPRS gprs;
GSMClient client;

void setup() {
  // Start serial communication
  Serial.begin(9600);
  Serial.println("Starting GSM connection...");

  // Connect to the network
  bool connected = false;
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      if (gprs.attachGPRS(apn, user, pass) == GPRS_READY) {
        connected = true;
      } else {
        Serial.println("GPRS attach failed. Retrying...");
      }
    } else {
      Serial.println("GSM init failed. Retrying...");
    }
    delay(2000);
  }

  Serial.println("GSM connected.");

  // Connect to server
  Serial.print("Connecting to server...");
  if (client.connect(server, port)) {
    Serial.println("Connected!");

    // Send HTTP GET
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

  } else {
    Serial.println("Connection failed.");
  }
}

void loop() {
  // Read response from server
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println("\nDisconnected.");
    client.stop();
    while (true); // Stop program
  }
}
