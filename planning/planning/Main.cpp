///////////////////////////////////////////////////////////////////////////////////////////
//
//
//  
//                        
//
//	 	      Program Name: Incremental Search 
//	 	       Description: start-up code for simulating LPA* and D*Lite
//                        - implements a gridworld that can be loaded from file, and 
//                          modified through a user-interface 
//
//          Run Parameters: 
//
//      Keys for Operation: 
//
//	 		        History:  date of revision
//                         28/July/2015  
//                         03/Aug/2014  
//
//      Start-up code by:    n.h.reyes@massey.ac.nz
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <set>
#include <vector>

#include "../lib/a_star_sel.hpp"
#include "../lib/utility.hpp"

//-------------------------
#include "globalVariables.h"
#include "transform.h"
#include "graphics.h"
#include "AstarSearch.h"
#include "LPAstar.h"
#include "gridworld.h"

// colour constants
int BACKGROUND_COLOUR;
int LINE_COLOUR;

int robotWidth;
int GRIDWORLD_ROWS; //duplicated in GridWorld
int GRIDWORLD_COLS; //duplicated in GridWorld

//----------------------------
unsigned int HEURISTIC;
//~ bool USE_EUCLIDEAN_DISTANCE_HEURISTIC;
int numberOfExpandedStates;
int MAX_MOVES;
int maxQLength;
int qLengthAfterSearch;

///////////////////////////////////////////////////////////////////////////////
LpaStar* lpa_star;
GridWorld grid_world;

bool SHOW_MAP_DETAILS;
///////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
//copy maze (from LPA*) to map (of GridWorld)
void copyMazeToDisplayMap(GridWorld &gWorld, LpaStar* lpa)
{
    for (int i = 0; i < gWorld.getGridWorldRows(); i++)
    {
        for (int j = 0; j < gWorld.getGridWorldCols(); j++)
        {
            gWorld.map[i][j].type = lpa->maze[i][j].type;
            gWorld.map[i][j].h = lpa->maze[i][j].h;
            gWorld.map[i][j].g = lpa->maze[i][j].g;
            gWorld.map[i][j].rhs = lpa->maze[i][j].rhs;
            gWorld.map[i][j].row = lpa->maze[i][j].y;
            gWorld.map[i][j].col = lpa->maze[i][j].x;

            for (int k = 0; k < 2; k++)
            {
                gWorld.map[i][j].key[k] = lpa->maze[i][j].key[k];
            }
        }
    }
    gWorld.map[lpa->start->y][lpa->start->x].h = lpa->start->h;
    gWorld.map[lpa->start->y][lpa->start->x].g = lpa->start->g;
    gWorld.map[lpa->start->y][lpa->start->x].rhs = lpa->start->rhs;
    gWorld.map[lpa->start->y][lpa->start->x].row = lpa->start->y;
    gWorld.map[lpa->start->y][lpa->start->x].col = lpa->start->x;
    for (int k = 0; k < 2; k++)
    {
        gWorld.map[lpa->start->y][lpa->start->x].key[k] = lpa->start->key[k];
    }

    gWorld.map[lpa->goal->y][lpa->goal->x].h = lpa->goal->h;
    gWorld.map[lpa->goal->y][lpa->goal->x].g = lpa->goal->g;
    gWorld.map[lpa->goal->y][lpa->goal->x].rhs = lpa->goal->rhs;
    gWorld.map[lpa->goal->y][lpa->goal->x].row = lpa->goal->y;
    gWorld.map[lpa->goal->y][lpa->goal->x].col = lpa->goal->x;
    for (int k = 0; k < 2; k++)
    {
        gWorld.map[lpa->goal->y][lpa->goal->x].key[k] = lpa->goal->key[k];
    }
}

