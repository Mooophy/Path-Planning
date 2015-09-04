#include <algorithm>

#include "algorithm.h"
#include "globalvariables.h"
#include "graphics.h"

bool DEBUG_FLAG=false;

extern float WORLD_MAXX;
extern float WORLD_MAXY;



//----------------------------------------------------------------------------------------
//A*


void markCell(Robot robot, int rowIndex, int colIndex, int weight){
	

	if((rowIndex > MAXROWS-1) || (colIndex > MAXCOLS-1)){
		return;
	}
	
	int xInc = (fieldX2-fieldX1)/30;//28;
	int yInc = (fieldY2-fieldY1)/26;//28;
	
	setbkcolor(BACKGROUND_COLOUR);
	
	if(yInc < 0) {
		yInc = yInc * -1;
	}
	
	if(xInc < 0) {
		xInc = xInc * -1;
	}
	
   //setfillstyle(SOLID_FILL, weight);
   
   
	switch(weight){
		//baricade=0, space=1, fence=2, metal=3, brick=4, paper=5, wood=6, start=7, goal=8
		case 0: setfillstyle(SOLID_FILL, DARKGRAY); break;
		case 1: setfillstyle(SOLID_FILL, BACKGROUND_COLOUR); break;
		case 2: setfillstyle(CLOSE_DOT_FILL, DARKGRAY); break;
		case 3: setfillstyle(LTSLASH_FILL, BLUE); break;
		//case 4: setfillstyle(LTSLASH_FILL, BROWN); break;
		case 4: setfillstyle(CLOSE_DOT_FILL, DARKGRAY); break;
		case 7: setfillstyle(SOLID_FILL, GREEN); break; //START
		case 8: setfillstyle(SOLID_FILL, RED); break; //GOAL
		//case path: setfillstyle(SOLID_FILL, RED); break; //GOAL
		
	}
	
	if((weight != 1) && (weight != 9)){	
     //bar(fieldX1 + (xInc * colIndex)+3, fieldY1 + (yInc * rowIndex)+2, fieldX1 + ((xInc * colIndex) + xInc)-1, fieldY1 + ((yInc * rowIndex) + yInc)-1);
     bar(fieldX1 + (xInc * colIndex)+1, fieldY1 + (yInc * rowIndex), fieldX1 + ((xInc * colIndex) + xInc), fieldY1 + ((yInc * rowIndex) + yInc));
   }
   
   if(weight==7){
	   settextstyle(TRIPLEX_FONT, HORIZ_DIR, 1);
	   settextjustify(CENTER_TEXT, CENTER_TEXT);
	   setbkcolor(GREEN);
	   setcolor(LINE_COLOUR);
	   outtextxy(fieldX1 + (xInc * colIndex) + (xInc/2), fieldY1 + (yInc * rowIndex) + (yInc/2),"S");
		
	}
   
   if(weight==8){
  	   settextstyle(TRIPLEX_FONT, HORIZ_DIR, 1);
  	   settextjustify(CENTER_TEXT, CENTER_TEXT);
  	   setbkcolor(RED);
	   setcolor(LINE_COLOUR);
	   //outtextxy(fieldX1 + (xInc * colIndex) + (xInc/2), fieldY1 + (yInc * rowIndex) + (yInc/2),"G");
		outtextxy(fieldX1 + (xInc * colIndex) + (xInc/2), fieldY1 + (yInc * rowIndex) + ((yInc/2)+7),"G");
	}

   if(weight == 9){
	   setcolor(YELLOW);
	   circle(fieldX1 + (xInc * colIndex) + (xInc/2), fieldY1 + (yInc * rowIndex) + (yInc/2),3);
	}
	
}


