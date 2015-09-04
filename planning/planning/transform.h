#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__


#include <math.h>
#include <cstdlib>
#include <iostream>
 

using namespace std;

//const float PI = 3.14159265358979323846f;  //use M_PI from math.h instead
const float M_PI = 3.14159265358979323846f;
typedef struct
{
  float x1,y1,x2,y2;
} WorldBoundaryType; 

typedef struct
{
  int x1,y1,x2,y2;
} DevBoundaryType; 



/// Function Prototypes ////////////////////////////////////////////////////////////////////

float degToRad(float deg);
int xDev(WorldBoundaryType WorldBound,DevBoundaryType DevBound,float xworld);
int yDev(WorldBoundaryType WorldBound,DevBoundaryType DevBound,float yworld);

float xWorld(WorldBoundaryType WorldBound,DevBoundaryType DevBound,int xDev);
float yWorld(WorldBoundaryType WorldBound,DevBoundaryType DevBound,int yDev);

int  randomVal(int min, int max);
float randomVal(float min, float max);


#endif
