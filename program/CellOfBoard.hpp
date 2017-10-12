class CellOfBoard{
public:
    bool visited;
	bool isEmpty;
	bool isWolf; // There is no bear range and wolf range, just bear and wolf,
	bool isBear; // because they do the same thing
	bool isGranny;
	bool isWoodCutterWork;
	bool isWoodCutterHouse;
    bool isWoodCutterHere; // whether wood cutter in this cell (in home or in work)

    int cost; // need when we go to the bear range, because in A* we should go there last
    // 0 - for all cells except wolf and his range,
    // 100 - for bear cells, in which we can go and not fail with 100% probability

	CellOfBoard(){
        visited = false;
		isEmpty = true;
		isBear = false;
		isWolf = false;
		isGranny = false;
		isWoodCutterHouse = false;
		isWoodCutterWork = false;
        cost = 0;
	}
	void setWolf(){
		isWolf = true;
		isEmpty = false;
        visited = true; // we should not go to the cell with wolf or with its range, because we will fail
	}
	void setBear(){
		isBear = true;
		isEmpty = false;
	}
	void setGranny(){
		isGranny = true;
		isEmpty = false;
	}
	void setWoodCutterHouse(){
		isWoodCutterHouse = true;
		isEmpty = false;
	}
	void setWoodCutterWork(){
		isWoodCutterWork = true;
		isEmpty = false;
	}
    void setWoodCutterPlaceHere(){
        isWoodCutterHere = true;
    }
};