int FindPath (Robot robot, Obstacle *obs, float target_X, float target_Y)
{
	// initialization
	int numberOfOpenListItems=0, newOpenListItemID=0;

	// get the node of the robot
	int startX, startY;
	getNodeIndexFromPos(robot.getX(), robot.getY(), robot, &startX, &startY);

	// get the node of the target
	int targetX, targetY;
	getNodeIndexFromPos(target_X, target_Y, robot, &targetX, &targetY);

	//	if starting location and target are in the same location...
	if (startX == targetX && startY == targetY)
	{
		if (pathLocation > 0)
			return FOUND;
		else if (pathLocation == 0)
			return NONEXISTENT;
	}

	// if the target is in the obstacle node, the path is non-existent
	if (checkObstacle(targetX, targetY))
		return NONEXISTENT;

	// Reset some variables that need to be cleared
	int x, y;
	for (x = 0; x < MAX_COLS + 1; x++)
		for (y = 0; y < MAX_ROWS + 1;y++)
		{
			nodeInfoList[x][y].listState = 0;		// initialization, the nodes are neither in the openlist nor in the closelist
			nodeInfoList[x][y].walkability = WALKABLE;
		}

	// mark the obstacles
	MarkObstacleNodes(obs, robot);

	// reset the path searching variables
	pathLength = Not_Started;
	pathLocation = Not_Started;
	nodeInfoList[startX][startY].gCost = 0; //reset starting node's G value to 0
	
	/****************** begin path searching ***********************/

	// add the starting node into the openlist
	numberOfOpenListItems = 1;
	openList[1] = 1;//assign it as the top (and currently only) item in the open list, which is maintained as a binary heap
	openX[1] = startX; 
	openY[1] = startY;
	
	int parentXval=0, parentYval=0;
	int path = NONEXISTENT;

	//Do the following until a path is found or deemed nonexistent.
	do
	{
		// if the open list is not empty, take the first node off the list. Because this is the lowest F cost node on the open list.
		if (numberOfOpenListItems != 0)
		{
			// Pop the first node off the open list, and record the parent indices in OpenX and OpenY array
			parentXval = openX[openList[1]];
			parentYval = openY[openList[1]]; 
			nodeInfoList[parentXval][parentYval].listState = CLOSED;	//add the node into the closed list

			/****** Delete this node from the open list ******/

			//	Delete the top item in binary heap and reorder the heap, with the lowest F cost item rising to the top.
			openList[1] = openList[numberOfOpenListItems];	// move the last item in the heap up to slot #1
			numberOfOpenListItems--;								// reduce number of open list by 1	
			
			int u=0, v=1;
			//	Repeat the following until the new node in slot #1 sinks to its proper spot in the heap.
			do
			{
				u = v;		
				if (2*u+1 <= numberOfOpenListItems) //if both children exist
				{
					//Check if the F cost of the parent is greater than each child.
					//Select the lowest of the two children.
					if (Fcost[openList[u]] >= Fcost[openList[2*u]]) 
						v = 2*u;
					if (Fcost[openList[v]] >= Fcost[openList[2*u+1]]) 
						v = 2*u+1;		
				}
				else
				{
					if (2*u <= numberOfOpenListItems) //if only child #1 exists
					{
						//Check if the F cost of the parent is greater than child #1	
						if (Fcost[openList[u]] >= Fcost[openList[2*u]]) 
							v = 2*u;
					}
				}

				if (u != v) //if parent's F is > one of its children, swap them
				{
					int temp = openList[u];
					openList[u] = openList[v];
					openList[v] = temp;			
				}
				else
					break; //otherwise, exit loop

			}
			while (1);		// resort the openlist with the lowest F value on the first position

			// Check the adjacent nodes.  Add these adjacent child nodes to the open list	for later consideration if appropriate 
			checkNode(parentXval, parentYval-1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// north
			checkNode(parentXval+1, parentYval-1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// northeast
			checkNode(parentXval+1, parentYval, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// east
			checkNode(parentXval+1, parentYval+1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// southeast
			checkNode(parentXval, parentYval+1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// south
			checkNode(parentXval-1, parentYval+1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// southwest
			checkNode(parentXval-1, parentYval, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// west
			checkNode(parentXval-1, parentYval-1, parentXval, parentYval, targetX, targetY, newOpenListItemID, numberOfOpenListItems);		// northwest
		}
		else //If open list is empty then there is no path.	
		{
			path = NONEXISTENT; 
			break;
		}  

		//If target is added to open list then path has been found.
		if (nodeInfoList[targetX][targetY].listState == OPENED)
		{
			path = FOUND; break;
		}
	}
	while (1);					//Do until path is found or deemed nonexistent

	//Save the path if it exists.
	if (path == FOUND)
	{
		// 1) Working backwards from the target to the starting location by checking	each node's parent, figure out the length of the path.
		int pathX = targetX; 
		int pathY = targetY;
		int tempx;
		do
		{
			//Look up the parent of the current node.	
			tempx = nodeInfoList[pathX][pathY].parentX;		
			pathY = nodeInfoList[pathX][pathY].parentY;
			pathX = tempx;

			//Figure out the path length
			pathLength++;
		}
		while (pathX != startX || pathY != startY);

		// 2) Resize the pathBank to the right size in bytes to store the path
		// pathBank is a one dimension array it takes two elements to store the coordinates of one node
		pathBank = (int*) realloc (pathBank, pathLength*8);

		// 3) Now copy the path information over to pathBank. Since we are
		//	working backwards from the target to the start location,copy the information to pathBank in a reverse order. 
		pathX = targetX ; pathY = targetY;
		int cellPosition = pathLength*2;	//start at the end	
		do
		{
			cellPosition = cellPosition - 2;//work backwards 2 integers
			pathBank[cellPosition] = pathX;
			pathBank[cellPosition+1] = pathY;

			//Look up the parent of the current node.	
			tempx = nodeInfoList[pathX][pathY].parentX;		
			pathY = nodeInfoList[pathX][pathY].parentY;
			pathX = tempx;
			
			markCell(robot, pathY, pathX, 5);
		}
		while (pathX != startX || pathY != startY);	//e.If we have reached the starting square, exit the loop.
	}
	return path;
}

// read the path and use it to navigate the robot
void ReadPath(Robot robot, float &worldX, float &worldY) 
{
	pathLocation = 1; //set pathLocation to 1st step
	while (pathLocation < pathLength)
	{
		int a = pathBank [pathLocation*2-2];
		int b = pathBank [pathLocation*2-1];
		pathLocation++;

		float row_step = robot.getLength();
		float col_step = WORLD_MAXX / MAXCOLS;

		// get the center of the node in the x-y coordinates
		float gX = a * col_step + col_step/2;
		float gY = (MAXROWS - 1 - b) * row_step + row_step/2;
		
		// calculate the distance from robot to the node
		float dis = sqrt(pow(gX - robot.getX(), 2) + pow(gY - robot.getY(), 2));
		
		// if the robot is too close to the node, set a new goal for the robot to make it move more smoothly
		if (dis > 5.0)				
		{
			worldX = gX;
			worldY = gY;
			break;
		}
	} 
}

// check the adjacent nodes
void checkNode(int x, int y, int parentXval, int parentYval, int targetX, int targetY, int &newOpenListItemID, int &numberOfOpenListItems)
{
	//	avoid array out-of-bounds errors
	if (!(x != -1 && y != -1 && x != MAXCOLS && y != MAXROWS))
		return;

	//	if the node is on the closed list or it is unwalkable, ignore it.
	if (nodeInfoList[x][y].listState == CLOSED || checkObstacle(x, y)) 
		return;

	//	Don't cut across corners
	int corner = WALKABLE;		
	if (x == parentXval-1) 
	{
		if (y == parentYval-1)
		{
			if (nodeInfoList[parentXval-1][parentYval].walkability == UNWALKABLE ||
				nodeInfoList[parentXval][parentYval-1].walkability == UNWALKABLE)
				corner = UNWALKABLE;
		}
		else if (y == parentYval+1)
		{
			if (nodeInfoList[parentXval][parentYval+1].walkability == UNWALKABLE ||
				nodeInfoList[parentXval-1][parentYval].walkability == UNWALKABLE) 
				corner = UNWALKABLE; 
		}
	}
	else if (x == parentXval+1)
	{
		if (y == parentYval-1)
		{
			if (nodeInfoList[parentXval][parentYval-1].walkability == UNWALKABLE ||
				nodeInfoList[parentXval+1][parentYval].walkability == UNWALKABLE) 
				corner = UNWALKABLE;
		}
		else if (y == parentYval+1)
		{
			if (nodeInfoList[parentXval+1][parentYval].walkability == UNWALKABLE || 
				nodeInfoList[parentXval][parentYval+1].walkability == UNWALKABLE)
				corner = UNWALKABLE; 
		}
	}
	
	int m = 0, addedGCost=0, tempGcost = 0;
	if (corner == WALKABLE) 
	{
		//	If not already on the open list, add it to the open list.			
		if (nodeInfoList[x][y].listState != OPENED) 
		{	
			// add the new node into the openlist
			newOpenListItemID++; 					//each new item has a unique ID #
			numberOfOpenListItems++;//add one to the number of items in the heap
			m = numberOfOpenListItems;
			openList[m] = newOpenListItemID;		//place the new open list item (actually, its ID#) at the bottom of the heap
			
			//record the x and y coordinates of the new item
			openX[newOpenListItemID] = x;
			openY[newOpenListItemID] = y;			

			//Figure out its G cost, Manhattan method is used
			if (abs(x-parentXval) == 1 && abs(y-parentYval) == 1)
				addedGCost = 14;//cost of going to diagonal squares	
			else	
				addedGCost = 10;//cost of going to non-diagonal squares				
			nodeInfoList[x][y].gCost = nodeInfoList[parentXval][parentYval].gCost + addedGCost;

			//Figure out its H and F costs and parent
			Hcost[openList[m]] = 10*(abs(x - targetX) + abs(y - targetY));
			Fcost[openList[m]] = nodeInfoList[x][y].gCost + Hcost[openList[m]];
			nodeInfoList[x][y].parentX = parentXval ; 
			nodeInfoList[x][y].parentY = parentYval;	

			//Move the new open list node to the proper place in the list.
			//Starting at the bottom, successively compare to parent items,
			//swapping as needed until the item finds its place in the heap
			//or bubbles all the way to the top.
			while (m != 1) //While node hasn't bubbled to the top (m=1)	
			{
				//Check if child's F cost is < parent's F cost. If so, swap them.	
				if (Fcost[openList[m]] <= Fcost[openList[m/2]])
				{
					int temp = openList[m/2];
					openList[m/2] = openList[m];
					openList[m] = temp;
					m = m/2;
				}
				else
					break;
			}
			nodeInfoList[x][y].listState = OPENED;			// add the node into openlist
		}
		// If adjacent node is already on the open list, check to see if this 
		//	path to that node from the starting location is a better one. 
		//	If so, change the parent of the node and its G and F costs.	
		else 
		{
			//Figure out the G cost of this possible new path
			if (abs(x-parentXval) == 1 && abs(y-parentYval) == 1)
				addedGCost = 14;//cost of going to diagonal tiles	
			else	
				addedGCost = 10;//cost of going to non-diagonal tiles				
			tempGcost = nodeInfoList[parentXval][parentYval].gCost + addedGCost;

			//If this path is shorter (G cost is lower) then change the parent node, G cost and F cost. 		
			if (tempGcost < nodeInfoList[x][y].gCost) //if G cost is less,
			{
				nodeInfoList[x][y].parentX = parentXval; 		//change the node's parent
				nodeInfoList[x][y].parentY = parentYval;
				nodeInfoList[x][y].gCost = tempGcost;			//change the G cost			

				//Because changing the G cost also changes the F cost, if the item is on the open list we need to change the node's
				//recorded F cost and its position on the open list to make sure that we maintain a properly ordered open list.
				for (int i = 1; i <= numberOfOpenListItems; i++) //look for the node in the openlist
				{
					if (openX[openList[i]] == x && openY[openList[i]] == y) 
					{
						Fcost[openList[i]] = nodeInfoList[x][y].gCost + Hcost[openList[i]];//change the F cost

						//See if changing the F score bubbles the node up from it's current location in the list
						m = i;
						while (m != 1) //While node hasn't bubbled to the top (m=1)	
						{
							//Check if child is < parent. If so, swap them.	
							if (Fcost[openList[m]] < Fcost[openList[m/2]])
							{
								int temp = openList[m/2];
								openList[m/2] = openList[m];
								openList[m] = temp;
								m = m/2;
							}
							else
								break;
						} 
						break; //exit for x = loop
					} //If openX(openList(x)) = a
				} //For x = 1 To numberOfOpenListItems
			}//If tempGcost < Gcost(a,b)
		}//else If listState(x,y) = OPENED	
	}//If not cutting a corner
}


// translate the position in the coordinates into the indices of the nodes
void getNodeIndexFromPos(float x, float y, Robot robot, int *x_Index, int *y_Index)
{	
	int col_count = int(ceil(fabs(WORLD_MAXX)/robot.getWidth()));
	float col_step = WORLD_MAXX / col_count;
	*x_Index = int(floor(x/col_step));
	
	*y_Index = int(fabs(WORLD_MAXY)/robot.getLength() - ceil(y/robot.getLength()));
}

// set the position of the obstacles unwalkable
void MarkObstacleNodes(Obstacle *obs, Robot robot)
{
	float row_step = robot.getLength();
	float col_step = WORLD_MAXX / MAXCOLS;
	
	for(int i=0; i < numOfObstacles; ++i)
	{
		int x_index, y_index;
		
		// check the positions of the four vertices, and set them unwalkable
		float *pos = obs[i].getPos();
		for (int j = 0; j < 4; ++j)
		{
			float x = pos[j*2];
			float y = pos[j*2 + 1];

			// get the index the of vertex
			getNodeIndexFromPos(x, y, robot, &x_index, &y_index);
			
			if (y_index < 0 || y_index >= MAXROWS || x_index < 0 || x_index >= MAXCOLS)
				continue;

			// set the node unwalkable
			 nodeInfoList[x_index][y_index].walkability = UNWALKABLE;
			
			/* If the the obstacle is only one node's distance to the edge, the node between the edge and the obstacle is unwalkable.
				Because this is the point where the fuzzy logic and A* conflict, this node may be on the path that A* found, but Fuzzy 
				logic will perform the obstcle avoidance. Therefore, the robot may be confused and sway around
			*/
			if (1 == x_index)
			{
				 nodeInfoList[0][y_index].walkability = UNWALKABLE;
			}
			else if (MAXCOLS - 2 == x_index)
			{
				 nodeInfoList[MAXCOLS-1][y_index].walkability = UNWALKABLE;
			}
			if (1 == y_index)
			{
				 nodeInfoList[x_index][0].walkability = UNWALKABLE;
			}
			else if (MAXROWS - 2 == y_index)
			{
				 nodeInfoList[x_index][MAXROWS - 1].walkability = UNWALKABLE;
			}
		}
	}
}

// theck the node(x, y) whether it is an obstacle
bool checkObstacle(int x, int y)
{
	return nodeInfoList[x][y].walkability == UNWALKABLE;
}

//End of A*
//--------------------------------------------------------------------------------------------------











