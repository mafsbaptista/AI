#define W5100
#include <SocketIOClient.h>


String received = "";

bool SocketIOClient::connect(char thehostname[], int theport) {
	if (!client.connect(thehostname, theport)) return false;
	hostname = thehostname;
	port = theport;
	sendHandshake(hostname);
	return readHandshake();
}

bool SocketIOClient::connectHTTP(char thehostname[], int theport) {
	if (!client.connect(thehostname, theport)) return false;
	hostname = thehostname;
	port = theport;
	return true;
}


bool SocketIOClient::connected() {
	return client.connected();
}

void SocketIOClient::parser(int index) {
	int sizemsg = databuffer[index + 1];   // 0-125 byte, index ok        Fix provide by Galilei11. Thanks
	if (databuffer[index + 1]>125)
	{
		sizemsg = databuffer[index + 2];    // 126-255 byte
		index += 1;       // index correction to start
	}

	switch (databuffer[2])
	{
	case '2':
		heartbeat(1);
		break;

	case '3':
		Serial.println("Pong received - All good");
		break;

	case '4':
		switch (databuffer[3])
		{
		case '0':
			break;
		case '2':
			received = "";
			for (int i = index + 4; i < index + sizemsg + 2; i++) 
				received += (char)databuffer[i];
			break;
		}
	}
}

bool SocketIOClient::monitor() {
	int index = -1;
	*databuffer = 0;

	if (!client.connected()) {
		if (!client.connect(hostname, port)) return 0;
	}

	if (!client.available())
	{
		return 0;
	}
	char which;

	while (client.available()) {
		readLine();
		//Serial.println(databuffer);
		index = atoi(strchr(databuffer, (char)129) - databuffer + 1);
		Serial.println(index);
		/*Serial.print("Index = ");			//Can be used for debugging
		Serial.print(index);
		Serial.print(" & Index2 = ");
		Serial.println(index2);*/
		if (index != -1)
		{
			parser(index);
		}
	}
	Serial.flush();
}

void SocketIOClient::sendHandshake(char hostname[]) {
	client.println(F("GET /socket.io/1/?transport=polling&b64=true HTTP/1.1"));
	client.print(F("Host: "));
	client.println(hostname);
	client.println(F("Origin: Arduino\r\n"));
}

bool SocketIOClient::waitForInput(void) {
	unsigned long now = millis();
	while (!client.available() && ((millis() - now) < 30000UL)) { ; }
	return client.available();
}

void SocketIOClient::eatHeader(void) {
	while (client.available()) {	// consume the header
		readLine();
		if (strlen(databuffer) == 0) break;
	}
}

bool SocketIOClient::readHandshake() {

	if (!waitForInput()) return false;

	// check for happy "HTTP/1.1 200" response
	readLine();
	if (atoi(&databuffer[9]) != 200) {
		while (client.available()) readLine();
		client.stop();
		return false;
	}
	eatHeader();
	readLine();
	String tmp = databuffer;

	int sidindex = tmp.indexOf("sid");
	int sidendindex = tmp.indexOf("\"", sidindex + 6);
	int count = sidendindex - sidindex - 6;

	for (int i = 0; i < count; i++)
	{
		sid[i] = databuffer[i + sidindex + 6];
	}
	//Serial.println(" ");
	//Serial.print(F("Connected. SID="));
	//Serial.println(sid);	// sid:transport:timeout 

	while (client.available()) readLine();
	client.stop();
	delay(1000);

	// reconnect on websocket connection
	if (!client.connect(hostname, port)) {
		Serial.print(F("Websocket failed."));
		return false;
	}
	//Serial.println(F("Connecting via Websocket"));

	client.print(F("GET /socket.io/1/websocket/?transport=websocket&b64=true&sid="));
	client.print(sid);
	client.print(F(" HTTP/1.1\r\n"));

	client.print(F("Host: "));
	client.print(hostname);
	client.print("\r\n");
	client.print(F("Sec-WebSocket-Version: 13\r\n"));
	client.print(F("Origin: ArduinoSocketIOClient\r\n"));
	client.print(F("Sec-WebSocket-Extensions: permessage-deflate\r\n"));
	client.print(F("Sec-WebSocket-Key: IAMVERYEXCITEDESP32FTW==\r\n")); // can be anything

	client.print(F("Cookie: io="));
	client.print(sid);
	client.print("\r\n");
	
	client.print(F("Connection: Upgrade\r\n"));

	client.println(F("Upgrade: websocket\r\n"));	// socket.io v2.0.3 supported




	if (!waitForInput()) return false;
	readLine();
	if (atoi(&databuffer[9]) != 101) {	// check for "HTTP/1.1 101 response, means Updrage to Websocket OK
		while (client.available()) readLine();
		client.stop();
		return false;
	}
	readLine();
	readLine();
	readLine();

	eatHeader();


	/*
	Generating a 32 bits mask requiered for newer version
	Client has to send "52" for the upgrade to websocket
	*/
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "52";
	String message = "52";
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("52")



	client.print((char)0x81);	//has to be sent for proper communication
	client.print((char)130);	//size of the message (2) + 128 because message has to be masked
	client.print(mask);
	client.print(masked);

	monitor();		// treat the response as input
	return true;
}


void SocketIOClient::readLine() {
	for (int i = 0; i < DATA_BUFFER_LEN - 1; i++)
		databuffer[i] = ' ';
	char * dataptr = databuffer;
	while (client.available() && (dataptr < &databuffer[DATA_BUFFER_LEN - 2]))
	{
		char c = client.read();
		if (c == '\r') { ; }
		else if (c == '\n') break;
		else *dataptr++ = c;
	}
	*dataptr = 0;
}


void SocketIOClient::heartbeat(int select) {
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "";
	String message = "";
	if (select == 0)
	{
		masked = "2";
		message = "2";
	}
	else
	{
		masked = "3";
		message = "3";
	}
	for (int i = 0; i < 4; i++)	//generate a random mask, 4 bytes, ASCII 0 to 9
	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < message.length(); i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("2" : ping or "3" : pong)



	client.print((char)0x81);	//has to be sent for proper communication
	client.print((char)129);	//size of the message (1) + 128 because message has to be masked
	client.print(mask);
	client.print(masked);
}
