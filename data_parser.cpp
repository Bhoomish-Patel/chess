#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <utility>
#include <algorithm>
#include "bitboard.hpp"
#include "moves.hpp"
#include "utils.hpp"

//copied from nnue_data_binpack_format.h 
std::int16_t unsignedToSigned(std::uint16_t r){
    std::int16_t a;
    r = (r << 15) | (r >> 1);
    if (r & 0x8000)
    {
        r ^= 0x7FFF;
    }
    std::memcpy(&a, &r, sizeof(std::uint16_t));
    return a;
}

bool hasNext(std::fstream &input_file){
    if(!input_file){
        return false;
    }
    input_file.peek();
    return !input_file.eof();
}

bool init(std::fstream &input_file){
    unsigned char header[4];
    input_file.read(reinterpret_cast<char*>(header), 4);
    if(header[0] != 'B' || header[1] != 'I' || header[2] != 'N' || header[3] != 'P'){
        return false;
    }
    return true;
}

uint32_t get_chunk_size(std::fstream &input_file){
    unsigned char header[4];
    input_file.read(reinterpret_cast<char*>(header), 4);
    uint32_t size = (header[0] | header[1] << 8 | header[2] << 16 | header[3] << 24);
    return size;
}

uint64_t big_endian_read(unsigned char data[]){
    uint64_t val = ((std::uint64_t)data[0] << 56 | (std::uint64_t)data[1] << 48 | (std::uint64_t)data[2] << 40 | (std::uint64_t)data[3] << 32 | (std::uint64_t)data[4] << 24 | (std::uint64_t)data[5] << 16 | (std::uint64_t)data[6] << 8 | (std::uint64_t)data[7]);
    return val;
} 

void set_position(Board &board, std::uint8_t sq, std::uint8_t nibble){
    if(nibble == 0){
        //white pawn 
        board.bitboard[5] |= (1ll << sq);
    }
    else if(nibble == 1){
        //black pawn
        board.bitboard[11] |= (1ll << sq);
    }
    else if(nibble == 2){
        //white knight 
        board.bitboard[4] |= (1ll << sq);
    }
    else if(nibble == 3){
        //black knight
        board.bitboard[10] |= (1ll << sq);
    }
    else if(nibble == 4){
        //white bishop
        board.bitboard[3] |= (1ll << sq);
    }
    else if(nibble == 5){
        //black bishop
        board.bitboard[9] |= (1ll << sq);
    }
    else if(nibble == 6){
        //white rook
        board.bitboard[2] |= (1ll << sq);
    }
    else if(nibble == 7){
        //black rook
        board.bitboard[8] |= (1ll << sq);
    }
    else if(nibble == 8){
        //white queen
        board.bitboard[1] |= (1ll << sq);
    }
    else if(nibble == 9){
        //black queen
        board.bitboard[7] |= (1ll << sq);
    }
    else if(nibble == 10){
        //white king
        board.bitboard[0] |= (1ll << sq);
    }
    else if(nibble == 11){
        //black king
        board.bitboard[6] |= (1ll << sq);
    }
    else if(nibble == 12){
        //enpassant
        if(sq / 8 == 3){
            board.bitboard[5] |= (1ll << sq);
            board.en_passant = (sq - 8); 
        }
        else{
            board.bitboard[11] |= (1ll << sq);
            board.en_passant = (sq + 8);
        }
    }
    else if(nibble == 13){
        //white rook with castling rights
        board.bitboard[2] |= (1ll << sq);
        if(sq == 0){
            board.castling_rights |= 1;
        }
        else{
            board.castling_rights |= 2;
        }
    }
    else if(nibble == 14){
        //black rook with castling rights
        board.bitboard[8] |= (1ll << sq);
        if(sq == 56){
            board.castling_rights |= 4;
        }
        else{
            board.castling_rights |= 8;
        }
    }
    else if(nibble == 15){
        board.bitboard[6] |= (1ll << sq);
        board.active_color = 1;
    }
}

