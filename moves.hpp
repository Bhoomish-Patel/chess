#ifndef MOVES_HPP
#define MOVES_HPP

class Move{
    public:
        int from;
        int to;
        int flags;
        Move(int from,int to,int flags);
};
#endif