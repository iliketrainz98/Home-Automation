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
    
    fadeNumber = fadeNumberAsString.toInt();
    analogueWrite(2, fadeNumber * 10.24);
    
    client.flush();

    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
    s += fadeNumberAsString;
    s += "</html>\n";

    // Send the response to the client
    client.print(s);
    delay(1);
    Serial.println("Client disonnected");
  }
  else
  {
    Serial.println("invalid request");
    client.flush();
    String returnString = "HTTP/1.1 400 BAD REQUEST"
    client.print(s)
    client.stop()
    return;
  }
  
  // Match the request
//   int val;
//   if (req.indexOf("/gpio/0") != -1)
//     val = 0;
//   else if (req.indexOf("/gpio/1") != -1)
//     val = 1;
//   else {
//     Serial.println("invalid request");
//     client.stop();
//     return;
//   }

  // Set GPIO2 according to the request
  
  
  

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

