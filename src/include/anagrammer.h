// anagrammer.h

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cctype>


using namespace std;

// Base64 decoding function
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::vector<unsigned char> base64_decode(const std::string& encoded_string) {
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(static_cast<unsigned char>(encoded_string[in_]))) {
        char_array_4[i++] = static_cast<unsigned char>(encoded_string[in_]); in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(base64_chars.find(static_cast<char>(char_array_4[i])));

            char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
            char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
            char_array_3[2] = static_cast<unsigned char>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        int j;
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = static_cast<unsigned char>(base64_chars.find(static_cast<char>(char_array_4[j])));

        char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
        char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
        char_array_3[2] = static_cast<unsigned char>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

        for (j = 0; (j < i - 1); j++)
            ret.push_back(char_array_3[j]);
    }

    return ret;
}

// Function to strip accents from a string
char remove_accent(char c) {
    // Simple mapping for accented characters to their non-accented counterparts
    static const std::string accents = "ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖØòóôõöøÙÚÛÜùúûüÝýÿÇç";
    static const std::string no_accents = "AAAAAAaaaaaaEEEEeeeeIIIIiiiiOOOOOOooooooUUUUuuuuYyyCc";

    size_t pos = accents.find(c);
    if (pos != std::string::npos) {
        return no_accents[pos];
    }
    else {
        return c;
    }
}

std::string strip_accents(const std::string& text) {
    std::string result;
    for (char c : text) {
        result += remove_accent(c);
    }
    return result;
}

// Function to generate the Cartesian product of word lists
template<typename T>
std::vector<std::vector<T>> cartesian_product(const std::vector<std::vector<T>>& v) {
    std::vector<std::vector<T>> s = {{}};
    for (const auto& u : v) {
        std::vector<std::vector<T>> r;
        for (const auto& x : s) {
            for (const auto& y : u) {
                auto z = x;
                z.push_back(y);
                r.push_back(z);
            }
        }
        s = std::move(r);
    }
    return s;
}

// Define the SignatureEntry struct
struct SignatureEntry {
    std::vector<int> counts;
    std::string signature_str;
    std::vector<std::string> words;
    int length;
};

class Anagrammer {
public:
    Anagrammer(const std::unordered_map<std::string, std::vector<std::string>>& signature_dict) : signature_dict(signature_dict) {
        prepare_signatures();
    }

