#define W5100

#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include "Ethernet.h"
#include "SPI.h"


//Pin port numbers
int redLedPin = 9;
int greenLedPin = 8;
int buttonPin = 7;
int temperaturePotPin = A0;
int buttonState = 0;

//Temperature sensor values
int temperature;

//Old values
int oldTemperature = -1;
bool oldOn = false;


//Ethernet configurations
char server[] = "homecontrolserver.herokuapp.com";    // name address for HomeControl Server (using DNS)
SocketIOClient client;
EthernetClient httpclient;

//Socket.io configurations
extern String received;

//Heartbeat configurations - to keep the connection alive
unsigned long previousMillis = 0;
long interval = 5000;

void setup() {

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(temperaturePotPin, INPUT); //Optional 
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW); //Start with green led off
  Serial.begin(9600);


  //Start the ethernet connection
  Ethernet.init(10);
  if (Ethernet.begin(mac) == 0) {
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware || Ethernet.linkStatus() == LinkOFF)
      shutdown();
  }

  // Give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println((String)"Connecting to " + server);

  if (!client.connect(server)) shutdown();

  if (!httpclient.connect(server, 80))  shutdown();
  Serial.print("http connected to ");
  Serial.println(httpclient.remoteIP());
}

// the loop function runs over and over again forever
void loop() {

  if(!httpclient.connected()) {
    httpclient.stop();
    httpclient.connect(server, 80);
  }

  //Button status update
  buttonState = digitalRead(buttonPin);

  //If the button is pushed
  if (buttonState == HIGH) {
    changeStateButton();
    while (digitalRead(buttonPin) != LOW);
  }

  //Temeprature sensor update
  if (oldTemperature < temperature - 10 || oldTemperature > temperature + 10) {
    oldTemperature = temperature;
  }
  temperature = analogRead(temperaturePotPin); //Read and save analog value from potentiometer
  temperature = map(temperature, 0, 1023, 255, 0); //Map value 0-1023 to 255-0 

  //If oven is on
  if(digitalRead(greenLedPin) == HIGH) {
    if (temperature >= 180) {
      digitalWrite(redLedPin, LOW);
    }
    else {
      digitalWrite(redLedPin, HIGH);
    }
  }

  //If there's new messages from the websocket
  if (client.monitor()) {
    Serial.flush();
    
    if(received.length() != 0) {
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonArray& json = jsonBuffer.parseArray(received);
      Serial.println(received);
  
      if (!json.success()) Serial.println("parse failed!");
  
      //If it's a status update
      if (json[0] == "newState") {
        JsonObject& state = json[1]["state"];
        if(state.size() > 0) {
          String division = state["division"];
          String type = state["type"];
          String object = state["object"];
        
          //If the status update is regarding the oven represented by this arduino
          if (division == "kitchen" && type == "appliances" && object == "oven") {
            int newstate = atoi(state["state"]);
            Serial.println("Update State");
            changeStateUpdate(newstate);
          }
        }
      }
    }
  }
  
  //Handmade heartbeat
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    client.heartbeat(0);
  }

  delay(100);
  updateServerState();
  oldOn = digitalRead(greenLedPin) == HIGH? 1: 0;
}

void changeStateUpdate(int newState) {
  if (newState > 0) {
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
  }
}

void changeStateButton() {
  //If oven is turned off
  if (digitalRead(greenLedPin) == HIGH)  {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    //If oven is turned on
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
  }
}

void shutdown() {
  Serial.println("Shutting down");
  while (1) {
    delay(500);
  }
}


void updateServerState() {
  int ovenOn = digitalRead(greenLedPin) == HIGH? 1: 0;
  if (temperature < oldTemperature - 10 || temperature > oldTemperature + 10 || ovenOn != oldOn) {
    Serial.println("Old: " + String(oldTemperature) + ". New: " + String(temperature) + ". oldOn = " + String(oldOn) + ". newOn: " + String(ovenOn));

    String data = "{\"state\": \"" + String(ovenOn) + "\"}";
    httpclient.println("POST /kitchen/appliances/oven HTTP/1.1");
    httpclient.println("Host: homecontrolserver.herokuapp.com");
    httpclient.println("Content-Type: application/json");
    httpclient.println("Content-Length:" + String(data.length()));
    httpclient.println();
    httpclient.println(data);

    data = "{\"state\": \"" + String(temperature) + "\"}";
    httpclient.println("POST /kitchen/appliances/oven_temperature HTTP/1.1");
    httpclient.println("Host: homecontrolserver.herokuapp.com");
    httpclient.println("Content-Type: application/json");
    httpclient.println("Content-Length:" + String(data.length()));
    httpclient.println();
    httpclient.println(data);
  }
}
