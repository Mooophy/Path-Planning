#ifndef __LPASTAR_H__
#define __LPASTAR_H__

#include <vector> 
#include "globalVariables.h"

class GridWorld; //forward declare class GridWorld to be able to create the friend functions later

class LpaStar 
{
public:
    LpaStar(int rows, int cols);

    void initialise(int startX, int startY, int goalX, int goalY);
    double minValue(double g_, double rhs_);
    //double maxValue(double v1, double v2);
    int maxValue(int v1, int v2);
    void calcKey(int x, int y);
    void calcKey(LpaStarCell *cell);
    //void calc_H(int x, int y);
    double calc_H(int x, int y);
    void updateHValues();
    void updateAllKeyValues();

    //void copyMazeToDisplayMap(GridWorld gWorld);
    friend void copyMazeToDisplayMap(GridWorld &gWorld, LpaStar* lpa);
    friend void copyDisplayMapToMaze(GridWorld &gWorld, LpaStar* lpa);

private:
    vector<vector<LpaStarCell>> maze;
    LpaStarCell l;
    vector<LpaStarCell*> U; //Priority Queue
    LpaStarCell* start;
    LpaStarCell* goal;

    int rows;
    int cols;
};
#endif
