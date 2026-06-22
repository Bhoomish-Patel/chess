#include "moves.hpp"
#include <cstdint>

Move::Move(uint8_t from, uint8_t to, uint8_t flags){
    this->from = from;
    this->to = to;
    this->flags = flags;
}
Move::Move(){
    this->from = 0;
    this->to = 0;
    this->flags = 0xFF;
}