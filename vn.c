#include <stdio.h> // printf
#include <string.h> // strstr
#include <stdint.h> // fixed-width types
#include <stdbool.h> // for bools silly
#include <stdlib.h> // atoi, atoif, strtol

#define AUTO              "YMR"
#define RRG               "RRG"

#define YPR               "$VNRRG,08*4B"
#define ypr               8

#define QTN               "$VNRRG,09*7A"
#define qtn               9

#define YMR               "$VNRRG,27*76"
#define ymr               27

#define QMR               "$VNRRG,15*77"
#define qmr               15

typedef struct
{
  // vectornav is configured to automagically send this data
  float yaw;
  float pitch;
  float roll;
  float magx;
  float magy;
  float magz;
  float accelx;
  float accely;
  float accelz;
  float gyrox;
  float gyroy;
  float gyroz;

  // this data will be polled for, referred to on the datasheet as 0-3 so this can be an array
  float quat[4];
} VectorNav;

VectorNav vnav;

float *p_vnav = NULL;
char *p_echo = NULL;

uint8_t vn_register;
char header[7];
uint8_t calc_cksum;
uint8_t rec_cksum;

uint8_t i;

bool processing;


char echo[128];

#define teststring1 "$VNRRG,08,+008.79,+013.59,+179.21*66\r\n"
#define teststring2 "$VNRRG,09,+0.963855,+0.219545,-0.148160,+0.028840*7A\r\n"
#define teststring3 "$VNRRG,27,+008.70,+013.28,+179.10,+0.3297,+0.1134,-2.2246,+01.729,-00.117,+09.400,-00.0092,+00.0014,-00.0418*43\r\n"
#define teststring4 "$VNRRG,15,+0.990252,+0.078458,-0.114043,+0.015497,+0.3278,+0.1102,-2.2198,+01.726,-00.210,+09.387,-00.0074,-00.0168,+00.0062*56\r\n"

#define teststring5 "$VNRRG,27,+006.380,+000.023,-001.953,+1.0640,-0.2531,+3.0614,+00.005,+00.344,-09.758,-0.001222,-0.000450,-0.001218*4F\r\n"

int main()
{  

  processing = true;

  char teststring[] = teststring5;

  printf("%s\n", teststring);
  calc_cksum = 0x00;
  rec_cksum = 0x00;

  p_vnav = &vnav.yaw; // start from yaw and parse all floats

  p_echo = &echo[0];


  memcpy(header, teststring, 7); // copy header data into header buffer
  header[6] = '\0'; // null terminate it for printing if need be

  printf("header:\t\t\t%s\n", header);

  p_echo += sprintf(p_echo, "%s,", header);

  if(teststring[0] != '$')
  {
    printf("mangled data, guaranteed\n");
    return -1;
  }

  if(strstr(header, AUTO) != NULL) // if auto data
  {
    vn_register = 0;
  }
  else if(strstr(header, RRG) != NULL) // polled register data
  {
    vn_register = atoi(&teststring[7]); // take register number out of string
    printf("vn_register:\t\t%d\n", vn_register);
    p_echo += sprintf(p_echo, "%d,", vn_register);
  }
  else
  {
    printf("mangled data, guaranteed\n");
    return -1;
  }


  // extend this switch with more registers, update VectorNav struct to contain it
  switch(vn_register)
  {
    case 0: // YMR auto data
    case ymr:
      break;
    case qtn: // quaternions are inbound
      p_vnav = &vnav.quat[0];
      break;
    default:
      printf("%s\t%d\n", "unknown register: ", vn_register);
      break;
  }

  // checksum loop, and parse when we can
  for(i = 1; teststring[i] != '*'; i++)
  {
    calc_cksum ^= teststring[i]; // taken from datasheet of vn100

    // comma delimited data
    if(teststring[i] == ',' && (!(vn_register) || i >= 7))
    {
      // if YMR header, go ahead and pull the floats as there will be no reg number
      // else wait until we've skipped register number and its trailing comma
      // skip the comma when by adding 1
      // do not increment or checksum will be incorrect, commas are checksummed
      *p_vnav = atof(&teststring[i + 1]);
      p_echo += sprintf(p_echo, "%f,", *p_vnav);
      p_vnav++;
    }
  }
  printf("\n");
  p_echo--; // point to the last comma, so we overwrite it

  // parse hex checksum from teststring
  rec_cksum = strtol(&teststring[i++], NULL, 16);
  rec_cksum += strtol(&teststring[i], NULL, 16);

  printf("cksum received:\t\t%X\n", rec_cksum);
  printf("cksum calculated:\t%X\n", calc_cksum);
  p_echo += sprintf(p_echo, "*%X\r\n", calc_cksum);


  // should only see this on problems with serial
  if(rec_cksum != calc_cksum)
  {
    printf("%s\n", "checksums did not match");
  }

  processing = false; // allow interrupt to overwrite data now

  p_vnav = &vnav.yaw;

  for(int j = 0; j < 16; j++)
  {
    printf("%lf\n", *p_vnav);
    p_vnav++;
  }
  
  printf("%s\n", echo);

  return 0;
}
