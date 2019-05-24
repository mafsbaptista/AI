#define W5100

#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include "Ethernet.h"
#include "SPI.h"


//Pin port numbers
int redLedPin = 9;
int greenLedPin = 8;
int buttonPin = 7;
int buzzerPin = 5;
int temperaturePotPin = A0;
int buttonState = 0;

//Temperature sensor values
int temperature;
int desiredTemperature = 180;

//Old values
int oldTemperature = -1;
int oldOn = 0;


//Ethernet configurations
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "homecontrolserver.herokuapp.com";    // name address for HomeControl Server (using DNS)
SocketIOClient client;
EthernetClient httpclient;

//Socket.io configurations
extern String received;

//Heartbeat configurations - to keep the connection alive
unsigned long previousMillis = 0;
long interval = 5000;

void setup() {

  //Set pin modes
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(temperaturePotPin, INPUT); //Optional 
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW); //Start with green led off - TODO request state to server
  Serial.begin(9600);

  playMelody();

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

  requestState();

}

// the loop function runs over and over again forever
void loop() {

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
    if (temperature >= desiredTemperature) {
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
      received = "";
    }
  }
  
  //Handmade heartbeat
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    client.heartbeat(0);
  }

  delay(100);
}

void changeStateUpdate(int newState) {
  if (newState > 0) {
    digitalWrite(greenLedPin, HIGH);
    tone(buzzerPin, 220, 200);
    oldOn = true;
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    tone(buzzerPin, 220, 200);
    oldOn = false;
  }
}

void changeStateButton() {
  //If oven is turned off
  if (digitalRead(greenLedPin) == HIGH)  {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    tone(buzzerPin, 220, 200);
    oldOn = 0;
    //If oven is turned on
  } else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    tone(buzzerPin, 220, 200);
    oldOn = 1;
  }
}

void shutdown() {
  Serial.println("Shutting down");
  while (1) {
    delay(500);
  }
}

void playMelody() {
  tone(buzzerPin, 740, 300);
  delay(400);
  tone(buzzerPin, 740, 700);
}

void requestState() {
  String path = "kitchen/appliances/oven";
  httpclient.println("GET /kitchen/appliances/oven HTTP/1.1");
  httpclient.println("Host: homecontrolserver.herokuapp.com");
  httpclient.println();

  char buffer[10];

  while (true) {
    if (httpclient.available() > 0) {
      char c = httpclient.read();
      if (c == '\n' && httpclient.available() > 0) {
        httpclient.read();
        c = httpclient.read();
        if (c == '\n') {
          int len = httpclient.available();
          if (len > 10) len = 10;
          httpclient.read(buffer, len);
          if (atoi(buffer) > 0) {
            digitalWrite(greenLedPin, HIGH);
          }
          break;
        }
      }
    }
  }
}
