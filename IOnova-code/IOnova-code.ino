// Define your modem type
#define TINY_GSM_MODEM_M95

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

// Replace with your SIM card details
const char apn[]  = "internet";  // APN for your SIM card
const char user[] = "";          // Usually blank
const char pass[] = "";          // Usually blank

// Set serial ports
#define SerialMon Serial
SoftwareSerial SerialAT(7, 8); // RX, TX for GSM modem

// Create TinyGSM modem object
TinyGsm modem(SerialAT);

void setup() {
  // Start serial monitor
  SerialMon.begin(115200);
  delay(10);
  SerialMon.println("=== Quectel M95 GSM Test ===");

  // Start GSM serial
  SerialAT.begin(9600);
  delay(3000);

  // Restart the modem
  SerialMon.println("Initializing modem...");
  modem.restart();

  // Get modem info
  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Connect to network
  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" connected!");

  // Optional: check signal quality
  int csq = modem.getSignalQuality();
  SerialMon.print("Signal quality: ");
  SerialMon.println(csq);

  // Connect to GPRS
  SerialMon.print("Connecting to GPRS...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    return;
  }
  SerialMon.println(" connected!");

  SerialMon.println("Modem is ready.");
}

void loop() {
  // Nothing to do here
}
