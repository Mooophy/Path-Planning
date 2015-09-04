#include "transform.h"
 

using namespace std;

/////////////////////////////////////////////////////////////////////////////////
// World-to-Device Coordinates Transformation 

int xDev(WorldBoundaryType WorldBound,DevBoundaryType DevBound,float xworld)
{
    float xslope,xint;
	 int devX;
    
    if((WorldBound.x2-WorldBound.x1) == 0.0f)
      xslope = float(DevBound.x2-DevBound.x1)/(WorldBound.x2-WorldBound.x1 + 0.001f);
    else
      xslope = float(DevBound.x2-DevBound.x1)/(WorldBound.x2-WorldBound.x1);
    xint = DevBound.x1-xslope*WorldBound.x1;

    devX = (int)ceil(xslope*xworld+xint);
	 //~ if(devX > (int)DevBound.x2) devX = (int)DevBound.x2;
	 //~ if(devX < (int)DevBound.x1) devX = (int)DevBound.x1;
	 return devX;
}

int yDev(WorldBoundaryType WorldBound,DevBoundaryType DevBound,float yworld)
{
    float yslope,yint;
	 int devY;
    
    if((WorldBound.y2-WorldBound.y1) == 0.0f)
       yslope = float(DevBound.y2-DevBound.y1)/(WorldBound.y2-WorldBound.y1 + 0.001f);
    else
       yslope = float(DevBound.y2-DevBound.y1)/(WorldBound.y2-WorldBound.y1);
    yint = DevBound.y1-yslope*WorldBound.y1;

    devY = (int)ceil(yslope*yworld+yint);
	 //~ if(devY > (int)DevBound.y2) devY = (int)DevBound.y2;
	 //~ if(devY < (int)DevBound.y1) devY = (int)DevBound.y1;
	 return (int)devY;
}

//////////////////////////////////////////////////////////////////////////////////////////

float xWorld(WorldBoundaryType WorldBound,DevBoundaryType DevBound,int xDev)
{
    float xslope,xint;
	 float worldX;
    
    if((DevBound.x2-DevBound.x1) == 0)
      xslope = (WorldBound.x2-WorldBound.x1)/(float(DevBound.x2)-float(DevBound.x1) + 0.001f);
    else
      xslope = (WorldBound.x2-WorldBound.x1)/(float(DevBound.x2)-float(DevBound.x1));
    xint = WorldBound.x1-xslope*DevBound.x1;

	 //for debugging purposes only
	 //~ cout << "numerator = " << (WorldBound.x2-WorldBound.x1) << endl;
	 //~ cout << "denomimator = " << (float(DevBound.x2)-float(DevBound.x1)) << endl;
	 //~ cout << "xslope = " << xslope << endl;
	 //~ cout << "xint = " << xint << endl;
	 
    worldX = (float)ceil(xslope*(float)xDev+xint);
	 
	 if(worldX >= (float)WorldBound.x2) worldX = WorldBound.x2;
	 if(worldX <= (float)WorldBound.x1) worldX = WorldBound.x1;
	 return worldX;
}

float yWorld(WorldBoundaryType WorldBound,DevBoundaryType DevBound,int yDev)
{
    float yslope,yint;
	 float worldY;
    
    if((DevBound.y2-DevBound.y1) == 0)
      yslope = (WorldBound.y2-WorldBound.y1)/(float(DevBound.y2)-float(DevBound.y1) + 0.001f);
    else
      yslope = (WorldBound.y2-WorldBound.y1)/(float(DevBound.y2)-float(DevBound.y1));
    yint = WorldBound.y1-yslope*DevBound.y1;

    worldY = (float)ceil(yslope*(float)yDev + yint);
	 
	 if(worldY >= (float)WorldBound.y1) worldY = WorldBound.y1;
	 if(worldY <= (float)WorldBound.y2) worldY = WorldBound.y2;
	 return worldY;
}


float degToRad(float deg) {
	return(deg * M_PI / 180.0f);
}

int  randomVal(int min, int max)
{
	return (min + (rand() % ((max-min)+1)  ));
}

float randomVal(float min, float max)
{
	float r;
	
	r = (float)rand()/RAND_MAX;
	r = min + (r*(max-min));
	
	return r;
}

