from verbapy import *
import subprocess, time

anagram_finder = Anagrammer(load_signature_dictionary('dictionary/wordlist95000_ITA_sign.json'))

input_str = "siamoanapoliamici"


print("Starting Python execution...")
ex_time_py = 0
start_time = time.time()
results_py = anagram_finder.find_anagrams(input_str, max_results=10000, allow_partial=True)
ex_time_py = time.time() - start_time

print(f"Python completed in {ex_time_py}s")
print(f"{len(results_py)} anagrams found")

print("Starting C++ execution...")
ex_time_cpp = 0
start_time = time.time()
results_cpp = subprocess.run(
    ["./anagrammer_int", "dictionary/wordlist95000_ITA_sign.json", input_str, "--allow_partial"],
    text=True, capture_output=True).stdout.splitlines()
ex_time_cpp = time.time() - start_time

print(f"C++ completed in {ex_time_cpp}s")
print(f"{len(results_cpp)} anagrams found")

print("Starting C++ O3 execution...")
ex_time_cpp_8bit = 0
start_time = time.time()
results_cpp_8bit = subprocess.run(
    ["./anagrammer_03", "dictionary/wordlist95000_ITA_sign.json", input_str, "--allow_partial"],
    text=True, capture_output=True).stdout.splitlines()
ex_time_cpp_8bit = time.time() - start_time

print(f"C++ O3 completed in {ex_time_cpp_8bit}s")
print(f"{len(results_cpp_8bit)} anagrams found")