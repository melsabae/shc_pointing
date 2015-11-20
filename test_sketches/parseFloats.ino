/*
  Name:		DuePlayground.ino
  Created:	2015-11-13 17:21:42
  Author:	saba
*/

/*
   When the vectornav is set up to automatically spit out YPR,Mag,Accell,Gyro, this parses that data into the 12 floats
   Assumes using TX1 and RX1 pins on the arduino due, pins #18-19
*/

#define SERIAL_BAUD		115200 // max 50 hz on vectornav according to vectornav utility

#define SERIALPRINT

typedef struct
{
  float yaw_degs;
  float yaw_degs;
  float roll_degs;
  float magx_gauss;
  float magy_gauss;
  float magz_gauss;
  float accelx_ms2;
  float accely_ms2;
  float accelz_ms2;
  float gyrox_rads;
  float gyroy_rads;
  float gyroz_rads;
} VectorNav;

VectorNav vnav;
float *p_vnav;

// the setup function runs once when you press reset or power the board
void setup()
{
#ifdef SERIALPRINT
  Serial.begin(SERIAL_BAUD);
  while (!Serial);
#endif

  Serial1.begin(SERIAL_BAUD);
  while (!Serial1);
}

// the loop function runs over and over again until power down or reset
void loop()
{
  p_vnav = &vnav.yaw_deg;

  // vectornav spits out comma delimited data
  for (uint8_t i = 0; i < 12; i++) // 12 is number of floats in vnav object
  {
    Serial1.readStringUntil(','); // will skip the command header and commas
    *p_vnav = atof(Serial1.readStringUntil(',').c_str()); // store the float into the vectornav object pulled from the stream

#ifdef SERIALPRINT
    Serial.print(*p_vnav);
    Serial.print(',');
    Serial.print(' ');
#endif

    p_vnav++; // beware the trickery of pointer arithmetic
  }
  
#ifdef SERIALPRINT
  Serial.println();
#endif
}
