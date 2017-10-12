#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <iostream>
#include "Cell.hpp"
#include "Coordinate.hpp"
#include "Agent.hpp"
#include "CellOfBoard.hpp"
#include "RedRidingHood.hpp"
#include "Granny.hpp"
#include "Bear.hpp"
#include "Wolf.hpp"
#include "WoodCutter.hpp"
#include "Board.hpp"
#include "Comparator.hpp"

class Game{

	bool isValidCoordinate(int x, int y){
		return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH;
	}

	// returns random empty cell
	int getRandomEmptyCell(){
		int cell = rand() % 81 + 1;
		int j = 0;
		// loop for iterate through map and find allowed place
		while(true){
			// except cells, which are occupied
			if(board.getCell(j % 9, j / 9).isEmpty)
				cell--;
			if(cell == 0)
				break;
			j++;
			if(j == 81)
				j = 0;
		}
		return j;
	}

    void checkCell(int x, int y){
        if(board.getCell(x, y).isWolf)
            redRidingHood.intersectionWithWolfRange();
        if(board.getCell(x, y).isBear)
            redRidingHood.intersectionWithBearRange();
        if(woodCutter.intersection(x, y))
            redRidingHood.getPointsFromWoodCutter();
        win = granny.intersection(redRidingHood);
    }

	// calculates square of closest (without wolf or bear) distance to goal
	int distToGoal(int x, int y, Agent goal){
        int xSquare = abs(x - goal.getX()) * abs(x - goal.getX());
        int ySquare = abs(y - goal.getY()) * abs(y - goal.getY());
        return xSquare + ySquare;
	}

    void dfs(int x, int y){

        if(!win && redRidingHood.isAlive) {

            redRidingHood.position.x = x;
            redRidingHood.position.y = y;

            if(anotherSideOfBoard)
                setOfVisitedCells.push_back({x, y});

            //board.print(redRidingHood);

            checkCell(x, y);

            numberOfSteps++;

            board.getCell(x, y).visited = true;

            if (isValidCoordinate(x, y + 1) && !board.getCell(x, y + 1).visited
                && !board.getCell(x, y + 1).isBear) {
                // go right
                dfs(x, y + 1);
            }
            if (isValidCoordinate(x + 1, y) && !board.getCell(x + 1, y).visited
                    && !board.getCell(x + 1, y).isBear) {
                // go down
                dfs(x + 1, y);
            }
            if (isValidCoordinate(x, y - 1) && !board.getCell(x, y - 1).visited
                    && !board.getCell(x, y - 1).isBear) {
                // go left
                dfs(x, y - 1);
            }
            if (isValidCoordinate(x - 1, y) && !board.getCell(x - 1, y).visited
                    && !board.getCell(x - 1, y).isBear) {
                // go up
                dfs(x - 1, y);
            }
        }
    }

public:

    RedRidingHood redRidingHood;
    Granny granny;
    Bear bear;
    Wolf wolf;
    WoodCutter woodCutter;
    WoodCutter woodCutterHouse;
    WoodCutter woodCutterWork;

    int numberOfSteps;
    bool win; // whether we can go to granny with 6 points
    Board board;
    bool anotherSideOfBoard;
    vector < Coordinate > setOfVisitedCells;

	void backTracking(){

        setAllDefault();
        anotherSideOfBoard = false;
        //cout << "Backtracking: " << endl;
        dfs(0, 0);
        if(!win && board.haveUnvisitedCells()){
            board.estimateBearCellsCost(bear, wolf);
            bool canGoToBearRange = false;
            for(int i = 0; i < HEIGHT; i++){
                for(int j = 0; j < WIDTH; j++){
                    if(board.getCell(i, j).isBear && board.getCell(i, j).cost > 0){
                        canGoToBearRange = true;
                    }
                }
            }
            if(canGoToBearRange){
                int x = -1, y = -1;
                for(int i = 0; i < HEIGHT; i++){
                    for(int j = 0; j < WIDTH; j++){
                        if(board.getCell(i, j).isBear && board.getCell(i, j).cost > 0){
                            // we simulate going to bear cells, because need to calculate number of bear cells,
                            // that we will pass, and number of points that we will lose
                            for(int e = -1; e <= 1; e+=2){
                                // try to find cells to which we can go from bear range
                                if(isValidCoordinate(i + e, j) && !board.getCell(i + e, j).visited
                                        && !board.getCell(i + e, j).isBear){
                                    x = i + e;
                                    y = j;
                                    break;
                                }
                                if(isValidCoordinate(i, j  + e) && !board.getCell(i, j + e).visited
                                   && !board.getCell(i, j + e).isBear){
                                    x = i;
                                    y = j + e;
                                    break;
                                }
                            }
                            board.getCell(i, j).visited = true; // mark as visited, that we will not go there
                            numberOfSteps++;
                            redRidingHood.intersectionWithBearRange();
                        }
                        if(x != -1 && y != -1)
                            break;
                    }
                    if(x != -1 && y != -1)
                        break;
                }
                anotherSideOfBoard = true;
                dfs(x, y);
                // if we dont win then it means that we get all points, but path to granny was closed by
                // visited cells
                // and we need to mark unvisited all cells and try to run our algorithm again
                if(!win){
                    clearArrayAndMarkUnvisited(setOfVisitedCells);
                    dfs(x, y);
                }
            }
        }
    }

