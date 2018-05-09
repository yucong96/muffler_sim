#ifndef __CONSTANT_H__
#define __CONSTANT_H__

const int FACE_NODE_NUM = 3;
const int VOLUME_NODE_NUM = 4;

const double TOLERANCE = 1e-5;
const double PI = 3.1415926;
const double INF = 1e10;

const double DEFAULT_FREQ = 300;
const double DEFAULT_SPEED = 340;
const double DEFAULT_RHO = 1.29;

const int ERR_FILE_OPEN = 1;
const int ERR_FILE_FORMAT = 2;
const int ERR_WRONG_ARG_NUMBER = 3;

const int AREA_NORMALIZE = 0;
const int UNIT_NORMALIZE = 1;

//#define TEST
#define LOG
#define LINUX
// #define WINDOWS

#define NORMAL

#endif
