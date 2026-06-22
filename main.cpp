#include "bitboard.hpp"
#include "utils.hpp"
#include "search.hpp"
#include "types.hpp"
#include<chrono>
#include<iostream>
using namespace std;
int main(){
    Board board("c");
    int depth;
    cin>>depth;
    init_attack_tables();
    Move move = search(board,depth);
    cout<<pos_to_str(move.from)<<" "<<pos_to_str(move.to)<<" "<<(int)move.flags<<endl;
    return 0;
}   