    std::vector<std::string> find_anagrams(const std::string& phrase, int max_results = 10000, bool allow_partial = false) {
        std::string phrase_lower;
        for (char c : phrase) {
            phrase_lower += static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        std::string stripped_phrase = strip_accents(phrase_lower);
        std::vector<int> phrase_signature = word_signature(stripped_phrase);
        std::vector<std::string> results;
        std::vector<int> current_signature = phrase_signature;
        std::vector<SignatureEntry*> anagram_partial;
        size_t index = 0;
        search_anagrams(current_signature, anagram_partial, index, results, max_results, allow_partial);
        return results;
    }

private:
    void prepare_signatures() {
        for (const auto& [sig_str, words_value] : signature_dict) {
            std::vector<int> counts = string_to_signature(sig_str);
            int length = std::accumulate(counts.begin(), counts.end(), 0);
            const std::vector<std::string>& words = words_value;
            signatures.push_back({ counts, sig_str, words, length });
        }
        // Sort the signatures in decreasing order of word length
        std::sort(signatures.begin(), signatures.end(), [](const SignatureEntry& a, const SignatureEntry& b) {
            return a.length > b.length;
        });
    }

    void search_anagrams(const std::vector<int>& current_signature, std::vector<SignatureEntry*>& anagram_partial, size_t index, std::vector<std::string>& results, int max_results, bool allow_partial) {
        if (results.size() >= static_cast<size_t>(max_results)) {
            return;
        }
        bool is_zero = std::all_of(current_signature.begin(), current_signature.end(), [](int c) { return c == 0; });
        if (is_zero) {
            expand_anagram(anagram_partial, results, max_results);
            return;
        }
        bool found = false;
        for (size_t i = index; i < signatures.size(); ++i) {
            const SignatureEntry& sig_entry = signatures[i];
            const std::vector<int>& sig_counts = sig_entry.counts;
            if (is_included(sig_counts, current_signature)) {
                found = true;
                anagram_partial.push_back(const_cast<SignatureEntry*>(&sig_entry));
                std::vector<int> new_signature = subtract_signatures(sig_counts, current_signature);
                search_anagrams(new_signature, anagram_partial, i, results, max_results, allow_partial);
                anagram_partial.pop_back();
            }
        }
        if (!found && allow_partial) {
            expand_anagram(anagram_partial, results, max_results);
        }
    }

    void expand_anagram(const std::vector<SignatureEntry*>& anagram_partial, std::vector<std::string>& results, int max_results) {
        // Collect word lists
        std::vector<std::vector<std::string>> word_lists;
        for (const auto& entry : anagram_partial) {
            word_lists.push_back(entry->words);
        }
        // Generate Cartesian product
        std::vector<std::vector<std::string>> combinations = cartesian_product(word_lists);
        for (const auto& words_combo : combinations) {
            std::string anagram;
            for (size_t i = 0; i < words_combo.size(); ++i) {
                if (i > 0) {
                    anagram += " ";
                }
                anagram += words_combo[i];
            }
            if (std::find(results.begin(), results.end(), anagram) == results.end()) {
                results.push_back(anagram);
                if (results.size() >= static_cast<size_t>(max_results)) {
                    break;
                }
            }
        }
    }

    static std::vector<int> string_to_signature(const std::string& sig_str) {
        // Decode the base64 signature string back to counts vector
        std::vector<unsigned char> counts_bytes = base64_decode(sig_str);
        std::vector<int> counts(counts_bytes.begin(), counts_bytes.end());
        // Ensure the counts vector has 26 elements (one for each alphabet letter)
        if (counts.size() < 26) {
            counts.resize(26, 0);
        }
        return counts;
    }

    static std::vector<int> word_signature(const std::string& word) {
        std::vector<int> counts(26, 0);
        for (char c : word) {
            if (isalpha(static_cast<unsigned char>(c))) {
                int index = tolower(static_cast<unsigned char>(c)) - 'a';
                counts[index]++;
            }
        }
        return counts;
    }

    static bool is_included(const std::vector<int>& sig_a, const std::vector<int>& sig_b) {
        for (size_t i = 0; i < sig_a.size(); ++i) {
            if (sig_a[i] > sig_b[i]) {
                return false;
            }
        }
        return true;
    }

    static std::vector<int> subtract_signatures(const std::vector<int>& sig_a, const std::vector<int>& sig_b) {
        std::vector<int> result(sig_a.size());
        for (size_t i = 0; i < sig_a.size(); ++i) {
            result[i] = sig_b[i] - sig_a[i];
        }
        return result;
    }

    const std::unordered_map<std::string, std::vector<std::string>>& signature_dict;
    std::vector<SignatureEntry> signatures;
};

// Function to read the entire file into a string
std::string read_file_to_string(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file " << filename << "\n";
        exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Function to parse the JSON file manually
std::unordered_map<std::string, std::vector<std::string>> parse_signature_dict(const std::string& json_content) {
    std::unordered_map<std::string, std::vector<std::string>> signature_dict;
    size_t pos = 0;
    size_t length = json_content.size();

    // Skip any whitespace at the beginning
    while (pos < length && isspace(json_content[pos])) pos++;

    // Expect '{'
    if (pos >= length || json_content[pos] != '{') {
        std::cerr << "Invalid JSON format: expected '{' at the beginning.\n";
        exit(1);
    }
    pos++; // Skip '{'

    while (pos < length) {
        // Skip whitespace
        while (pos < length && isspace(json_content[pos])) pos++;
        if (pos >= length) break;

        // Check if end of object
        if (json_content[pos] == '}') {
            pos++;
            break;
        }

        // Parse key
        if (json_content[pos] != '"') {
            std::cerr << "Invalid JSON format: expected '\"' at position " << pos << ".\n";
            exit(1);
        }
        pos++; // Skip '"'
        size_t key_start = pos;
        while (pos < length && json_content[pos] != '"') pos++;
        if (pos >= length) {
            std::cerr << "Invalid JSON format: unexpected end of key.\n";
            exit(1);
        }
        std::string key = json_content.substr(key_start, pos - key_start);
        pos++; // Skip closing '"'

        // Skip whitespace
        while (pos < length && isspace(json_content[pos])) pos++;

        // Expect ':'
        if (pos >= length || json_content[pos] != ':') {
            std::cerr << "Invalid JSON format: expected ':' after key.\n";
            exit(1);
        }
        pos++; // Skip ':'

        // Skip whitespace
        while (pos < length && isspace(json_content[pos])) pos++;

        // Parse value (array of strings)
        if (pos >= length || json_content[pos] != '[') {
            std::cerr << "Invalid JSON format: expected '[' at position " << pos << ".\n";
            exit(1);
        }
        pos++; // Skip '['

        std::vector<std::string> values;
        while (pos < length) {
            // Skip whitespace
            while (pos < length && isspace(json_content[pos])) pos++;
            if (pos >= length) {
                std::cerr << "Invalid JSON format: unexpected end inside array.\n";
                exit(1);
            }

            // Check if end of array
            if (json_content[pos] == ']') {
                pos++; // Skip ']'
                break;
            }

            // Parse string value
            if (json_content[pos] != '"') {
                std::cerr << "Invalid JSON format: expected '\"' at position " << pos << ".\n";
                exit(1);
            }
            pos++; // Skip '"'
            size_t value_start = pos;
            while (pos < length && json_content[pos] != '"') {
                if (json_content[pos] == '\\') {
                    // Skip escaped characters
                    pos += 2;
                }
                else {
                    pos++;
                }
            }
            if (pos >= length) {
                std::cerr << "Invalid JSON format: unexpected end of string value.\n";
                exit(1);
            }
            std::string value = json_content.substr(value_start, pos - value_start);
            pos++; // Skip closing '"'
            values.push_back(value);

            // Skip whitespace
            while (pos < length && isspace(json_content[pos])) pos++;
            if (pos >= length) {
                std::cerr << "Invalid JSON format: unexpected end after value.\n";
                exit(1);
            }

            // Check for ',' or ']'
            if (json_content[pos] == ',') {
                pos++; // Skip ','
            }
            else if (json_content[pos] == ']') {
                pos++; // Skip ']'
                break;
            }
            else {
                std::cerr << "Invalid JSON format: expected ',' or ']' after value.\n";
                exit(1);
            }
        }

        signature_dict[key] = values;

        // Skip whitespace
        while (pos < length && isspace(json_content[pos])) pos++;

        // Check for ',' or '}'
        if (pos < length && json_content[pos] == ',') {
            pos++; // Skip ','
            continue;
        }
        else if (pos < length && json_content[pos] == '}') {
            pos++; // Skip '}'
            break;
        }
    }

    return signature_dict;
}
