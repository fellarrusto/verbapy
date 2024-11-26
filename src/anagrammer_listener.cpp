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
    if (argc < 2) {
        std::cerr << "Please provide the path to the JSON file." << std::endl;
        return 1;
    }

    // Load the signature dictionary from JSON file
    std::string json_content = read_file_to_string(argv[1]);
    auto signature_dict = parse_signature_dict(json_content);

    // Initialize the Anagrammer
    Anagrammer anagrammer(signature_dict);

    // Anagrammer is loaded
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue; // Skip empty lines

        // Parse the query parameters from the input line
        std::string string_to_anagram = line; // For simplicity, assume the entire line is the string
        bool allow_partial = false; // Set this based on your requirements or parse from input

        // Find anagrams
        auto anagrams = anagrammer.find_anagrams(string_to_anagram, 10000, allow_partial);

        // Output the results
        for (const auto& anagram : anagrams) {
            std::cout << anagram << std::endl;
        }
        // Indicate the end of the response
        std::cout << "END_OF_RESULT" << std::endl;
    }

    return 0;
}