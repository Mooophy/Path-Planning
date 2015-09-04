#include <stdio.h>
#include <iostream>
#include <stdlib.h>     /* calloc, exit, free */
#include <math.h>  //sqrt, pow

#include "LPAstar.h"
#include "gridworld.h"


 LpaStar::LpaStar(int rows_, int cols_){
		 rows = rows_;
	    cols = cols_;
	 
		 //Allocate memory 
		 maze.resize(rows);
		 for(int i=0; i < rows; i++){
		   maze[i].resize(cols);
		 }
}

void LpaStar::initialise(int startX, int startY, int goalX, int goalY){
	for(int i=0; i < rows; i++){
	   for(int j=0; j < cols; j++){
		   maze[i][j].g = INF;
			maze[i][j].rhs = INF;
		}
	}
	start = new LpaStarCell;
	goal = new LpaStarCell;
	
	//START VERTEX
	start->g = INF;
	start->rhs = 0.0;
	start->x = startX;
	start->y = startY;
	
	//GOAL VERTEX
	goal->g = INF;
	goal->rhs = INF;
	goal->x = goalX;
	goal->y = goalY;
	//---------------------
	maze[start->y][start->x].g = start->g;
	maze[start->y][start->x].rhs = start->rhs;
	
	maze[goal->y][goal->x].g = goal->g;
	maze[goal->y][goal->x].rhs = goal->rhs;
	//---------------------
	
	//for debugging only
	//~ for(int i=0; i < rows; i++){
	   //~ for(int j=0; j < cols; j++){
		   //~ //cout << maze[i][j].g << ", ";
			//~ cout << maze[i][j].rhs << ", ";
			
		//~ }
		//~ cout << endl;
	//~ }
	
}

double LpaStar::minValue(double g_, double rhs_){
	if(g_ <= rhs_){
		return g_;
	} else {
		return rhs_;
	}	
}

int LpaStar::maxValue(int v1, int v2){
	
	if(v1 >= v2){
		return v1;
	} else {
		return v2;
	}	
}

double LpaStar::calc_H(int x, int y){
	
	int diffY = abs(goal->y - y);
	int diffX = abs(goal->x - x);
	
	//maze[y][x].h = (double)maxValue(diffY, diffX);
	return (double)maxValue(diffY, diffX);
}

void LpaStar::updateHValues(){
	for(int i=0; i < rows; i++){
	   for(int j=0; j < cols; j++){
		   maze[i][j].h = calc_H(j, i);
		}
	}
	
	start->h = calc_H(start->x, start->y);
	goal->h = calc_H(goal->x, goal->y);
}

void LpaStar::calcKey(int x, int y){
	double key1, key2;
	
	key2 = minValue(maze[y][x].g, maze[y][x].rhs);
	key1 = key2 + maze[y][x].h;
}


void LpaStar::calcKey(LpaStarCell *cell){
	double key1, key2;
	
	key2 = minValue(cell->g, cell->rhs);
	key1 = key2 + cell->h;
	
	cell->key[0] = key1;
	cell->key[1] = key2;
}

void LpaStar::updateAllKeyValues(){	
	for(int i=0; i < rows; i++){
	   for(int j=0; j < cols; j++){
		   calcKey(&maze[i][j]);
		}
	}
	
	calcKey(start);
	calcKey(goal);
}




