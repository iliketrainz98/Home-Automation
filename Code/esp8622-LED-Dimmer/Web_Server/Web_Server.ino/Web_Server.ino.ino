/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

const char* ssid = "Gillfamily-home";
const char* password = "Jamie030199";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
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
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  int startOfGpioString = req.indexOf("/gpio/");
  if(startOfGpioString != -1)
  {
    /*
     * 6 = length of /gpio/
     */
    int startOfFadeString = startOfGpioString + 6;
    
    
    
    int fadeNumberSearchLocation = startOfFadeString;
    String fadeNumberAsString = "";
    while(req.charAt(fadeNumberSearchLocation) != ' ')
    {
      fadeNumberAsString.concat(req.charAt(fadeNumberSearchLocation));
      fadeNumberSearchLocation++;
    }
    
    int fadeNumber = fadeNumberAsString.toInt();
    analogWrite(2, fadeNumber * 10.24);
    
    client.flush();

    // Prepare the response
    String returnString = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
    returnString += fadeNumberAsString;
    returnString += "</html>\n";

    // Send the response to the client
    client.print(returnString);
    delay(1);
    Serial.println("Client disonnected");
    return;
  }
  else
  {
    Serial.println("invalid request");
    client.flush();
    // Send a 400 which means "The client fucked up - try again"
    String returnString = "HTTP/1.1 400 BAD REQUEST";
    client.print(returnString);
    client.stop();
    return;
  }
}

