#include <SPI.h>
#include <Ethernet.h>

//Pin port numbers
int redLedPin = 10;
int greenLedPin = 9;
int buttonPin = 8;
int yellowLedPin = 7;
int buzzerPin = 6;
int buttonState = 0;

//Ethernet configurations
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "homecontrolserver.herokuapp.com";    // name address for HomeControl Server (using DNS)
EthernetClient client;

void setup() {
  
  //Set pin mode
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

  if (client.connect(server, 80)) {
      Serial.print("Connected to ");
      Serial.println(client.remoteIP());
      digitalWrite(yellowLedPin, LOW); //Status - Prepared
  } else shutdown();
  
}

// the loop function runs over and over again forever
void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (digitalRead(redLedPin) == HIGH)  {
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
      tone(buzzerPin, 220, 200);
    } else {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
      tone(buzzerPin, 220, 200);
    }
    while(digitalRead(buttonPin) != LOW);
  }
  delay(100);
}

void shutdown() {
  Serial.println("Shutting down");
  while(1) {
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