//--------------------------------------------------------------
//copy map (of GridWorld)to maze (of LPA*)
void copyDisplayMapToMaze(GridWorld &gWorld, LpaStar* lpa) {
    for (int i = 0; i < gWorld.getGridWorldRows(); i++) {
        for (int j = 0; j < gWorld.getGridWorldCols(); j++) {
            lpa->maze[i][j].type = gWorld.map[i][j].type;
            lpa->maze[i][j].x = gWorld.map[i][j].col;
            lpa->maze[i][j].y = gWorld.map[i][j].row;

            //lpa->maze[i][j].g = gWorld.map[i][j].g;
             //lpa->maze[i][j].rhs = gWorld.map[i][j].rhs;
        }
    }

    vertex startV = gWorld.getStartVertex();
    vertex goalV = gWorld.getGoalVertex();

    //lpa->start->g = gWorld.map[startV.row][startV.col].g ;
    //lpa->start->rhs = gWorld.map[startV.row][startV.col].rhs ;
    lpa->start->x = gWorld.map[startV.row][startV.col].col;
    lpa->start->y = gWorld.map[startV.row][startV.col].row;

    //lpa->goal->g = gWorld.map[goalV.row][goalV.col].g;
    //lpa->goal->rhs = gWorld.map[goalV.row][goalV.col].rhs;
    lpa->goal->x = gWorld.map[goalV.row][goalV.col].col;
    lpa->goal->y = gWorld.map[goalV.row][goalV.col].row;
}

void drawInformationPanel(int x, int y, char* info)
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
    settextjustify(LEFT_TEXT, CENTER_TEXT);
    setcolor(YELLOW);
    outtextxy(x, y, info);
    ///////////////////////////////////////////////////////////////////////////////////////////
}

int getKey()
{
    if (GetAsyncKeyState(VK_UP) < 0)
    { //UP ARROW
        return 200;
    }

    if (GetAsyncKeyState(VK_DOWN) < 0)
    { //DOWN ARROW
        return 201;
    }

    if (GetAsyncKeyState(VK_F4) < 0)
    {
        SHOW_MAP_DETAILS = false;
        return 104;
    }

    if (GetAsyncKeyState(VK_F5) < 0) 
    {
        SHOW_MAP_DETAILS = true;
        return 105;
    }

    if (GetAsyncKeyState(VK_F6) < 0) 
    {
        //execute A* with strict expanded list
        return 106;
    }
    
    if (GetAsyncKeyState(VK_F7) < 0) 
    {
        //execute LPA*
        return 107;
    }
    
    if (GetAsyncKeyState(VK_F8) < 0) 
    {
        //execute D*Lite
        return 108;
    }

    //copy display map to algorithm's maze
    if (GetAsyncKeyState(VK_F9) < 0) 
    {
        return 109;
    }

    //copy algorithm's maze to display map
    if (GetAsyncKeyState(VK_F10) < 0) 
    {
        return 110;
    }

    if (GetAsyncKeyState(0x53) < 0) 
    { //S-key (start cell)
        return 6;
    }

    if (GetAsyncKeyState(0x58) < 0) 
    { //X-key (goal cell)
        return 7;
    }

    if (GetAsyncKeyState(0x42) < 0) 
    { //B-key (block cell)
        return 1;
    }

    if (GetAsyncKeyState(0x47) < 0) 
    {  //G-key
        return 9;
    }

    if (GetAsyncKeyState(0x48) < 0) 
    {  //H-key
        return 10;
    }

    if (GetAsyncKeyState(0x4B) < 0) 
    {  //K-key
        return 11;
    }

    if (GetAsyncKeyState(0x55) < 0) 
    { //U-key (Unblock cell)
        return 12;
    }

    if (GetAsyncKeyState(0x50) < 0) 
    { //P-key (position of cells)
        return 14;
    }

    if (GetAsyncKeyState(0x43) < 0) 
    { //C-key (connections of cells)
        return 15;
    }

    if (GetAsyncKeyState(0x4D) < 0) 
    { //M-key (entire map connections)
        return 16;
    }

    if (GetAsyncKeyState(0x52) < 0) 
    { //R-key (REINFORCEMENT LEARNING - reward values)
        return 17;
    }

    if (GetAsyncKeyState(0x51) < 0) 
    { //Q-key (REINFORCEMENT LEARNING - maxQ-values)
        return 18;
    }

    return -1;
}

