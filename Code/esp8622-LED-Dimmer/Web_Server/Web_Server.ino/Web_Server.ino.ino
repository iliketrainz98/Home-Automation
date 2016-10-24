/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/100 will set the GPIO2 high
 *  This version of the server allows pwm input
 */

#include <ESP8266WiFi.h>

const char* ssid = "your-ssid";
const char* password = "your-wifi-password";

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
     * -1 -> undefined
     * 1  -> digital
     * 2  -> pwm
     */
    int controlType = -1;
    int lengthOfTypeString = 0;
    
    if(req.indexOf("/digital/") != 1)
    {
      /*
       * Request looks like:
       *   /gpio/digital/50
       */
      controlType = 1;
      lengthOfTypeString = 8;
    }
    else if(req.indexOf("/pwm/") != 1)
    {
      /*
       * Request looks like:
       *   /gpio/pwm/50
       */
      controlType = 2;
      lengthOfTypeString = 4;
    }
    else
    {
      /*
       * Default to controlType=pwm to allow a url that looks like:
       *   /gpio/50
       * instead of the new format that looks like:
       *   /gpio/pwm/50
       *
       * Here purely for backwards-compatability.
       */
      controlType = 2;
      lengthOfTypeString = 0;
    }
    
    
    /*
     * 6 = length of /gpio/
     */
    int startOfFadeString = startOfGpioString + 6 + lengthOfTypeString;
    
    int fadeNumberSearchLocation = startOfFadeString;
    String fadeNumberAsString = "";
    while(req.charAt(fadeNumberSearchLocation) != ' ')
    {
      fadeNumberAsString.concat(req.charAt(fadeNumberSearchLocation));
      fadeNumberSearchLocation++;
    }
    
    int fadeNumber = fadeNumberAsString.toInt();
    
    if(controlType == 1)
    {
      digitalWrite(2, fadeNumber);
    }
    else if(controlType == 2)
    {
      analogWrite(2, fadeNumber * 10.24);
    }
    else
    {
      /*
       * We should never arrive here, but handle it anyway.
       */
    }
    
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

