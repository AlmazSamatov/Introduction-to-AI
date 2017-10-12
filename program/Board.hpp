#define WIDTH 9 // default values of board for our task
#define HEIGHT 9

using namespace std;

class Board{
    vector < vector < CellOfBoard > > board;
public:
	
	Board(){
		board = vector < vector < CellOfBoard > >(WIDTH, vector < CellOfBoard > (HEIGHT));
	}
	
	// function for accessing elements of board
	CellOfBoard& getCell (int i, int j){
		return board[i][j];
	}

	// function for accessing elements of board
	CellOfBoard& getCell (Coordinate c){
		return board[c.x][c.y];
	}

    void print(RedRidingHood redRidingHood){
        for(int i = 0; i < HEIGHT; i++){
            for(int j = 0; j < WIDTH; j++){
                CellOfBoard cell = board[i][j];
                if(i == redRidingHood.getX() && j == redRidingHood.getY())
                    cout << 'R'; // Red Riding Hood
                else if(cell.isWolf)
                    cout << 'W'; // Wolf
                else if(cell.isBear)
                    cout << 'B'; // Bear
                else if(cell.isGranny)
                    cout << 'G'; // Granny
                else if(cell.isWoodCutterHouse)
                    cout << 'H'; // house of wood cutter
                else if(cell.isWoodCutterWork)
                    cout << 'F'; // work of wood cutter (forest)
                else
                    cout << '.'; // empty cell
            }
            cout << endl;
        }
        cout << "---------" << endl;
    }

    bool haveUnvisitedCells(){
        for(int i = 0; i < HEIGHT; i++){
            for(int j = 0; j < WIDTH; j++){
                CellOfBoard cell = board[i][j];
                if(!cell.visited && !cell.isBear && !cell.isWolf){
                    return true;
                }
            }
        }
        return false;
    }

    void estimateBearCellsCost(Bear bear, Wolf wolf){

        // check that it is possible to go to bear cells and save at least 2 points
        // calculate square of distance between bear and wolf
        int xSquare = abs(bear.getX() - wolf.getX()) * abs(bear.getX() - wolf.getX());
        int ySquare = abs(bear.getY() - wolf.getY()) * abs(bear.getY() - wolf.getY());
        int dist = xSquare + ySquare;

        // we can go to bear cells and save at least 2 points only if distance between bear and wolf is 10 or 13
        if(dist == 10 || dist == 13){
            // here we assign costs of 2 or 1 bear range cells to 100 and other bear cells mark as visited
            // we assign cost to 100 if only near we have 1 wolf range cell
            for(int i = 0; i < HEIGHT; i++){
                for(int j = 0; j < WIDTH; j++){
                    if(board[i][j].isBear) {
                        // check up, down, left, right and diagonal cells
                        int wolfRangeNear = 0;
                        for (int y = -1; y <= 1; y += 2) {
                            if (isValidCoordinate(i + y, j) && board[i + y][j].isWolf) {
                                wolfRangeNear++;
                            }
                            if (isValidCoordinate(i, j + y) && board[i][j + y].isWolf) {
                                wolfRangeNear++;
                            }
                            if (isValidCoordinate(i + y, j + y) && board[i + y][j + y].isWolf) {
                                wolfRangeNear++;
                            }
                            if (isValidCoordinate(i + y, j - y) && board[i + y][j - y].isWolf) {
                                wolfRangeNear++;
                            }
                        }

                        // whether near 1 wolf range cell
                        if (wolfRangeNear == 1)
                            board[i][j].cost = 100;
                        else
                            board[i][j].visited = true;
                    }
                }
            }
        } else{
            // if distance is other than 10 or 13 then we mark bear and his range as visited
            for(int i = 0; i < HEIGHT; i++){
                for(int j = 0; j < WIDTH; j++){
                    if(board[i][j].isBear)
                        board[i][j].visited = true;
                }
            }
        }
    }

    // assign false to visited attributes to all cells of board
    void setCellsToUnvisited() {
        for(int i = 0; i < HEIGHT; i++){
            for(int j = 0; j < WIDTH; j++){
                // cells to which we can go is empty cells, granny, wood cutter house, wood cutter work and
                // bear cells with cost 1000
                if(board[i][j].isBear && board[i][j].cost == 100)
                    board[i][j].visited = false;
                else if(!board[i][j].isWolf)
                    board[i][j].visited = false;
            }
        }
    }

    bool isValidCoordinate(int x, int y){
        return x >= 0 && x < HEIGHT && y >= 0 && y < WIDTH;
    }
};