void set_board(Board &board, unsigned char packed[]){
    for(int i = 0; i < 12; i++){
         board.bitboard[i] = 0;
    }
    board.castling_rights = 0;
    board.active_color = 0;
    board.en_passant = square_nb;
    //first 8 bytes of packed is the bitboard and remaining are the nibbles 
    //each nibble(1 byte) have info about 2 pieces 
    uint64_t occupied_pos = big_endian_read(packed);
    uint8_t nibbles[16];
    std::memcpy(nibbles, packed + 8, 16);
    std::uint8_t nibble_idx = 0;
    for(std::uint8_t i = 0; i < 64; i++){
        if((occupied_pos >> i) & 1){
            std::uint8_t nibble;
            if(nibble_idx % 2){
                nibble = (nibbles[nibble_idx / 2] >> 4);
            }
            else{
                nibble = (nibbles[nibble_idx / 2] & 0xF);
            }
            set_position(board, i, nibble);
            nibble_idx++;
        }
    }
}

void set_move(Move &move, Board board, unsigned char packed[]){
    std::uint16_t compressed_move = (packed[0] << 8 | packed[1]);
    std::uint8_t move_type = (compressed_move >> 14);
    std::uint8_t from = (compressed_move >> 8) & 0x3F;
    std::uint8_t to = (compressed_move >> 2) & 0x3F;
    std::uint8_t promotion_piece = compressed_move & 0x03;
    
    move.from = from;
    move.to = to;
    
    bool is_occupied = false;
    for(std::uint8_t i = 0; i < 12; i++){
        if((board.bitboard[i] >> to) & 1){
            is_occupied = true;
            break;
        }
    }
    if(move_type == 1){
        //promotion
        std::uint16_t promo_flag = promotion_piece + 8;
        if(is_occupied){
            promo_flag += 4;
        }
        move.flags = promo_flag;
    }
    else if(move_type == 2){
        //castle
        if(to % 8 == 0){
            move.flags = 2;
        }
        else{
            move.flags = 3;
        }
    }
    else if(move_type == 3){
        //enpassant
        move.flags = 5;
    }
    else{
        //normal 
        if(is_occupied){
            move.flags = 4;
        }
        else{
            if((std::abs(static_cast<int>(from) - static_cast<int>(to)) == 16) && get_piece_at_pos(board.bitboard, from) % 6 == 0){
                move.flags = 1;
            }
            else{
                move.flags = 0;
            }
        }
    }
}

std::vector<unsigned char> fetch_next_chunk(std::fstream &input_file){
    init(input_file);
    uint32_t chunk_size = get_chunk_size(input_file);
    std::vector<unsigned char> chunk_data(chunk_size);
    input_file.read(reinterpret_cast<char*>(chunk_data.data()),chunk_size);
    return chunk_data;
}

