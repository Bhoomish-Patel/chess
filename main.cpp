#include "bitboard.hpp"
#include "utils.hpp"
#include "search.hpp"
#include<chrono>
#include<iostream>
using namespace std;
int main(){
    Board board;
    int depth;
    cin>>depth;
    auto t_start = chrono::high_resolution_clock::now();
    int node = perft(board,depth,depth);
    // board.make_move(Move(str_to_pos("b5"),str_to_pos("b7"),0));
    auto t_end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(t_end - t_start);
    cout<<"Nodes processed:"<< node<< " Time taken: " << duration.count() << " ms" << " (" << ((double)node * 1000 / (double)duration.count()) << " nodes/s)" << endl;
    return 0;
}   