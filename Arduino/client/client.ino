#define W5100

#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include "Ethernet.h"
#include "SPI.h"


//Pin port numbers
int redLedPin = 9;
int greenLedPin = 8;
int buttonPin = 7;
int yellowLedPin = 6;
int buzzerPin = 5;
int buttonState = 0;

//Ethernet configurations
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "homecontrolserver.herokuapp.com";    // name address for HomeControl Server (using DNS)
SocketIOClient client;

//Socket.io configurations
extern String RID;
extern String Rname;
extern String Rcontent;

//Heartbeat configurations - to keep the connection alive
unsigned long previousMillis = 0;
long interval = 10000;

void setup() {

  //Set pin modes
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(redLedPin, HIGH); //Start with red led on - TODO request state to server
  digitalWrite(yellowLedPin, HIGH); //Status - Preparing
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

  digitalWrite(yellowLedPin, LOW); //Status - Prepared
}

// the loop function runs over and over again forever
void loop() {

  //Handmade heartbeat
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    client.heartbeat(0);
  }

  //Button status update
  buttonState = digitalRead(buttonPin);

  //If the button is pushed
  if (buttonState == HIGH) {
    changeStateButton();
    while (digitalRead(buttonPin) != LOW);
  }

  //If there's new messages from the websocket
  if (client.monitor()) {
    Serial.flush();

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.parse(Rcontent);

    if (!json.success()) Serial.println("parseObject() failed!");

    //If it's a status update
    if (RID == "newState" && Rname == "state") {
      String division = json["division"];
      String type = json["type"];
      String object = json["object"];

      //If the status update is regarding the oven represented by this arduino
      if (division == "kitchen" && type == "appliances" && object == "oven") {
        int state = atoi(json["state"]);
        changeStateUpdate(state);
      }
    }
  }
}

void changeStateUpdate(int newState) {
  if (newState > 0) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    tone(buzzerPin, 220, 200);
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    tone(buzzerPin, 220, 200);
  }
}

void changeStateButton() {
  if (digitalRead(redLedPin) == HIGH)  {
    Serial.println("RED LED 1");
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    tone(buzzerPin, 220, 200);
  } else {
    Serial.println("GREEN LED 1");
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    tone(buzzerPin, 220, 200);
  }
}

void shutdown() {
  Serial.println("Shutting down");
  while (1) {
    digitalWrite(yellowLedPin, HIGH);
    delay(500);
    digitalWrite(yellowLedPin, LOW);
    delay(500);
  }
}

void playMelody() {
  tone(buzzerPin, 740, 300);
  delay(400);
  tone(buzzerPin, 740, 700);
}
