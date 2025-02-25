#include "WiFiS3.h"
#include <IRremote.hpp>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define IR_SEND_PIN 4

void setup() {
  Serial.begin(9600);      // initialize serial communication
  IPAddress ip(192, 168, 0, 15);
  WiFi.config(ip);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status

  IrSender.begin(IR_SEND_PIN, DISABLE_LED_FEEDBACK, 0);
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<p style=\"font-size:30px;\"><a href=\"/EPS\">電源</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/ALL\">全灯</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/night\">常夜灯</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/eighty\">80%</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/sixty\">60%</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/thirty\">30%</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/bright\">明</a><br></p>");
            client.print("<p style=\"font-size:30px;\"><a href=\"/dark\">暗</a><br></p>");
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /EPS")) {
          IrSender.sendNEC(0x459A, 0x4, 0);
        }
        if (currentLine.endsWith("GET /ALL")) {
          IrSender.sendNEC(0x459A, 0x48, 0);
        }
        if (currentLine.endsWith("GET /night")) {
          IrSender.sendNEC(0x459A, 0x88, 0);
        }
        if (currentLine.endsWith("GET /eighty")) {
          IrSender.sendNEC(0x459A, 0x21, 0);
        }
        if (currentLine.endsWith("GET /sixty")) {
          IrSender.sendNEC(0x459A, 0x22, 0);
        }
        if (currentLine.endsWith("GET /thirty")) {
          IrSender.sendNEC(0x459A, 0x23, 0);
        }
        if (currentLine.endsWith("GET /bright")) {
          IrSender.sendNEC(0x459A, 0xC4, 0);
        }
        if (currentLine.endsWith("GET /dark")) {
          IrSender.sendNEC(0x459A, 0x44, 0);
        }
      }
      
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

