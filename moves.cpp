#include "moves.hpp"

Move::Move(int from,int to,int flags){
    this->from = from;
    this->to = to;
    this->flags = flags;
}