void runSimulation(char *fileName)
{
    WorldBoundaryType worldBoundary;    //duplicated in GridWorld
    DevBoundaryType deviceBoundary;     //duplicated in GridWorld
    bool ANIMATE_MOUSE_FLAG = false;
    bool validCellSelected = false;
    static bool page = false;
    auto mX = 0, mY = 0;                //must be initialized, otherwise will crash when compiled with vs2015.
    float worldX, worldY;
    worldX = 0.0f;
    worldY = 0.0f;

    CellPosition p;
    int rowSelected = -1, colSelected = 1;
    int mouseRadius = 1;

    srand(static_cast<unsigned>(time(NULL)));  // Seed the random number generator

    //Initialise the world boundaries
    grid_world.initSystemOfCoordinates();

    grid_world.loadMapAndDisplay(fileName);
    grid_world.initialiseMapConnections();

    //----------------------------------------------------------------
    //LPA*
    lpa_star = new LpaStar(grid_world.getGridWorldRows(), grid_world.getGridWorldCols());
    vertex start = grid_world.getStartVertex();
    vertex goal = grid_world.getGoalVertex();

    cout << "(start.col = " << start.col    << ", start.row = " << start.row    << ")"  << endl;
    cout << "(goal.col = "  << goal.col     << ", goal.row = "  << goal.row     << ")"  << endl;

    lpa_star->initialise(start.col, start.row, goal.col, goal.row);

    //lpa_star->copyMazeToDisplayMap(grid_world);
    //copyMazeToDisplayMap(grid_world, lpa_star);
    copyDisplayMapToMaze(grid_world, lpa_star);
    //----------------------------------------------------------------

    worldBoundary = grid_world.getWorldBoundary();
    deviceBoundary = grid_world.getDeviceBoundary();
    GRIDWORLD_ROWS = grid_world.getGridWorldRows();
    GRIDWORLD_COLS = grid_world.getGridWorldCols();

    //setvisualpage(page);

    //grid_world.setMapTypeValue(5, 4 , '1');
    //grid_world.setMapTypeValue(5, 5 , '2');
    //grid_world.setMapTypeValue(5, 6 , '3');
    //grid_world.setMapTypeValue(5, 7 , '4');
    //grid_world.setMapTypeValue(5, 8 , '5');
    //grid_world.setMapTypeValue(5, 9 , '7');
    //grid_world.setMapTypeValue(5, 10, '8');
    //grid_world.setMapTypeValue(5, 11, '9');

    // keep running the program until the ESC key is pressed   
    while ((GetAsyncKeyState(VK_ESCAPE)) == 0)
    {
        setactivepage(page);
        cleardevice();
        auto action = getKey();

        if (SHOW_MAP_DETAILS)
            grid_world.displayMapWithDetails();
        else
            grid_world.displayMap();
        switch (action)
        {
        //case -1:
        //    std::cout << "dummy case\n";
        //    break;

        case 1: //Block selected cell
            if (rowSelected != -1 && colSelected != -1)
            {
                grid_world.setMapTypeValue(rowSelected - 1, colSelected - 1, '1');
                grid_world.initialiseMapConnections();

                rowSelected = -1;
                colSelected = -1;
            }
            action = -1;
            break;

        case 105:
            grid_world.displayMapWithKeyDetails();
            break;

        case 106:
            //~ algorithmSelection = ASTAR_ALGORITHM;
        {
            auto const& g = grid_world;
            State start{ g.getStartVertex().col, g.getStartVertex().row };
            State goal{ g.getGoalVertex().col, g.getGoalVertex().row };

            print("start : ", start.x, start.y);
            print("goal : ", goal.x, goal.y);

            unordered_set<State> blokeds{ };
            function<bool(Node)> validate = [&](Node const& n) {
                auto const& b = blokeds;
                auto is_not_bloked = none_of(blokeds.cbegin(), blokeds.cend(), [&](State s) {
                    return s == n.state();
                });
                return is_not_bloked && n.state().is_with_in_grid({ 0, 0 }, { 39, 39 });
            };

            AStarSEL<ManhattanDistance<Node>, decltype(validate)> astar;
            astar(Node{ "", start, goal }, validate);
            print(astar.last_run.final_path);
            auto plan = path_to_states(start, astar.last_run.final_path);
            if (!plan.empty())
                for (auto state : plan)
                    grid_world.setMapTypeValue(state.x, state.y,'1');
        }
            break;

        case 107:
            //~ algorithmSelection = LPASTAR_ALGORITHM;
            break;

        case 108:
            //~ algorithmSelection = DSTAR_ALGORITHM;
            break;

        case 15:
            if (rowSelected != -1 && colSelected != -1)
            {
                grid_world.displayVertexConnections(colSelected - 1, rowSelected - 1);
                //cout << "display connections" << endl;
                rowSelected = -1;
                colSelected = -1;
            }
            else
            {
                cout << "invalid new START vertex, please select a new START vertex first." << endl;
                break;
            }
            //--------------------------------------------
            action = -1;
            break;

        case 16:
            if (grid_world.isGridMapInitialised())
            {
                grid_world.displayMapConnections();
                //cout << "display connections" << endl;
                //~ rowSelected=-1;
                //~ colSelected=-1;
            }
            else 
            {
                cout << "map has not been initialised yet." << endl;
                break;
            }
            //--------------------------------------------
            action = -1;
            break;

        case 6: //set cell as new START vertex 
        {
            //--------------------------------------------
            // retrieve current START vertex
            vertex s = grid_world.getStartVertex();
            if ((s.row != -1) && (s.col != -1))
            {
                //set current START VERTEX to an ordinary TRAVERSABLE CELL
                grid_world.setMapTypeValue(s.row, s.col, '0');
                grid_world.initialiseMapConnections();
                //ok, proceed
            }
            else
            {
                cout << "invalid START vertex" << endl;
                break;
            }
            //--------------------------------------------
            //set selected cell as the NEW START VERTEX
            if (rowSelected != -1 && colSelected != -1)
            {
                grid_world.setMapTypeValue(rowSelected - 1, colSelected - 1, '6');
                s.row = rowSelected - 1;
                s.col = colSelected - 1;
                grid_world.setStartVertex(s);

                rowSelected = -1;
                colSelected = -1;
            }
            else 
            {
                cout << "invalid new START vertex, please select a new START vertex first." << endl;
                break;
            }
            //--------------------------------------------
            action = -1;
            break;
        }

        case 7: //set cell as new GOAL vertex 
        {
            //--------------------------------------------
            // retrieve current GOAL vertex
            vertex s = grid_world.getGoalVertex();
            if ((s.row != -1) && (s.col != -1)) 
            {
                //set current GOAL VERTEX to an ordinary TRAVERSABLE CELL
                grid_world.setMapTypeValue(s.row, s.col, '0');
                //ok, proceed
            }
            else 
            {
                cout << "invalid GOAL vertex" << endl;
                action = -1;
                break;
            }
            //--------------------------------------------
            //set selected cell as the NEW GOAL VERTEX
            if (rowSelected != -1 && colSelected != -1)
            {
                grid_world.setMapTypeValue(rowSelected - 1, colSelected - 1, '7');
                s.row = rowSelected - 1;
                s.col = colSelected - 1;
                grid_world.setGoalVertex(s);
                grid_world.initialiseMapConnections();

                rowSelected = -1;
                colSelected = -1;
            }
            else 
            {
                cout << "invalid new GOAL vertex, please select a new GOAL vertex first." << endl;
                action = -1;
                break;
            }
            //--------------------------------------------
            action = -1;
            break;
        }

        case 109:
            copyDisplayMapToMaze(grid_world, lpa_star);
            cout << "copied display map to algorithm's maze" << endl;
            action = -1;
            break;

        case 110:
            lpa_star->updateHValues();
            copyMazeToDisplayMap(grid_world, lpa_star);
            cout << "copied algorithm's maze to display map" << endl;
            action = -1;
            break;

        case 9: //display g-values only
            grid_world.displayMapWithSelectedDetails(true, false, false, false);  //(bool display_g, bool display_rhs, bool display_h, bool display_key) 
            action = -1;
            break;

        case 10: //display h-values only
            std::cout << "---------------zzz----------\n";
            grid_world.displayMapWithSelectedDetails(false, false, true, false);  //(bool display_g, bool display_rhs, bool display_h, bool display_key) 
            std::cout << "---------------zzz----------\n";
            action = -1;
            break;

        case 11: //display key-values only
            lpa_star->updateAllKeyValues();
            copyMazeToDisplayMap(grid_world, lpa_star);
            grid_world.displayMapWithSelectedDetails(false, false, false, true);  //(bool display_g, bool display_rhs, bool display_h, bool display_key) 
            action = -1;
            break;

        case 12: //make cell Traversable
            if (rowSelected != -1 && colSelected != -1)
            {
                grid_world.setMapTypeValue(rowSelected - 1, colSelected - 1, '0');
                rowSelected = -1;
                colSelected = -1;
            }
            action = -1;
            break;

        case 14:
            grid_world.displayMapWithPositionDetails();
            action = -1;
            break;

            //~ default: //Display grid without details
            //~ grid_world.displayMap();
        };

        //----------------------------------------------------------------------------------------------------------------	  
        // Mouse handling
        //
        if (mousedown())
        {
            ANIMATE_MOUSE_FLAG = true;

            mX = mousecurrentx();
            mY = mousecurrenty();

            //if the goal selected is within the playing field boundaries
            if (mX >= grid_world.getFieldX1() && mX <= grid_world.getGridMaxX() && mY >= grid_world.getFieldY1() && mY <= grid_world.getGridMaxY())
            {
                circle(mX, mY, 3);
                validCellSelected = true;
            }
            else
            {
                validCellSelected = false;
            }
        } //end of mousedown()
        //------------------------------------------------------------------------------------------------------------------
        /////////////////////////////////////////////////////////////////////////////

        if (ANIMATE_MOUSE_FLAG)
        {
            //draw Cross-hair to mark Goal	    
            setcolor(RED);
            circle(mX, mY, 20);
            line(mX, mY - 20, mX, mY + 20);
            line(mX - 20, mY, mX + 20, mY);
            //end of draw Cross-hair 

            // special effect to display concentric circles locating the target
            setcolor(YELLOW);

            if (mouseRadius < 40) ++mouseRadius;
            circle(mX, mY, mouseRadius);

            if (mouseRadius >= 40)
            {
                ANIMATE_MOUSE_FLAG = false;
                mouseRadius = 0;
            }
            //end of special effect
        }
        /////////////////////////////////////////////////////////////////////////////
        char info[80];
        //float wX, wY;
        auto wX = xWorld(worldBoundary, deviceBoundary, mX);
        auto wY = yWorld(worldBoundary, deviceBoundary, mY);
        sprintf(info, "x: %d, y: %d", mX, mY);
        drawInformationPanel(grid_world.getFieldX2(), grid_world.getFieldY1() + textheight("H") * 2, info);

        sprintf(info, "wX: %3.0f, wY: %3.0f", wX, wY);
        drawInformationPanel(grid_world.getFieldX2(), grid_world.getFieldY1() + textheight("H") * 5, info);
        ///////////////////////////////////////////////////////////////////////////// 

         //~ CellPosition p;
         //~ int rowSelected, colSelected;
        if (validCellSelected)
        {
            p = grid_world.getCellPosition_markCell(mX, mY);
            rowSelected = p.row;
            colSelected = p.col;

            sprintf(info, "row: %d, col: %d", rowSelected, colSelected);
            drawInformationPanel(grid_world.getFieldX2(), grid_world.getFieldY1() + textheight("H") * 6, info);
        }
        setvisualpage(page);
        page = !page;  //switch to another page
    }
}

