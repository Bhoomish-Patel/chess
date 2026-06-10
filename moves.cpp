#include "moves.hpp"
#include <cstdint>

Move::Move(uint8_t from, uint8_t to, uint8_t flags){
    this->from = from;
    this->to = to;
    this->flags = flags;
}