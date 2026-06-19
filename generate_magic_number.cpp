#include <iostream>
#include <vector>
#include <stdint.h>
#include <random>
#include <fstream>
using namespace std;

int RBits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

int BBits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

mt19937_64 rng(123456);

uint64_t random_uint64() {
    return rng();
}

uint64_t rook_attack(int square){
    uint64_t attacks = 0;
    int row = square / 8;
    int col = square % 8;
    for(int i = row + 1; i <= 6; i++) attacks |= (1ULL << (i * 8 + col));
    for(int i = row - 1; i >= 1; i--) attacks |= (1ULL << (i * 8 + col));
    for(int i = col + 1; i <= 6; i++) attacks |= (1ULL << (row * 8 + i));
    for(int i = col - 1; i >= 1; i--) attacks |= (1ULL << (row * 8 + i));
    return attacks;
}

uint64_t bishop_attack(int square){
    uint64_t attacks = 0;
    int row = square / 8;
    int col = square % 8;

    for(int r=row+1,c=col+1;r<=6 && c<=6;r++,c++) attacks |= 1ULL<<(r*8+c);
    for(int r=row+1,c=col-1;r<=6 && c>=1;r++,c--) attacks |= 1ULL<<(r*8+c);
    for(int r=row-1,c=col+1;r>=1 && c<=6;r--,c++) attacks |= 1ULL<<(r*8+c);
    for(int r=row-1,c=col-1;r>=1 && c>=1;r--,c--) attacks |= 1ULL<<(r*8+c);

    return attacks;
}

uint64_t rook_attack_squares(int square, uint64_t blockers){
    uint64_t attacks = 0;
    int row = square / 8;
    int col = square % 8;

    for(int i=row+1;i<8;i++){ attacks|=1ULL<<(i*8+col); if(blockers&(1ULL<<(i*8+col))) break; }
    for(int i=row-1;i>=0;i--){ attacks|=1ULL<<(i*8+col); if(blockers&(1ULL<<(i*8+col))) break; }
    for(int i=col+1;i<8;i++){ attacks|=1ULL<<(row*8+i); if(blockers&(1ULL<<(row*8+i))) break; }
    for(int i=col-1;i>=0;i--){ attacks|=1ULL<<(row*8+i); if(blockers&(1ULL<<(row*8+i))) break; }

    return attacks;
}

uint64_t bishop_attack_squares(int square, uint64_t blockers){
    uint64_t attacks = 0;
    int row = square / 8;
    int col = square % 8;

    for(int r=row+1,c=col+1;r<8&&c<8;r++,c++){ attacks|=1ULL<<(r*8+c); if(blockers&(1ULL<<(r*8+c))) break; }
    for(int r=row+1,c=col-1;r<8&&c>=0;r++,c--){ attacks|=1ULL<<(r*8+c); if(blockers&(1ULL<<(r*8+c))) break; }
    for(int r=row-1,c=col+1;r>=0&&c<8;r--,c++){ attacks|=1ULL<<(r*8+c); if(blockers&(1ULL<<(r*8+c))) break; }
    for(int r=row-1,c=col-1;r>=0&&c>=0;r--,c--){ attacks|=1ULL<<(r*8+c); if(blockers&(1ULL<<(r*8+c))) break; }

    return attacks;
}

uint64_t get_blockers(uint64_t raw_attacks, int occupancy){
    uint64_t result = 0;
    int bits = __builtin_popcountll(raw_attacks);
    for(int i=0;i<bits;i++){
        int idx = __builtin_ctzll(raw_attacks);
        raw_attacks &= raw_attacks - 1;
        if((occupancy>>i)&1) result |= 1ULL<<idx;
    }
    return result;
}

uint64_t generate_magic_number_rook(int square){
    uint64_t raw_attacks = rook_attack(square);
    uint64_t magic_number;
    while(true){
        magic_number = random_uint64() & random_uint64() & random_uint64();
        int bits = RBits[square];
        vector<uint64_t> table(1<<bits,0);
        bool is_found = true;

        for(int i=0;i<(1<<bits);i++){
            uint64_t blockers = get_blockers(raw_attacks,i);
            uint64_t attacks = rook_attack_squares(square,blockers);
            int idx = (blockers * magic_number) >> (64-bits);
            if(table[idx]==0) table[idx]=attacks;
            else if(table[idx]!=attacks){ is_found=false; break; }
        }

        if(is_found) break;
    }
    return magic_number;
}

uint64_t generate_magic_number_bishop(int square){
    uint64_t raw_attacks = bishop_attack(square);
    uint64_t magic_number;
    while(true){
        magic_number = random_uint64() & random_uint64() & random_uint64();
        int bits = BBits[square];
        vector<uint64_t> table(1<<bits,0);
        bool is_found = true;

        for(int i=0;i<(1<<bits);i++){
            uint64_t blockers = get_blockers(raw_attacks,i);
            uint64_t attacks = bishop_attack_squares(square,blockers);
            int idx = (blockers * magic_number) >> (64-bits);
            if(table[idx]==0) table[idx]=attacks;
            else if(table[idx]!=attacks){ is_found=false; break; }
        }

        if(is_found) break;
    }
    return magic_number;
}

int main(){
    vector<uint64_t> rook_magics(64), bishop_magics(64);
    vector<vector<uint64_t>> rook_table(64, vector<uint64_t>(4096));
    vector<vector<uint64_t>> bishop_table(64, vector<uint64_t>(512));

    for(int sq=0;sq<64;sq++){
        rook_magics[sq]=generate_magic_number_rook(sq);
        bishop_magics[sq]=generate_magic_number_bishop(sq);
    }

    for(int sq=0;sq<64;sq++){
        uint64_t mask=rook_attack(sq);
        for(int i=0;i<(1<<RBits[sq]);i++){
            uint64_t blockers=get_blockers(mask,i);
            int idx=(blockers*rook_magics[sq])>>(64-RBits[sq]);
            rook_table[sq][idx]=rook_attack_squares(sq,blockers);
        }
    }

    for(int sq=0;sq<64;sq++){
        uint64_t mask=bishop_attack(sq);
        for(int i=0;i<(1<<BBits[sq]);i++){
            uint64_t blockers=get_blockers(mask,i);
            int idx=(blockers*bishop_magics[sq])>>(64-BBits[sq]);
            bishop_table[sq][idx]=bishop_attack_squares(sq,blockers);
        }
    }

    ofstream f1("rook_magics.txt");
    ofstream f2("bishop_magics.txt");
    ofstream f3("rook_table.txt");
    ofstream f4("bishop_table.txt");

    for(auto x: rook_magics) f1<<x<<"\n";
    for(auto x: bishop_magics) f2<<x<<"\n";

    for(int i=0;i<64;i++)
        for(int j=0;j<4096;j++)
            f3<<rook_table[i][j]<<"\n";

    for(int i=0;i<64;i++)
        for(int j=0;j<512;j++)
            f4<<bishop_table[i][j]<<"\n";
}