int main(int argc, char *argv[])
{
    char gridFileName[80];
    if (argc == 3)
    {
        strcpy(gridFileName, argv[1]);

        if (!strcmp(argv[2], "EUCLIDEAN"))
        {
            HEURISTIC = EUCLIDEAN;
            cout << "Heuristics = EUCLIDEAN" << endl;
        }
        else if (!strcmp(argv[2], "MANHATTAN"))
        {
            HEURISTIC = MANHATTAN;
            cout << "Heuristics = MANHATTAN" << endl;
        }
    }
    else
    {
        cout << "missing parameters:  gridworld heuristic" << endl;
    }

    int graphDriver = 0, graphMode = 0;

    //initgraph(&graphDriver, &graphMode, "", 1440, 900); // Start Window
    //initgraph(&graphDriver, &graphMode, "", 1280, 1024); // Start Window

    initgraph(&graphDriver, &graphMode, "", 1360, 768); // Start Window - LAPTOP SCREEN
    //initgraph(&graphDriver, &graphMode, "", 1920, 1080); // Start Window - Full-HD

    BACKGROUND_COLOUR = WHITE;
    LINE_COLOUR = GREEN;

    GRIDWORLD_ROWS = 0; //7; //6; //duplicated in GridWorld
    GRIDWORLD_COLS = 0; //15;//13; //duplicated in GridWorld
    SHOW_MAP_DETAILS = false;

    try
    {
        runSimulation(gridFileName);
    }
    catch (...)
    {
        cout << "Exception caught!\n";
    }

    cout << "----<< The End.>>----" << endl;
    return 0;
}
