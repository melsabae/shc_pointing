#ifndef VN_H
#define VN_H

#define DEBUG_VN

#ifdef DEBUG_VN
#warning "VectorNav debugging statements will be printed."
#else
#warning "VectorNav debugging statements will not be printed."
#endif

// 8.2.1 Yaw Pitch Roll
#define YPR               "$VNRRG,08*4B"
#define ypr               8
// example response to this command: $VNRRG,08,+008.79,+013.59,+179.21*66


// 8.2.2 Attitude Quaternion
#define QTN               "$VNRRG,09*7A"
#define qtn               9
// example response to this command: $VNRRG,09,+0.963855,+0.219545,-0.148160,+0.028840*7A


// 8.2.3 Yaw, Pitch, Roll, Magnetic, Acceleration, and Angular Rates
#define YMR               "$VNRRG,27*76"
#define ymr               27
// example response to this command: $VNRRG,27,+008.70,+013.28,+179.10,+0.3297,+0.1134,-2.2246,+01.729,-00.117,+09.400,-00.0092,+00.0014,-00.0418*43


// 8.2.4 Quaternion, Magnetic, Acceleration and Angular Rates
#define QMR               "$VNRRG,15*77"
#define qmr               15
// example response to this command: $VNRRG,15,+0.990252,+0.078458,-0.114043,+0.015497,+0.3278,+0.1102,-2.2198,+01.726,-00.210,+09.387,-00.0074,-00.0168,+00.0062*56


// i'm skipping using arrays because the names are easier to grab when you have intellisense
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

void DumpVNData(void);

#endif
