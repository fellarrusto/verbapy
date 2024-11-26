from verbapy import Anagrammer
import subprocess
import time
import os

# Configuration
input_str = "siamoanapoliamici" # Define input string
max_results = 10000  # Define max results as a global variable

dictionary_file = "../dictionary/wordlist95000_ITA_sign.json"
listener_binary = "../build/anagrammer_listener"
listener_binary_O3 = "../build/anagrammer_listener_O3"

# Function to run listener and send input
def run_listener(listener_binary, dictionary_file, input_str):
    # Start the listener in the background
    listener_process = subprocess.Popen(
        [listener_binary, dictionary_file],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )
    try:
        # Measure runtime for sending input and receiving output
        start_time = time.time()
        listener_process.stdin.write(input_str + "\n")
        listener_process.stdin.flush()
        
        results = []
        while True:
            line = listener_process.stdout.readline().strip()
            if line == "END_OF_RESULT":
                break
            results.append(line)
        
        ex_time = time.time() - start_time
    finally:
        # Clean up the process
        listener_process.stdin.close()
        listener_process.terminate()
        listener_process.wait()

    return ex_time, results

def main():
    # 1) Find anagrams using Python
    print("Starting Python execution...")
    anagram_finder = Anagrammer(dictionary_file)
    start_time = time.time()
    results_py = anagram_finder.find_anagrams(input_str, max_results=max_results, allow_partial=True)
    ex_time_py = time.time() - start_time

    print(f"Python completed in {ex_time_py:.6f}s")
    print(f"{len(results_py)} anagrams found")

    # 2) Find anagrams using C++ listener
    print("Starting C++ listener execution...")
    ex_time_cpp, results_cpp = run_listener(listener_binary, dictionary_file, input_str)

    print(f"C++ listener completed in {ex_time_cpp:.6f}s")
    print(f"{len(results_cpp)} anagrams found")

    # 3) Find anagrams using C++ O3 listener
    print("Starting C++ O3 listener execution...")
    if os.path.exists(listener_binary_O3):
        ex_time_cpp_O3, results_cpp_O3 = run_listener(listener_binary_O3, dictionary_file, input_str)
        print(f"C++ O3 listener completed in {ex_time_cpp_O3:.6f}s")
        print(f"{len(results_cpp_O3)} anagrams found")
    else:
        print("C++ O3 listener binary not found. Skipping test.")

if __name__ == "__main__":
    main()
