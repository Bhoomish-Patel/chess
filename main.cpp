#include "bitboard.hpp"
#include<bits/stdc++.h>
using namespace std;
int main(){
    Board board("4k3/4P3/8/8/8/8/8/4K3 w - - 0 1");
    vector<Move>legal_moves = board.generate_legal_moves();
    cout<<legal_moves.size()<<endl;
    for(auto i:legal_moves){
        cout<<i.from<<" "<<i.to<<" "<<i.flags<<endl;
    }
    return 0;
}