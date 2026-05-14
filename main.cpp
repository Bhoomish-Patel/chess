#include "bitboard.hpp"
#include "utils.hpp"
#include "search.hpp"
#include<chrono>
#include<bits/stdc++.h>
using namespace std;
int main(){
    Board board("8/6k1/R7/1Rp5/ppp5/5pp1/4pp2/7K w - - 0 1");
    auto start = chrono::high_resolution_clock::now();
    Move move = search(board,4);
    // board.make_move(Move(str_to_pos("b5"),str_to_pos("b7"),0));
    cout<<"Final move "<<pos_to_str(move.from)<<" "<<pos_to_str(move.to)<<endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Time taken: " << duration.count() << " ms" << endl;
    return 0;
}