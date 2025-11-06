#include <iostream>       // For input/output streams (cout, cerr)
#include <fstream>        // For file input/output (ifstream)
#include <vector>         // For using vector data structure
#include <ctime>          // For timing functions (clock())
#include "json.hpp"       // Include the JSON library for parsing/creating JSON
#include "insertionsort.h" // Include the insertion sort algorithm
#include "mergesort.h"     // Include the merge sort algorithm  
#include "quicksort.h"     // Include the quick sort algorithm

using json = nlohmann::json;  // Create shortcut for nlohmann::json
using namespace std;          // Use standard namespace to avoid std:: prefix

// Function to read and parse a JSON file
json readJSONFile(const string& filename) {
    // Create input file stream object to read the JSON file
    ifstream input_file(filename);
    // Check if file was successfully opened
    if (!input_file.is_open()) {
        // Throw error if file cannot be opened
        throw runtime_error("Cannot open file: " + filename);
    }
    
    // Create JSON object to store parsed data
    json data;
    try {
        // Parse the entire JSON file into the data object
        input_file >> data;  // This reads and parses JSON from the file stream
    } catch (exception& e) {
        // Throw error for invalid JSON format
        throw runtime_error("Invalid JSON in file: " + filename);
    }
    // Close the input file since we've read all data into memory
    input_file.close();
    return data;  // Return the parsed JSON data
}

int main(int argc, char** argv) {
    // Check if correct number of command line arguments provided
    // argc should be 2: program name + input filename
    if (argc != 2) {
        // Print error message to standard error if usage is incorrect
        cerr << "Usage: " << argv[0] << " <input.json>" << endl;
        return 1;  // Return error code 1 indicating failure
    }
    
    // Store the filename from command line arguments
    string filename = argv[1];  // argv[1] contains the input JSON filename
    
    // Read the input JSON file
    json data;
    try {
        data = readJSONFile(filename);  // Read the input JSON file
    } catch (const exception& e) {
        // Print error message if file reading fails
        cerr << "Error: " << e.what() << endl;
        return 1;  // Return error code 1 indicating failure
    }
    
    // Print CSV header row with required column names
    cout << "Sample,InsertionSortTime,InsertionSortCompares,InsertionSortMemaccess,";
    cout << "MergeSortTime,MergeSortCompares,MergeSortMemaccess,";
    cout << "QuickSortTime,QuickSortCompares,QuickSortMemaccess" << endl;
    
    // Process each sample in the JSON file
    for (auto it = data.begin(); it != data.end(); ++it) {
        string sample_name = it.key();  // Get the sample name from current element
        
        // Skip metadata section since we only want to process sample arrays
        if (sample_name == "metadata") {
            continue;  // Skip to next iteration of the loop
        }
        
        // Get the original array for this sample
        vector<int> original_array = it.value();  // Extract the array from JSON
        
        // Print the sample name for the current CSV row
        cout << sample_name << ",";
        
        // Test Insertion Sort
        vector<int> insertion_array = original_array;  // Create copy for insertion sort
        int insertion_compares = 0;  // Counter for insertion sort comparisons
        int insertion_memaccess = 0; // Counter for insertion sort memory accesses
        clock_t insertion_start = clock();  // Start timing insertion sort
        InsertionSort(&insertion_array, insertion_compares, insertion_memaccess);  // Run insertion sort
        clock_t insertion_end = clock();  // End timing insertion sort
        double insertion_time = double(insertion_end - insertion_start) / CLOCKS_PER_SEC;  // Calculate time
        
        // Output Insertion Sort results for CSV
        cout << insertion_time << "," << insertion_compares << "," << insertion_memaccess << ",";
        
        // Test Merge Sort  
        vector<int> merge_array = original_array;  // Create copy for merge sort
        int merge_compares = 0;  // Counter for merge sort comparisons
        int merge_memaccess = 0; // Counter for merge sort memory accesses
        clock_t merge_start = clock();  // Start timing merge sort
        MergeSort(&merge_array, merge_compares, merge_memaccess);  // Run merge sort
        clock_t merge_end = clock();  // End timing merge sort
        double merge_time = double(merge_end - merge_start) / CLOCKS_PER_SEC;  // Calculate time
        
        // Output Merge Sort results for CSV
        cout << merge_time << "," << merge_compares << "," << merge_memaccess << ",";
        
        // Test Quick Sort
        vector<int> quick_array = original_array;  // Create copy for quick sort
        int quick_compares = 0;  // Counter for quick sort comparisons
        int quick_memaccess = 0; // Counter for quick sort memory accesses
        clock_t quick_start = clock();  // Start timing quick sort
        QuickSort(&quick_array, quick_compares, quick_memaccess);  // Run quick sort
        clock_t quick_end = clock();  // End timing quick sort
        double quick_time = double(quick_end - quick_start) / CLOCKS_PER_SEC;  // Calculate time
        
        // Output Quick Sort results for CSV
        cout << quick_time << "," << quick_compares << "," << quick_memaccess << endl;
    }
    
    return 0;  // Return 0 indicating successful program execution
}
