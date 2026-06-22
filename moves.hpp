#ifndef MOVES_HPP
#define MOVES_HPP

#include <cstdint>
class Move{
    public:
        uint8_t from;
        uint8_t to;
        uint8_t flags;
        Move(uint8_t from, uint8_t to, uint8_t flags);
        Move();
};
#endif