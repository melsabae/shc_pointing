/*
 Name:		DuePlayground.ino
 Created:	2015-11-13 17:21:42
 Author:	saba
*/

#define ONBOARD_LED		13
#define SERIAL_BAUD		9600
#define DELIMS			" ,.-"

char buffer[1024];
char *pch;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(SERIAL_BAUD);
	Serial1.begin(SERIAL_BAUD);
	while(!Serial1);
	while (!Serial);

	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	strcpy(buffer, Serial1.readStringUntil('\n').c_str());
	Serial1.print("$VNRRG,8*4B");
	pch = strtok(buffer, DELIMS);

	while (pch != NULL)
	{
		Serial.print(pch);
		pch = strtok(NULL, DELIMS);
		if (pch != NULL)
		{
			Serial.print("_");
		}
	}
	Serial1.flush();
	
}

// this code would read from atxmega
//if (Serial1.readStringUntil('\n') == "Hello")
//{
//	Serial.println("Hi there");
//}

