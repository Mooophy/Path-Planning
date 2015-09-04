#ifndef __GRIDWORLD_H__
#define __GRIDWORLD_H__

#include <iostream>
#include <fstream>
#include <vector>

#include "transform.h"
#include "globalvariables.h"
#include "graphics.h"
#include "LpaStar.h"


class GridWorld {

public:
    GridWorld() {
        xInc = 0;
        yInc = 0;
        strcpy(fileName, "");
    }

    bool isGridMapInitialised() {
        return MAP_INITIALISED;
    }

    void initSystemOfCoordinates();
    void initialiseMapConnections();
    void loadMapAndDisplay(const char* fn);
    //------------------------------------------------
    void displayHeader();
    //void displayRLHeader();
    void displayRLHeader(long maxEpoch);
    void displayMap();
    void displayMapWithDetails();
    void displayMapWithPositionDetails();
    void displayMapWithKeyDetails();
    void displayMapWithSelectedDetails(bool display_g, bool display_rhs, bool display_h, bool display_key);

    //--------------------------------------------------
    //void displayMapWithRLDetails(bool display_reward, bool display_maxQ, long maxEpoch);
    //void displayRLShortestPath(bool display_reward, bool display_maxQ); 
    //--------------------------------------------------

    void displayVertexConnections(int i, int j);
    void displayMapConnections();
    //------------------------------------------------
    void drawGrid();

    Coordinates markCell_col_row(int col, int row, int fillColour, int outlineColour);
    void markCell(int x, int y);
    CellPosition getCellPosition_markCell(int x, int y);

    int getGridMaxX();
    int getGridMaxY();

    WorldBoundaryType getWorldBoundary() {
        return worldBoundary;
    }
    DevBoundaryType getDeviceBoundary() {
        return deviceBoundary;
    }

    int getCellWidth() {
        return cellWidth;
    }

    int getCellHeight() {
        return cellHeight;
    }

    int getFieldX1() {
        return fieldX1;
    }
    int getFieldY1() {
        return fieldY1;
    }
    int getFieldX2() {
        return fieldX2;
    }
    int getFieldY2() {
        return fieldY2;
    }

    int getGridWorldRows() {
        return GRIDWORLD_ROWS;
    }

    int getGridWorldCols() {
        return GRIDWORLD_COLS;
    }

    //-------------------------------------------------
    void setMapTypeValue(int row, int col, char tp) {
        map[row][col].type = tp;
    }

    void setMapGValue(int row, int col, double g) {
        map[row][col].g = g;
    }

    void setMapRhsValue(int row, int col, double rhs) {
        map[row][col].rhs = rhs;
    }

    void setMapHValue(int row, int col, double h) {
        map[row][col].h = h;
    }

    void setMapFValue(int row, int col, double f) {
        map[row][col].f = f;
    }

    void setMapStatusValue(int row, int col, char s) {
        map[row][col].status = s;
    }

    void setStartVertex(vertex s) {
        if (!MAP_INITIALISED) {
            cout << "Error in getStartVertex()" << endl;
            exit(1);
        }

        if ((s.row != -1) && (s.col != -1))
            startVertex = s;
        else {
            cout << "Error in GridWorld::setStartVertex()" << endl;
            exit(1);
        }
    }

    void setGoalVertex(vertex s) {
        if (!MAP_INITIALISED) {
            cout << "Error in getGoalVertex()" << endl;
            exit(1);
        }

        if ((s.row != -1) && (s.col != -1))
            goalVertex = s;
        else {
            cout << "Error in GridWorld::setGoalVertex()" << endl;
            exit(1);
        }
    }

    //-------------------------------------------------
    char getMapTypeValue(int row, int col) {
        return map[row][col].type;
    }

    double getMapRhsValue(int row, int col) {
        return map[row][col].rhs;
    }
    double getMapGValue(int row, int col) {
        return map[row][col].g;
    }
    double getMapFValue(int row, int col) {
        return map[row][col].f;
    }
    double getMapHValue(int row, int col) {
        return map[row][col].h;
    }

    vertex getStartVertex() {
        if (!MAP_INITIALISED) {
            cout << "Error in getStartVertex()" << endl;
            exit(1);
        }
        return startVertex;
    }

    vertex getGoalVertex() {
        if (!MAP_INITIALISED) {
            cout << "Error in getStartVertex()" << endl;
            exit(1);
        }
        return goalVertex;
    }

    void markCell_col_row_details(int col, int row, int fillColour, int outlineColour);
    void markCell_col_row_details(int col, int row, int fillColour, int outlineColour, bool display_g, bool display_rhs, bool display_h, bool display_key);
    void markCell_col_row_details_xy(int col, int row, int fillColour, int outlineColour);
    void markCell_col_row_RL_details(int col, int row, int fillColour, int outlineColour, bool display_reward, bool display_maxQ);

    //----------------------------------------------------------------------------
    // friend functions
    friend void copyMazeToDisplayMap(GridWorld &gWorld, LpaStar* lpa);
    friend void copyDisplayMapToMaze(GridWorld &gWorld, LpaStar* lpa);

private:
    int cellWidth;
    int cellHeight;

    int GRIDWORLD_ROWS;
    int GRIDWORLD_COLS;

    WorldBoundaryType worldBoundary;
    DevBoundaryType deviceBoundary;

    float WORLD_MAXX;
    float WORLD_MAXY;
    int fieldX1, fieldY1, fieldX2, fieldY2;
    vector<vector<vertex> > map;

    vertex startVertex;
    vertex goalVertex;
    bool MAP_INITIALISED;
    int xInc;
    int yInc;

    char fileName[25];

    vector<loc_t> shortestPath; //RL - shortest path
};

#endif
