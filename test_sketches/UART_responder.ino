/*
 Name:		UART_Responder.ino
 Created:	2015-11-20 16:57:42
 Author:	saba
*/

#define ONBOARD_LED		13
#define SERIAL_BAUD		9600

char send0[2048];

int count = 0;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(SERIAL_BAUD);
	while (!Serial);

	Serial1.begin(SERIAL_BAUD);
	while (!Serial1);

	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);

	// tell me the Serial lines are open, board is also done setting up
	Serial.println();
	Serial.println();
	Serial.println("SLAVE: Done with setup.");
}

// the loop function runs over and over again until power down or reset
void loop()
{
	if (Serial1.available() > 0)
	{
		++count;
		strcpy(send0, Serial1.readStringUntil('\n').c_str());

		Serial.println("Received: ");
		Serial.print('\t');
		Serial.println(send0);


		Serial.println("Sending: ");
		Serial.print('\t');
		Serial.print(count);
		Serial.print(" ");
		Serial.println(send0);

		Serial1.print(count);
		Serial1.print(" ");
		Serial1.println(send0);
	}
}

