/**
  * Based on
  * https://www.instructables.com/id/ESP0101S-RELAY-MODULE-TUTORIAL
 **/

#include <ESP8266WiFi.h>

#define RELAY 0 // relay connected to  GPIO0
#define MAX_RETRIES 100

const char* ssid = "replace_with_your_SSID"; // fill in here your router or wifi SSID
const char* password = "replace_with_your_password"; // fill in here your router or wifi password

WiFiServer server(80);
 
void setup() {
  Serial.begin(115200); // must be same baudrate with the Serial Monitor
 
  pinMode(RELAY, OUTPUT);

  // Turn off relay by default
  digitalWrite(RELAY, HIGH);
 
  // Connect to WiFi network
  delay(3000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.println("");
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("New client");

  // Avoid infinite loop when requesting from mobile browser
  int retries = 0;
  while (!client.available()) {
    Serial.print("no data from client ");
    Serial.print("(retry ");
    Serial.print(retries);
    Serial.println(")");
    if (retries >= MAX_RETRIES) {
      return;
    }
    retries++;
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  int value = digitalRead(RELAY);
  if (request.indexOf("/turn=on") != -1) {
    Serial.println("turn=on");
    digitalWrite(RELAY, LOW);
    value = LOW;
  } else if (request.indexOf("/turn=off") != -1) {
    Serial.println("turn=off");
    digitalWrite(RELAY, HIGH);
    value = HIGH;
  }
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  this is a must
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP8266 RELAY Control</title></head>");
  client.print("Relay is now: ");
 
  if (value == HIGH) {
    client.print("OFF");
    client.println("<br><br>");

    client.println("<form action=\"/turn=on\">");
    client.println("  <input type=\"submit\" value=\"TURN ON\"/>");
    client.println("<form>");
  } else {
    client.print("ON");
    client.println("<br><br>");

    client.println("<form action=\"/turn=off\">");
    client.println("  <input type=\"submit\" value=\"TURN OFF\"/>");
    client.println("<form>");
  }
  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}
