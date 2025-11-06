#include <iostream>       // for input/output streams (cout, cerr)
#include <fstream>        // for file input/output (ifstream)
#include <vector>         // for using vector data structure
#include <ctime>          // for timing functions (clock())
#include "json.hpp"       // include the JSON library for parsing/creating JSON
#include "insertionsort.h" // include the insertion sort algorithm
#include "mergesort.h"     // include the merge sort algorithm  
#include "quicksort.h"     // include the quick sort algorithm

using json = nlohmann::json;  // create shortcut for nlohmann::json
using namespace std;          // use standard namespace

// function to read and parse a JSON file
json readJSONFile(const string& filename) {
    // create input file stream object to read the JSON file
    ifstream input_file(filename);
    // check if file was successfully opened
    if (!input_file.is_open()) {
        // throw error if file cannot be opened
        throw runtime_error("Cannot open file: " + filename);
    }
    
    // create JSON object to store parsed data
    json data;
    try {
        // parse the entire JSON file into the data object
        input_file >> data;  //  reads and parses from filestream
    } catch (exception& e) {
        // throw error for invalid JSON format
        throw runtime_error("Invalid JSON in file: " + filename);
    }
    input_file.close(); // close input file
    return data;  // return the parsed JSON data
}

int main(int argc, char** argv) {
    // check if correct number of command line arguments provided
    // argc should be 2
    // program name + input filename
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input.json>" << endl; // print error message to standard error
        return 1;  // return error code 1 indicating failure
    }
    
    // store the filename from command line arguments
    // define a variable
    string filename = argv[1];  // argv[1] contains the input JSON filename
    
    // create object to store data that was read
    json data;
    try {
        data = readJSONFile(filename);  // read the input JSON file
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl; // print error message if file reading fails
        return 1;  // return error code 1 indicating failure
    }
    
    // print CSV header row with required column names
    cout << "Sample,InsertionSortTime,InsertionSortCompares,InsertionSortMemaccess,";
    cout << "MergeSortTime,MergeSortCompares,MergeSortMemaccess,";
    cout << "QuickSortTime,QuickSortCompares,QuickSortMemaccess" << endl;
    
    // process each sample in the JSON file
    for (auto it = data.begin(); it != data.end(); ++it) {
        string sample_name = it.key();  // get the sample name from current element
        
        // skip metadata section 
        if (sample_name == "metadata") {
            continue;  // skip to next iteration of the loop
        }
        
        // get the original array for this sample
        vector<int> original_array = it.value();  // extract the array from JSON
        
        // print the sample name for the current CSV row
        cout << sample_name << ",";
        
        // test Insertion Sort
        vector<int> insertion_array = original_array;  // create copy for insertion sort
        int insertion_compares = 0;  // counter for insertion sort comparisons
        int insertion_memaccess = 0; // counter for insertion sort memory accesses
        clock_t insertion_start = clock();  // start timing insertion sort
        InsertionSort(&insertion_array, insertion_compares, insertion_memaccess);  // run insertion sort
        clock_t insertion_end = clock();  // end timing insertion sort
        double insertion_time = double(insertion_end - insertion_start) / CLOCKS_PER_SEC;  // calculate time
        
        // output Insertion Sort results for CSV
        cout << insertion_time << "," << insertion_compares << "," << insertion_memaccess << ",";
        
        // test Merge Sort  
        vector<int> merge_array = original_array;  // create copy for merge sort
        int merge_compares = 0;  // counter for merge sort comparisons
        int merge_memaccess = 0; // counter for merge sort memory accesses
        clock_t merge_start = clock();  // start timing merge sort
        MergeSort(&merge_array, merge_compares, merge_memaccess);  // run merge sort
        clock_t merge_end = clock();  // end timing merge sort
        double merge_time = double(merge_end - merge_start) / CLOCKS_PER_SEC;  // calculate time
        
        // output Merge Sort results for CSV
        cout << merge_time << "," << merge_compares << "," << merge_memaccess << ",";
        
        // test Quick Sort
        vector<int> quick_array = original_array;  // create copy for quick sort
        int quick_compares = 0;  // counter for quick sort comparisons
        int quick_memaccess = 0; // counter for quick sort memory accesses
        clock_t quick_start = clock();  // start timing quick sort
        QuickSort(&quick_array, quick_compares, quick_memaccess);  // run quick sort
        clock_t quick_end = clock();  // end timing quick sort
        double quick_time = double(quick_end - quick_start) / CLOCKS_PER_SEC;  // calculate time
        
        // output results for CSV
        cout << quick_time << "," << quick_compares << "," << quick_memaccess << endl;
    }
    
    return 0;  // Return 0 :)
}