class MoveScoreListReader{
    public:
        unsigned char* move_score_list;
        std::uint32_t bits_left;
        std::uint32_t offset;
        MoveScoreListReader(unsigned char* move_score_list_){
            move_score_list = move_score_list_;
            bits_left = 8;
            offset = 0;
        }   
        std::uint8_t get_bits_needed(Board board){
            uint8_t cnt = 0;
            for(uint8_t i=0;i<12;i++){
                if(board.active_color != i/6)continue;
                for(uint8_t j=0;j<64;j++){
                    if((board.bitboard[i] >> j) & 1){
                        cnt++;
                    }
                }
            }
            uint8_t bits_needed = 0;
            if(cnt != 0){
                cnt--;
                for(int i=7;i>=0;i--){
                    if((cnt>>i)&1){
                        bits_needed = i + 1;
                        break;
                    }
                }
            }
            return bits_needed;
        }
        std::uint8_t get_bits_needed(std::uint8_t cnt){
            if(cnt == 0) return 0;
            cnt--;
            for(int i = 7; i >= 0; i--){
                if((cnt >> i) & 1){
                    return i + 1;
                }
            }
            return 0;
        }
        std::uint8_t read_bits(std::uint8_t count){
            if(count == 0)return count;
            if(bits_left == 0){
                offset += 1;
                bits_left = 8;
            }
            const std::uint8_t current_byte = move_score_list[offset] << (8 - bits_left);
            std::uint8_t bits = current_byte >> (8 - count);
            if(count > bits_left){
                const auto remaining_bits = count - bits_left; 
                bits |= move_score_list[offset + 1] >> (8 - remaining_bits); 
                bits_left += 8;
                offset += 1;
            }
            bits_left -= count;
            return bits;
        }
        std::uint16_t read_block(std::uint16_t blockSize){
            auto mask = (1 << blockSize) - 1;
            std::uint16_t v = 0;
            std::size_t off = 0;
            while(true){
                std::uint16_t block = read_bits(blockSize + 1);
                v |= ((block & mask) << off);
                if(!(block >> blockSize)){
                    break;
                }
                off += blockSize;
            }
            return v;
        }
        std::uint8_t get_from(Board board, std::uint8_t piece_id){ 
            std::uint8_t cnt = 0;
            std::uint8_t from = 0;
            for(uint8_t j=0;j<64;j++){
                for(uint8_t i=0;i<12;i++){
                    if(board.active_color != i/6)continue;
                    if((board.bitboard[i] >> j) & 1){
                        if(cnt == piece_id){
                            return j;
                        }
                        cnt++;
                    }
                }
            }
            return from;
        }
        Move get_move(Board board,std::uint8_t from) {
            std::uint8_t piece = get_piece_at_pos(board.bitboard, from);
            std::vector<Move> generated_moves;
            if (piece % 6 == 0) {
                board.generate_king_moves(from,generated_moves);
                while(generated_moves.back().flags == 2 || generated_moves.back().flags == 3){
                    generated_moves.pop_back();
                }
                if(board.active_color == 0){
                    if(board.castling_rights & 2){
                        generated_moves.push_back(Move(e1,c1,3));
                    }
                    if(board.castling_rights & 1){
                        generated_moves.push_back(Move(e1,g1,2));
                    }
                }
                else{
                    if(board.castling_rights & 8){
                        generated_moves.push_back(Move(e8,c8,3));
                    }
                    if(board.castling_rights & 4){
                        generated_moves.push_back(Move(e8,g8,2));
                    }
                }
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            else if (piece % 6 == 1) {
                board.generate_queen_moves(from,generated_moves);
                std::sort(generated_moves.begin(), generated_moves.end(), [](const Move& a, const Move& b) {
                    if (a.to != b.to) return a.to < b.to;
                    return a.flags < b.flags;
                });
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            else if (piece % 6 == 2) {
                board.generate_rook_moves(from,generated_moves);
                std::sort(generated_moves.begin(), generated_moves.end(), [](const Move& a, const Move& b) {
                    if (a.to != b.to) return a.to < b.to;
                    return a.flags < b.flags;
                });
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            else if (piece % 6 == 3) {
                board.generate_bishop_moves(from,generated_moves);
                std::sort(generated_moves.begin(), generated_moves.end(), [](const Move& a, const Move& b) {
                    if (a.to != b.to) return a.to < b.to;
                    return a.flags < b.flags;
                });
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            else if (piece % 6 == 4) {
                board.generate_knight_moves(from,generated_moves);
                std::sort(generated_moves.begin(), generated_moves.end(), [](const Move& a, const Move& b) {
                    if (a.to != b.to) return a.to < b.to;
                    return a.flags < b.flags;
                });
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            else if (piece % 6 == 5) {
                board.generate_pawn_moves(from,generated_moves);
                std::sort(generated_moves.begin(), generated_moves.end(), [](const Move& a, const Move& b) {
                    if (a.to != b.to) return a.to < b.to;
                    return a.flags < b.flags;
                });
                uint8_t to_idx = read_bits(get_bits_needed(generated_moves.size()));
                if(to_idx >= generated_moves.size()) return Move(-1,-1,-1);
                return generated_moves[to_idx];
            }
            return Move(-1, -1,-1);
        }
        std::pair<Move,int16_t> get_move_score(Board board){
            std::uint8_t bits_needed = get_bits_needed(board);
            std::uint8_t piece_id = read_bits(bits_needed);
            Move move = get_move(board,get_from(board,piece_id));
            std::int16_t score = unsignedToSigned(read_block(4));
            return {move,score};
        }
        uint32_t get_offset(){
            return offset + (bits_left != 8);
        }
};
class output_writer{
    public:
        std::ofstream &output_file;
        std::string buffer;
        output_writer(std::ofstream &output_file_) : output_file(output_file_){
            buffer.reserve(2048);
        }
        void write(std::string fen, std::string move, std::string score, std::string result){
            buffer += fen;
            buffer += ",";
            buffer += move;
            buffer += ",";
            buffer += score;
            buffer += ",";
            buffer += result;
            buffer += "\n";
            if(buffer.size() > 1024){
                output_file << buffer;
                buffer.clear();
            }
        }
        void flush(){
            if(!buffer.empty()){
                output_file << buffer;
                buffer.clear();
            }
        }
};
void parse_data(std::fstream &input_file, std::ofstream &output_file){
    output_writer writer(output_file);
    while(hasNext(input_file)){
        std::vector<unsigned char> chunk_data = fetch_next_chunk(input_file);
        std::uint32_t offset = 0;
        Board board;
        Move move(0,0,0);
        std::int16_t score;
        std::int16_t result;

        while(offset < chunk_data.size()){
            unsigned char pos_packed[24], move_packed[2];
            std::memcpy(pos_packed, chunk_data.data() + offset, sizeof(pos_packed));
            offset += 24;
            std::memcpy(move_packed, chunk_data.data() + offset, sizeof(move_packed));
            offset += 2;
            set_board(board, pos_packed);
            set_move(move, board, move_packed);
            score = unsignedToSigned((chunk_data[offset] << 8) | chunk_data[offset + 1]);
            offset += 2;
            std::uint16_t pr = (chunk_data[offset] << 8) | chunk_data[offset + 1];
            offset += 2;
            board.fullmove_number = pr & 0x3FFF;
            result = unsignedToSigned(pr >> 14);
            board.halfmove_clock = ((chunk_data[offset] << 8) | chunk_data[offset + 1]);
            offset += 2;
            std::string result_str = std::to_string(result);
            writer.write(board.to_fen(), pos_to_str(move.from) + pos_to_str(move.to), std::to_string(score), result_str);

            std::uint16_t num_plies = ((chunk_data[offset] << 8) | (chunk_data[offset + 1]));
            offset += 2;
            std::uint16_t num_plies_processed = 0;
            MoveScoreListReader move_score_list_reader(reinterpret_cast<unsigned char*>(chunk_data.data()) + offset);
            while(num_plies_processed < num_plies){
                board.make_move(move);
                auto move_score = move_score_list_reader.get_move_score(board);
                move = move_score.first;
                score = -score + move_score.second;
                num_plies_processed++;
                writer.write(board.to_fen(), pos_to_str(move.from) + pos_to_str(move.to), std::to_string(score), result_str);
            }
            offset += move_score_list_reader.get_offset();
        }
    }
    writer.flush();
}

int main(){
    std::string input_path = "dataset/test80-2024-01-jan-2tb7p.min-v2.v6.binpack";
    std::string output_path = "dataset/dataset.plain";
    std::fstream input_file(input_path, std::ios_base::in | std::ios_base::binary | std::ios_base::out);
    std::ofstream output_file(output_path, std::ios_base::out | std::ios::app);
    parse_data(input_file,output_file);
    return 0;
}