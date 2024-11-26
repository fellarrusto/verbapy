from verbapy import Anagrammer
import requests
import time

# Configuration
input_str = "siamoanapoliamici"
dictionary_file = "../dictionary/wordlist95000_ITA_sign.json"
fastapi_url = "http://127.0.0.1:8051/anagram"  # FastAPI endpoint
max_results = 10000  # Define max results as a global variable

def run_fastapi_interface(fastapi_url, input_str):
    """
    Send a query to the FastAPI interface and measure the time taken.
    """
    start_time = time.time()
    response = requests.get(fastapi_url, params={"query": input_str})
    ex_time = time.time() - start_time

    if response.status_code == 200:
        return ex_time, response.json().get("anagrams", [])
    else:
        raise RuntimeError(f"Error from FastAPI: {response.status_code}, {response.text}")

def main():
    # Find anagrams using Python
    print("Starting Python execution...")
    anagram_finder = Anagrammer(dictionary_file)
    start_time = time.time()
    results_py = anagram_finder.find_anagrams(input_str, max_results=max_results, allow_partial=True)
    ex_time_py = time.time() - start_time

    print(f"Python completed in {ex_time_py:.6f}s")
    print(f"{len(results_py)} anagrams found")

    # Find anagrams using FastAPI interface
    print("Starting FastAPI execution...")
    try:
        ex_time_fastapi, results_fastapi = run_fastapi_interface(fastapi_url, input_str)
        print(f"FastAPI completed in {ex_time_fastapi:.6f}s")
        print(f"{len(results_fastapi)} anagrams found")
    except RuntimeError as e:
        print(f"FastAPI execution failed: {e}")

if __name__ == "__main__":
    main()