	void aStar(){

        setAllDefault();
        // estimate costs to go to bear cells
        board.estimateBearCellsCost(bear, wolf);

        //cout << endl << "A*:" << endl;

        //heuristic function is f(x) = g(x) + h(x)
        //farther I will name them as f, g and h
        //g = (last cell g) + 1, h = distance to goal (granny or woodcutter), depends on circumstances + cost of cell
        // (0 - for all cells except bear cells, where we can go and save at least 2 points,
        // 100 - for bear cells, in which we can go and not fail with 100% probability)
        // and we will never go to wolf cell, because then we will fail and we consider wolf and his range
        // (they are same in my implementation, because they do the same thing) as wall

        priority_queue < pair < cell, Coordinate >, vector <  pair < cell, Coordinate > >, comparator > pq;
        Agent goal = granny;
        pq.push({{0, distToGoal(0, 0, goal)}, Coordinate{0,0}}); // push 1-st coordinate (0,0)

        vector < Coordinate > setOfVisitedCells;

        while(!win && redRidingHood.isAlive && !pq.empty() && redRidingHood.getPoints() > 0) {

            Coordinate c = pq.top().second;
            int lastCellG = pq.top().first.g;
            int g = lastCellG + 1;

            pq.pop();

            int x = c.x;
            int y = c.y;

            if(!board.getCell(c).visited) {

                redRidingHood.position = c;

                checkCell(x, y);

                //board.print(redRidingHood);

                numberOfSteps++;

                board.getCell(x, y).visited = true;

                // if we lost some points then red riding hood should go to wood cutter
                if (!board.getCell(x, y).isBear && redRidingHood.getPoints() < 6 && goal == granny) {
                    // if we are here, then it means that now we are at other side of board
                   if(!board.getCell(woodCutterHouse.position).visited
                      && !board.getCell(woodCutterWork.position).visited){
                       if(distToGoal(x, y, woodCutterHouse) < distToGoal(x, y, woodCutterWork)){
                           goal = woodCutterHouse;
                       } else{
                           goal = woodCutterWork;
                       }
                   } else if(!board.getCell(woodCutterHouse.position).visited){
                       goal = woodCutterHouse;
                   } else if(!board.getCell(woodCutterWork.position).visited){
                       goal = woodCutterWork;
                   }
                }

                // insert all cells, which we visit, when we go to wood cutter house or work, to special set
                if (goal == woodCutterHouse || goal == woodCutterWork)
                    setOfVisitedCells.push_back(c);

                // if we are in one of goal cells, where can be wood cutter
                if (x == goal.getX() && y == goal.getY() && (goal == woodCutterHouse || goal == woodCutterWork)
                    && redRidingHood.getPoints() < 6) {
                    if (goal == woodCutterWork && !board.getCell(woodCutterHouse.position).visited) {
                        clearArrayAndMarkUnvisited(setOfVisitedCells);
                        goal = woodCutterHouse;
                    } else if (goal == woodCutterWork && !board.getCell(woodCutterHouse.position).visited) {
                        clearArrayAndMarkUnvisited(setOfVisitedCells);
                        goal = woodCutterWork;
                    }
                }

                // if we got from wood cutter all points then go to granny
                // but initially we need to mark unvisited all cells that we mark visited, when we
                // search wood cutter, beacause otherwise we just can not go to granny (there will not path to granny)
                if ((goal == woodCutterWork || goal == woodCutterHouse) && redRidingHood.getPoints() == 6) {
                    clearArrayAndMarkUnvisited(setOfVisitedCells);
                    goal = granny;
                }

                // check if we could go to neighbour cells, and if yes then push them to priority queue
                if (isValidCoordinate(x, y + 1) && !board.getCell(x, y + 1).visited) {
                    pq.push({{g, distToGoal(x, y + 1, goal) + board.getCell(x, y + 1).cost}, Coordinate{x, y + 1}});
                }
                if (isValidCoordinate(x + 1, y) && !board.getCell(x + 1, y).visited) {
                    pq.push({{g, distToGoal(x + 1, y, goal) + board.getCell(x + 1, y).cost}, Coordinate{x + 1, y}});
                }
                if (isValidCoordinate(x, y - 1) && !board.getCell(x, y - 1).visited) {
                    pq.push({{g, distToGoal(x, y - 1, goal) + board.getCell(x, y - 1).cost}, Coordinate{x, y - 1}});
                }
                if (isValidCoordinate(x - 1, y) && !board.getCell(x - 1, y).visited) {
                    pq.push({{g, distToGoal(x - 1, y, goal) + board.getCell(x - 1, y).cost}, Coordinate{x - 1, y}});
                }
            }
        }
	}

