import json
import base64
import itertools
import unicodedata

class Anagrammer:
    def __init__(self, signature_dict):
        # Load the signature dictionary
        self.signature_dict = signature_dict
        # Prepare the list of signatures with their counts and associated words
        self.prepare_signatures()

    def prepare_signatures(self):
        # Create a list of signatures with counts and associated words
        self.signatures = []
        for sig_str, words in self.signature_dict.items():
            counts = self.string_to_signature(sig_str)
            length = sum(counts)
            self.signatures.append({
                'counts': counts,
                'signature_str': sig_str,
                'words': words,
                'length': length
            })
        # Sort the signatures in decreasing order of word length
        self.signatures.sort(key=lambda x: -x['length'])

    @staticmethod
    def string_to_signature(sig_str):
        # Decode the base64 signature string back to counts tuple
        counts_bytes = base64.b64decode(sig_str.encode('ascii'))
        counts = tuple(counts_bytes)
        return counts

    @staticmethod
    def strip_accents(text):
        # Normalize the text to decompose characters
        text = unicodedata.normalize('NFD', text)
        # Remove diacritical marks
        text = ''.join(
            char for char in text
            if unicodedata.category(char) != 'Mn'
        )
        return text

    @staticmethod
    def word_signature(word):
        # Convert the word to lowercase and strip accents
        word = Anagrammer.strip_accents(word.lower())
        # Compute the signature of a word as a tuple of counts of each letter
        counts = [0]*26
        for char in word:
            if char.isalpha():
                index = ord(char) - ord('a')
                counts[index] +=1
        return tuple(counts)

    @staticmethod
    def signature_to_string(counts):
        # Encode the counts tuple into a base64 string
        counts_bytes = bytes(counts)
        return base64.b64encode(counts_bytes).decode('ascii')

    @staticmethod
    def is_included(sig_a, sig_b):
        # Check if sig_a is included in sig_b (element-wise less than or equal)
        return all(a <= b for a, b in zip(sig_a, sig_b))

    @staticmethod
    def subtract_signatures(sig_a, sig_b):
        # Return sig_b - sig_a
        return tuple(b - a for a, b in zip(sig_a, sig_b))

    def find_anagrams(self, phrase, max_results=10000, allow_partial=False):
        # Strip accents from the phrase and compute its signature
        phrase = self.strip_accents(phrase.lower())
        phrase_signature = self.word_signature(phrase)
        results = []
        current_signature = phrase_signature
        anagram_partial = []
        index = 0
        # Start the recursive search
        self.search_anagrams(current_signature, anagram_partial, index, results, max_results, allow_partial)
        return results

    def search_anagrams(self, current_signature, anagram_partial, index, results, max_results, allow_partial):
        if len(results) >= max_results:
            return
        if all(count == 0 for count in current_signature):
            self.expand_anagram(anagram_partial, results, max_results)
            return
        found = False
        for i in range(index, len(self.signatures)):
            sig_entry = self.signatures[i]
            sig_counts = sig_entry['counts']
            if self.is_included(sig_counts, current_signature):
                found = True
                anagram_partial.append(sig_entry)
                new_signature = self.subtract_signatures(sig_counts, current_signature)
                self.search_anagrams(new_signature, anagram_partial, i, results, max_results, allow_partial)
                anagram_partial.pop()
        if not found and allow_partial:
            self.expand_anagram(anagram_partial, results, max_results)

    def expand_anagram(self, anagram_partial, results, max_results):
        word_lists = [entry['words'] for entry in anagram_partial]
        for words_combo in itertools.product(*word_lists):
            anagram = ' '.join(words_combo)
            if anagram not in results:
                results.append(anagram)
                if len(results) >= max_results:
                    break




def build_signature_dictionary(wordlist_filename, json_filename):
    signature_dict = {}
    with open(wordlist_filename, 'r', encoding='utf-8') as f:
        for line in f:
            word = line.strip()
            if word:
                # Strip accents from the word
                word_no_accents = Anagrammer.strip_accents(word.lower())
                # Compute the signature using the word without accents
                sig_counts = Anagrammer.word_signature(word_no_accents)
                sig_str = Anagrammer.signature_to_string(sig_counts)
                # Store the original word (with accents) in the dictionary
                signature_dict.setdefault(sig_str, []).append(word)
    # Save the signature dictionary to JSON
    with open(json_filename, 'w', encoding='utf-8') as f:
        json.dump(signature_dict, f, ensure_ascii=False)

def load_signature_dictionary(json_filename):
    with open(json_filename, 'r', encoding='utf-8') as f:
        signature_dict = json.load(f)
    return signature_dict