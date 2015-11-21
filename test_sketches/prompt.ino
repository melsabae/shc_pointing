/*
  Name:    UART_Prompt.ino
  Created: 2015-11-20 17:37:48
  Author:  saba
*/

#define ONBOARD_LED     13
#define SERIAL_BAUD     230400

// buffers for TX/RX
char rec0[2048]; // PC-Arduino serial link, TX0 and RX0 pins
char rec1[2048]; // Arduino-Vecotrnav serial link, TX1 and RX1 pins

char send1[13]; // send buffer, all read commands take the form of $VNxxx,xx*xx, plus a null terminator makes for 13
//char send2[2048]; // send buffer for writing, varibale length commands
uint8_t cksum;

// the setup function runs once when you press reset or power the board
void setup()
{
  // clean garbage data
  cksum = 0x00; // remove garbage from checksum byte
  memset(send1, 0, (sizeof(send1) / sizeof(send1[0]))); // 0 out the send buffer to get rid of garbages
  
  Serial.begin(SERIAL_BAUD);
  while (!Serial);

  Serial1.begin(SERIAL_BAUD);
  while (!Serial1);

  // i use onboard LED to know when a chip erase and reset was successful, it will turn back on if so
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);

  Serial.println("MASTER: Done with setup.");
  Serial.println();
}

// the loop function runs over and over again until power down or reset
void loop()
{
  // check to see if we sent anything to Serial0, send it to Serial1
  if (Serial.available() > 0)
  {
    // store send data from serial into buffer
    strcpy(rec0, Serial.readStringUntil('\n').c_str());

    // turn all commands into uppercase, vectornav is fine with this
    // calculate checksum while i'm in here
    for (uint16_t i = 0; i < (sizeof(rec0) / sizeof(rec0[0])); i++)
    {
      if (rec0[i] == '\0')
      {
        break;
      }

      rec0[i] = toupper(rec0[i]);

      // vectornav datasheet says XOR all data that isn't a '$' or an '*', but ',' should be XOR'd
      if ((rec0[i] != '$') && (rec0[i] != '*'))
      {
        cksum ^= rec0[i];
      }
    }

    // store the command into the send buffer, then add in the checksum
    sprintf(send1, "%s*%X", rec0, cksum);  

    // reset checksum for next use
    cksum = 0x00;

    Serial.println("Sending to UART(s): ");
    Serial.print('\t');
    Serial.println(send1);

    // dump command to vectornav
    Serial1.println(send1);
  }

  // print any data off of Serial1 to Serial, store in buffer for later parsing
  if (Serial1.available() > 0)
  {
    strcpy(rec1, Serial1.readStringUntil('\n').c_str());
    //if (!(strstr(rec1, "$VNYMR") != NULL)) // this will disable YMR spam
    {
      Serial.println("UART1 Received: ");
      Serial.print('\t');
      Serial.println(rec1);
    }
  }
}
