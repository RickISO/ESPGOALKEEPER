#ifndef CONSTANTS_H
#define CONSTANTS_H
struct variables
{
   
  int ledSol = 9;
  int enable = 4;
  int direccion = 5;
  int stepper = 6;

  // Inputs
  int limitLeft = 2;
  int limitRight = 3;

  // Controller
  const char *ps4Mac = "01:02:03:04:05:06";
  int deadzone = 25;
  unsigned long stepInterval = 1000;
};


#endif