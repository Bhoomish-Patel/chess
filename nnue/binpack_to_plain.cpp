// Convert a Stockfish/nnue .binpack file to a .plain text file.
//
// Build:
//   clang++ -std=c++17 -O3 -DNDEBUG -o binpack_to_plain binpack_to_plain.cpp
//
// Usage:
//   ./binpack_to_plain <input.binpack> <output.plain> [--validate] [--append]
//
// The .plain format is a sequence of records, one per training position:
//   fen <FEN>
//   move <uci move>
//   score <int>
//   ply <int>
//   result <int>
//   e

#include <ios>
#include <iostream>
#include <string>

#include "nnue_data_binpack_format.h"

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <input.binpack> <output.plain> [--validate] [--append]\n";
        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    bool validate = false;
    std::ios_base::openmode om = std::ios_base::out | std::ios_base::trunc;

    for (int i = 3; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--validate")
        {
            validate = true;
        }
        else if (arg == "--append")
        {
            om = std::ios_base::out | std::ios_base::app;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << '\n';
            return 1;
        }
    }

    try
    {
        binpack::convertBinpackToPlain(inputPath, outputPath, om, validate);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
