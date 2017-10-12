class Granny: public Agent{
public:
    Granny(int x, int y): Agent(x, y) {}
    Granny(): Agent() {}
    bool intersection(RedRidingHood redRidingHood){
        return redRidingHood.getX() == position.x && redRidingHood.getY() == position.y
               && redRidingHood.getPoints() == 6;
    }
};