    void clearArrayAndMarkUnvisited(vector <Coordinate> & setOfVisitedCells){
        for(int i = 0; i < setOfVisitedCells.size(); i++){
            board.getCell(setOfVisitedCells[i]).visited = false;
            //cout << setOfVisitedCells[i].x << ' ' << setOfVisitedCells[i].y << endl;
        }
        setOfVisitedCells.clear();
    }

    // method for set all attributes to default value, that we can run another algorithm on it
    void setAllDefault(){
        numberOfSteps = 0;
        win = false;
        board.setCellsToUnvisited();
    }

    // method for initializing new board
    void createNewBoard(){

        setAllDefault();

        board = Board();
        redRidingHood = RedRidingHood();

        // mark cell (0, 0) that it is not empty, because there are Red Riding Hood appear at start of game
        board.getCell(0, 0).isEmpty = false;
        // place wolf
        int cell = rand() % 81 + 1;
        int place = 0;
        // loop for iterate through map and find allowed place for wolf
        while(true){
            // except cells, where it can not be placed,
            if(place != 1 && place != 9 && board.getCell(place % 9, place / 9).isEmpty)
                cell--;
            if(cell == 0)
                break;
            place++;
            if(place == 81)
                place = 0;
        }
        int wolf_X = place % 9;
        int wolf_Y = place / 9;
        wolf = Wolf(wolf_X, wolf_Y);
        // mark place of wolf in map
        board.getCell(wolf_X, wolf_Y).setWolf();
        // place wolf range
        for(int j = -1; j <= 1; j+=2) {
            if (isValidCoordinate(wolf_X + j, wolf_Y))
                board.getCell(wolf_X + j, wolf_Y).setWolf();
            if (isValidCoordinate(wolf_X, wolf_Y + j))
                board.getCell(wolf_X, wolf_Y + j).setWolf();
        }

        // place bear
        cell = rand() % 81 + 1;
        // loop for iterate through map and find allowed place for bear
        place = 0;
        while(true){
            // except cells, where it can not be placed, because otherwise it will kill Red Riding Hood at the beginning of game
            if(place != 9 && place != 10 && place != 1 && board.getCell(place % 9, place / 9).isEmpty)
                cell--;
            if(cell == 0)
                break;
            place++;
            if(place == 81)
                place = 0;
        }
        int bear_X = place % 9;
        int bear_Y = place / 9;
        bear = Bear(bear_X, bear_Y);
        // mark place of bear in map
        board.getCell(bear_X, bear_Y).setBear();
        // place bear range
        for(int j = -1; j <= 1; j+=2){
            if(isValidCoordinate(bear_X+j, bear_Y))
                board.getCell(bear_X+j, bear_Y).setBear();
            if(isValidCoordinate(bear_X, bear_Y+j))
                board.getCell(bear_X, bear_Y+j).setBear();
            if(isValidCoordinate(bear_X+j, bear_Y+j))
                board.getCell(bear_X+j, bear_Y+j).setBear();
            if(isValidCoordinate(bear_X+j, bear_Y-j))
                board.getCell(bear_X+j, bear_Y-j).setBear();
        }

        // place granny
        place = getRandomEmptyCell();
        int granny_X = place % 9;
        int granny_Y = place / 9;
        // mark place of granny in map
        board.getCell(granny_X, granny_Y).setGranny();

        // place wood cutter house
        place = getRandomEmptyCell();
        int woodCutterHouse_X = place % 9;
        int woodCutterHouse_Y = place / 9;
        // mark place of wood cutter house in map
        board.getCell(woodCutterHouse_X, woodCutterHouse_Y).setWoodCutterHouse();

        // place wood cutter work
        place = getRandomEmptyCell();
        int woodCutterWork_X = place % 9;
        int woodCutterWork_Y = place / 9;
        // mark place of wood cutter work in map
        board.getCell(woodCutterWork_X, woodCutterWork_Y).setWoodCutterWork();

        // define whether wood cutter in home or in work
        int isWoodCutterInHouse = rand() % 2;
        if(isWoodCutterInHouse){
            board.getCell(woodCutterHouse_X, woodCutterHouse_Y).setWoodCutterPlaceHere();
            woodCutter = WoodCutter(woodCutterHouse_X, woodCutterHouse_Y);
        } else{
            board.getCell(woodCutterWork_X, woodCutterWork_Y).setWoodCutterPlaceHere();
            woodCutter = WoodCutter(woodCutterWork_X, woodCutterWork_Y);
        }

        granny = Granny(granny_X, granny_Y);
        woodCutterHouse = WoodCutter(woodCutterHouse_X, woodCutterHouse_Y);
        woodCutterWork = WoodCutter(woodCutterWork_X, woodCutterWork_Y);
    }
};