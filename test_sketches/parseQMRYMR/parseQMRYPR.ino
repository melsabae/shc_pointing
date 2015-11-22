// parse data from vectornav, switching based on the command header

#include "vn.h"

#define ONBOARD_LED       13
#define SERIAL_BAUD       230400

VectorNav vnav;
float *p_vnav = &vnav.yaw; // set this here so we are never in a bad state, after making vnav struct

char header[8]; // will store something like $VNxxx, and a null terminator
uint8_t vn_register;

uint8_t i;

// the setup function runs once when you press reset or power the board
void setup()
{
  // i use onboard LED to know when a chip erase and reset was successful, it will turn back on if so
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);

  Serial.begin(SERIAL_BAUD);
  while (!Serial);

  Serial1.begin(SERIAL_BAUD);
  while (!Serial1);
}

// the loop function runs over and over again until power down or reset
void loop()
{
  // clear old data from header
  //memset(header, 0, (sizeof(header)/sizeof(header[0])));

  Serial1.println(QTN);

  strcpy(header, Serial1.readStringUntil(',').c_str()); // strip the header off so we know what command came in

  if (strstr(header, "$VNYMR") != NULL)
  {
    vn_register = 0;
  }
  else
  {
    vn_register = atoi(Serial1.readStringUntil(',').c_str()); // strip the register # out, not available in auto-fed data
  }

  if (!(vn_register)) // we are getting the auto-fed data
  {
#ifdef DEBUG_VN
    Serial.println("Got auto data.");
#endif

    p_vnav = &vnav.yaw;
    while (p_vnav < &vnav.gyroz) // skip n-1th element
    {
      *p_vnav = atof(Serial1.readStringUntil(',').c_str());
      p_vnav++; // this will increment past n-1th element to nth element
    }
    *p_vnav = atof(Serial1.readStringUntil('*').c_str()); // set nth element which is not comma delimited, but checksum delimited
  }
  else
  {
    #ifdef DEBUG_VN
    Serial.print("Got requested data: ");
    Serial.print(header);
    Serial.print(" ");
    Serial.println(vn_register);
    #endif
    
    switch (vn_register)
    {
      case qtn:
        p_vnav = &vnav.quat[0];
        while (p_vnav < &vnav.quat[3]) // skip n-1th element
        {
          *p_vnav = atof(Serial1.readStringUntil(',').c_str());
          p_vnav++; // this will increment past n-1 element to nth element
        }

        *p_vnav = atof(Serial1.readStringUntil('*').c_str()); // set nth element which is not comma delimited, but checksum delimited
        break;

      default:
#ifdef DEBUG_VN
        Serial.print("Unknown case: vn_register = ");
        Serial.println(vn_register);
#endif
        break;
    }
  }

  Serial1.readStringUntil('\n'); // remove the rest of the data after the checksum

#ifdef DEBUG_VN
  DumpVNData();
#endif
}

#ifdef DEBUG_VN
void DumpVNData(void)
{
  p_vnav = &vnav.yaw;

  while (p_vnav <= &vnav.quat[3])
  {
    if (p_vnav == &vnav.yaw) // why can't switches use address values?
    {
      Serial.print("Yaw ");
    }
    else if (p_vnav == &vnav.pitch)
    {
      Serial.print("Pitch ");
    }
    else if (p_vnav == &vnav.roll)
    {
      Serial.print("Roll ");
    }
    else if (p_vnav == &vnav.magx)
    {
      Serial.print("MagX ");
    }
    else if (p_vnav == &vnav.magy)
    {
      Serial.print("MagY ");
    }
    else if (p_vnav == &vnav.magz)
    {
      Serial.print("MagZ ");
    }
    else if (p_vnav == &vnav.accelx)
    {
      Serial.print("AccelX ");
    }
    else if (p_vnav == &vnav.accely)
    {
      Serial.print("AccelY ");
    }
    else if (p_vnav == &vnav.accelz)
    {
      Serial.print("AccelZ ");
    }
    else if (p_vnav == &vnav.gyrox)
    {
      Serial.print("GyroX ");
    }
    else if (p_vnav == &vnav.gyroy)
    {
      Serial.print("GyroY ");
    }
    else if (p_vnav == &vnav.gyroz)
    {
      Serial.print("GyroZ ");
    }
    else if (p_vnav == &vnav.quat[0])
    {
      Serial.print("Quat0 ");
    }
    else if (p_vnav == &vnav.quat[1])
    {
      Serial.print("Quat1 ");
    }
    else if (p_vnav == &vnav.quat[2])
    {
      Serial.print("Quat2 ");
    }
    else if (p_vnav == &vnav.quat[3])
    {
      Serial.print("Quat3 ");
    }
    else
    {
      if (p_vnav > &vnav.quat[3])
      {
        Serial.println(">>>>>>>>EXCEEDED BOUNDS<<<<<<<<");
      }
      else
      {
        Serial.println("Pointer pointed before struct.");
      }
    }

    Serial.print(*p_vnav);
    Serial.print(" ");
    p_vnav++;
  }

  Serial.println();
}
#endif
