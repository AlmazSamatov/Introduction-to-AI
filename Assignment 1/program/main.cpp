#include "Game.hpp"
#include <chrono>

#define NUMBER_OF_TESTS 1000

using namespace std;

typedef std::ratio<1l, 1000000000l> nano;
typedef std::chrono::duration<long long, nano> nanoseconds;

// This code requires C++11
int main(){

	Game game;

    // Test case when granny in other side of board and to go to her we should intersect bear range
    /*game.woodCutterWork = WoodCutter(7, 6);
    game.woodCutterHouse = WoodCutter(7, 8);
    game.woodCutter = WoodCutter(7, 6);
    game.granny = Granny(8, 7);
    game.bear = Bear(7, 4);
    game.wolf = Wolf(6, 7);

    game.board.getCell(6, 3).setBear();
    game.board.getCell(6, 4).setBear();
    game.board.getCell(6, 5).setBear();
    game.board.getCell(7, 3).setBear();
    game.board.getCell(7, 4).setBear();
    game.board.getCell(7, 5).setBear();
    game.board.getCell(8, 3).setBear();
    game.board.getCell(8, 4).setBear();
    game.board.getCell(8, 5).setBear();

    game.board.getCell(4, 7).setWolf();
    game.board.getCell(5, 6).setWolf();
    game.board.getCell(5, 7).setWolf();
    game.board.getCell(5, 8).setWolf();
    game.board.getCell(6, 7).setWolf();

    game.board.getCell(8, 7).setGranny();

    game.board.getCell(7, 8).setWoodCutterHouse();
    game.board.getCell(7, 6).setWoodCutterWork();
    game.board.getCell(7, 6).setWoodCutterPlaceHere();

    game.board.estimateBearCellsCost(game.bear, game.wolf);*/

    int numberOfWinsAStar = 0;
    int numberOfWinsBackTracking = 0;
    int numberOfStepsAStar = 0;
    int numberOfStepsBackTracking = 0;
    nanoseconds timeAStar(0);
    nanoseconds timeBackTracking(0);

    for(int i = 0; i < NUMBER_OF_TESTS; i++){
        game.createNewBoard();

        auto start = std::chrono::high_resolution_clock::now();
        game.backTracking();
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        if(game.win){
            numberOfWinsBackTracking++;
            numberOfStepsBackTracking += game.numberOfSteps;
            timeBackTracking += std::chrono::duration_cast<nanoseconds>(elapsed);
        }

        start = std::chrono::high_resolution_clock::now();
        game.aStar();
        elapsed = std::chrono::high_resolution_clock::now() - start;

        if(game.win){
            numberOfWinsAStar++;
            numberOfStepsAStar += game.numberOfSteps;
            timeAStar += std::chrono::duration_cast<nanoseconds>(elapsed);
        }
    }

    cout << "Number of wins in A*: " << numberOfWinsAStar << endl;
    cout << "Number of wins in BackTracking: " << numberOfWinsBackTracking << endl;

    cout << "Number of fails in A*: " << NUMBER_OF_TESTS - numberOfWinsAStar << endl;
    cout << "Number of fails in BackTracking: " << NUMBER_OF_TESTS - numberOfWinsBackTracking << endl;

    cout << "Average steps in A*: " << numberOfStepsAStar / numberOfWinsAStar << endl;
    cout << "Average steps in Backtracking: " << numberOfStepsBackTracking / numberOfWinsBackTracking << endl;

    cout << "Average time until solution of A*: " << double(timeAStar.count() / numberOfWinsAStar) << " ns" << endl;
    cout << "Average time until solution of BackTracking: " <<
         double(timeBackTracking.count() / numberOfWinsBackTracking) << " ns" << endl;

    return 0;
}