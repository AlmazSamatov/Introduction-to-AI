struct comparator{
    bool operator() (const pair < cell, Coordinate >& p1, const pair < cell, Coordinate >& p2){
        int f1 = p1.first.g + p1.first.h;
        int f2 = p2.first.g + p2.first.h;
        return f1 > f2;
    }
};