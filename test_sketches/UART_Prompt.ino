/*
 Name:		UART_Prompt.ino
 Created:	2015-11-20 17:37:48
 Author:	saba
*/

#define ONBOARD_LED			13
#define SERIAL_BAUD			9600

#define COMMAND_HEADER		"$VN"
#define CHECKSUM_SKIP		"*XX"

// allow/disallow serial echos
#define SERIAL1_OFF			"SERIAL1 OFF"
#define SERIAL1_ON			"SERIAL1 ON"

#define SERIAL2_OFF			"SERIAL2 OFF"
#define SERIAL2_ON			"SERIAL2 ON"

#define SERIAL3_OFF			"SERIAL3 OFF"
#define SERIAL3_ON			"SERIAL3 ON"

// vectornav tentative shutup commands
/*
$VNASY, 0 * XX
$VNASY, 0 * XX\r
$VNASY, 0 * XX\n
$VNASY, 0 * XX\n\r
$VNASY, 0 * XX\r\n
$VNWRG, 6, 10 * XX
$VNWRG, 6, 20 * XX
$VNWRG, 6, 10 * XX\r
$VNWRG, 6, 20 * XX\r
$VNWRG, 6, 10 * XX\n
$VNWRG, 6, 20 * XX\n
$VNWRG, 6, 10 * XX\r\n
$VNWRG, 6, 20 * XX\r\n
$VNWRG, 6, 10 * XX\n\r
$VNWRG, 6, 20 * XX\n\r
*/

void ToggleSerial1(void);
void Checksum8(void);

// buffers for TX/RX
char rec0[2048]; // PC-Arduino serial link, TX0 and RX0 pins
char rec1[2048]; // UART1 (TX1 and RX1 pins)
char rec2[2048]; // UART2 (TX2 and RX2 pins)
char rec3[2048]; // UART2 (TX2 and RX2 pins)

bool serial1 = true;
bool serial2 = true;
bool serial3 = true;

//typedef struct
//{
//	float yaw_degs;
//	float yaw_degs;
//	float roll_degs;
//	float magx_gauss;
//	float magy_gauss;
//	float magz_gauss;
//	float accelx_ms2;
//	float accely_ms2;
//	float accelz_ms2;
//	float gyrox_rads;
//	float gyroy_rads;
//	float gyroz_rads;
//} VectorNav;
//
//VectorNav vnav;
//float *p_vnav;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(SERIAL_BAUD);
	while (!Serial);

	Serial1.begin(SERIAL_BAUD);
	while (!Serial1);

	Serial2.begin(SERIAL_BAUD);
	while (!Serial2);

	Serial3.begin(SERIAL_BAUD);
	while (!Serial3);

	// i use onboard LED to know when a chip erase and reset was successful, it will turn back on if so
	pinMode(ONBOARD_LED, OUTPUT);
	digitalWrite(ONBOARD_LED, LOW);

	Serial.println();
	Serial.println();
	Serial.println("MASTER: Done with setup.");
	Serial.println("Serial: toggle with SERIAL[1,2,3] [ON,OFF]");
	Serial.println();
}

// the loop function runs over and over again until power down or reset
void loop()
{
	// check to see if we sent anything to Serial0, send them to Serial1-3
	if (Serial.available() > 0)
	{
		strcpy(rec0, Serial.readStringUntil('\n').c_str());

		// turn all commands into uppercase, vectornav is fine with this
		for (uint16_t i = 0; i < sizeof(rec0) / sizeof(rec0[0]); i++)
		{
			rec0[i] = toupper(rec0[i]);
		}

		ToggleSerial1();

		Serial.println("Sending to UART(s): ");
		Serial.print('\t');
		Serial.println(rec0);

		if (serial1)
		{
			Serial1.println(rec0);
		}

		if (serial2)
		{
			Serial2.println(rec0);
		}

		if (serial3)
		{
			Serial3.println(rec0);
		}
	}

	// print any data off of Serials to Serial0, store in buffer for later parsing
	// if the serial echo is off, it will still read out of it to keep the buffer from filling
	// this may cause issues with some sensors if the buffer fills
	if (Serial1.available() > 0)
	{
		strcpy(rec1, Serial1.readStringUntil('\n').c_str());

		if (serial1)
		{
			Serial.println("UART1 Received: ");
			Serial.print('\t');
			Serial.println(rec1);
		}
	}

	if (Serial2.available() > 0)
	{
		strcpy(rec2, Serial2.readStringUntil('\n').c_str());
		
		if (serial2)
		{
			Serial.println("UART2 Received: ");
			Serial.print('\t');
			Serial.println(rec2);
		}
	}

	if (Serial3.available() > 0)
	{
		strcpy(rec3, Serial3.readStringUntil('\n').c_str());

		if (serial3)
		{
			Serial.println("UART3 Received: ");
			Serial.print('\t');
			Serial.println(rec3);
		}
	}
}

void ToggleSerial1()
{
	if (strstr(rec0, SERIAL1_OFF) != NULL && (strstr(rec0, SERIAL1_ON) != NULL))
	{
		Serial.println();
		Serial.println("Make up your mind!");

		if (serial1)
		{
			Serial.println("Serial1 is still on.");
		}
		else
		{
			Serial.println("Serial1 is still off.");
		}

		Serial.println();
	}
	else if (strstr(rec0, SERIAL1_ON) != NULL) // serial1 is off and user says turn it on
	{
		serial1 = true;
		Serial.println();
		Serial.println("Serial1 will print and receive commands.");
		Serial.println();
	}
	else if (strstr(rec0, SERIAL1_OFF) != NULL) // serial1 is on and user says turn it off
	{
		serial1 = false;
		Serial.println();
		Serial.println("Serial1 will not print or receive commands.");
		Serial.println();
	}

	if (strstr(rec0, SERIAL2_OFF) != NULL && (strstr(rec0, SERIAL2_ON) != NULL))
	{
		Serial.println();
		Serial.println("Make up your mind!");

		if (serial2)
		{
			Serial.println("Serial2 is still on.");
		}
		else
		{
			Serial.println("Serial2 is still off.");
		}

		Serial.println();
	}
	else if (strstr(rec0, SERIAL2_ON) != NULL) // serial1 is off and user says turn it on
	{
		serial2 = true;
		Serial.println();
		Serial.println("Serial2 will print and receive commands.");
		Serial.println();
	}
	else if (strstr(rec0, SERIAL2_OFF) != NULL) // serial1 is on and user says turn it off
	{
		serial2 = false;
		Serial.println();
		Serial.println("Serial2 will not print or receive commands.");
		Serial.println();
	}

	if (strstr(rec0, SERIAL3_OFF) != NULL && (strstr(rec0, SERIAL3_ON) != NULL))
	{
		Serial.println();
		Serial.println("Make up your mind!");

		if (serial3)
		{
			Serial.println("Serial3 is still on.");
		}
		else
		{
			Serial.println("Serial3 is still off.");
		}

		Serial.println();
	}
	else if (strstr(rec0, SERIAL3_ON) != NULL) // serial1 is off and user says turn it on
	{
		serial3 = true;
		Serial.println();
		Serial.println("Serial3 will print and receive commands.");
		Serial.println();
	}
	else if (strstr(rec0, SERIAL3_OFF) != NULL) // serial1 is on and user says turn it off
	{
		serial3 = false;
		Serial.println();
		Serial.println("Serial3 will not print or receive commands.");
		Serial.println();
	}
}
