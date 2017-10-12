class RedRidingHood: public Agent{
	int points;
public:
	bool isAlive;

	RedRidingHood(): Agent(0, 0) {
		isAlive = 1;
		points = 6;
	}

    void getPointsFromWoodCutter(){
        if(points < 6)
            points = 6;
    }

	void intersectionWithBearRange(){
		points-=2;
		if(points == 0)
			isAlive = 0;
	}

	void intersectionWithWolfRange(){
		points = 0;
		isAlive = 0;
	}

    int getPoints(){
        return points;
    }
};