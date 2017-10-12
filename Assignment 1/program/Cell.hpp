struct cell{
    // g - distance between (0, 0) and this cell, h - (heuristic) distance between this cell and granny, without
    // taking into account the bear and wolf
    int g, h;
};