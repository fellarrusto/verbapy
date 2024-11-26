#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cctype>

#include "anagrammer.h"

int main(int argc, char* argv[]) {
    // Default values
    bool allow_partial = false;

    // Check command line arguments
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " vocabulary.json \"phrase to anagram\" [--allow_partial|-p]\n";
        return 1;
    }

    // Parse optional arguments
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--allow_partial" || arg == "-p") {
            allow_partial = true;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cout << "Usage: " << argv[0] << " vocabulary.json \"phrase to anagram\" [--allow_partial|-p]\n";
            return 1;
        }
    }

    // Load the signature dictionary from JSON file
    std::string json_content = read_file_to_string(argv[1]);
    auto signature_dict = parse_signature_dict(json_content);

    // Initialize the Anagrammer
    Anagrammer anagrammer(signature_dict);

    // Read the phrase to anagram
    std::string phrase = argv[2];

    // Find anagrams
    std::vector<std::string> anagrams = anagrammer.find_anagrams(phrase, 10000, allow_partial);

    // Output the anagrams
    for (const auto& anagram : anagrams) {
        std::cout << anagram << "\n";
    }

    return 0;
}