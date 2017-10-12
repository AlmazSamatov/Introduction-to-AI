class Agent{
public:
    Coordinate position;
    Agent(int x, int y){
        position = {x, y};
    }
    Agent(){
        position = {0, 0};
    }
    // takes coordinates of red riding
    bool intersection(int x, int y){
        return position.x == x && position.y == y;
    }
    int getX(){
        return position.x;
    }
    int getY(){
        return position.y;
    }
    bool operator == (Agent a){
        return position.x == a.getX() && position.y == a.getY();
    }
};