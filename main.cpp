#include "bitboard.hpp"
#include<bits/stdc++.h>
using namespace std;
int main(){
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.generate_legal_moves();
    return 0;
}