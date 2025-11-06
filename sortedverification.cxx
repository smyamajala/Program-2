#include <iostream>       // For input/output streams (cout, cerr)
#include <fstream>        // For file input/output (ifstream)
#include <vector>         // For using vector data structure
#include "json.hpp"       // Include the JSON library for parsing/creating JSON

using json = nlohmann::json;  // Create shortcut for nlohmann::json
using namespace std;          // Use standard namespace to avoid std:: prefix

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
    
    // Create input file stream object to read the JSON file
    ifstream input_file(filename);
    // Check if file was successfully opened
    if (!input_file.is_open()) {
        // Print error message if file cannot be opened
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;  // Return error code 1 indicating failure
    }

    // Create JSON object to store parsed data from input file
    json data;
    try {
        // Parse the entire JSON file into the data object
        input_file >> data;  // This reads and parses JSON from the file stream
    } catch (exception& e) {
        // Handle JSON parsing errors (invalid JSON format)
        cerr << "Error: Invalid JSON in file " << filename << endl;
        return 1;  // Return error code 1 indicating failure
    }
    // Close the input file since we've read all data into memory
    input_file.close();
    
    // Create output JSON object that will contain our verification results
    json output;
    // Counter to track how many samples have consecutive inversions
    int samples_with_inversions = 0;
    
    // Extract metadata information from the input JSON
    int arraySize = data["metadata"]["arraySize"];  // Get size of each array from metadata
    int numSamples = data["metadata"]["numSamples"];  // Get number of samples from metadata
    
    // Iterate through each key-value pair in the input JSON data using iterators
    // auto it = data.begin() gets an iterator to the first element
    for (auto it = data.begin(); it != data.end(); ++it) {
        // Get the key (sample name) from current JSON element using iterator
        string sample_name = it.key();
        
        // Skip the metadata section since we only want to check sample arrays
        if (sample_name == "metadata") {
            continue;  // Skip to next iteration of the loop
        }
        
        // Extract the array data for current sample from JSON value using iterator
        vector<int> sample_array = it.value();
        
        // Create JSON object to store inversions found in current sample
        json sample_inversions;
        // Flag to track if current sample has any inversions
        bool has_inversions = false;
        
        // Loop through the sample array to check for consecutive inversions
        // Check from first element to second-to-last element
        for (int i = 0; i < sample_array.size() - 1; i++) {
            // Check if current element is greater than next element (inversion found)
            if (sample_array[i] > sample_array[i + 1]) {
                // Set flag to true since we found at least one inversion
                has_inversions = true;
                
                // Add the inversion to sample_inversions JSON object
                // Key: index as string, Value: pair [current_element, next_element]
                sample_inversions[to_string(i)] = {sample_array[i], sample_array[i + 1]};
            }
        }
        
        // If current sample has any inversions, add it to output JSON
        if (has_inversions) {
            samples_with_inversions++;  // Increment counter of samples with inversions
            // Add ConsecutiveInversions object for this sample to output
            output[sample_name]["ConsecutiveInversions"] = sample_inversions;
            // Include the entire sample array in output for reference
            output[sample_name]["sample"] = sample_array;
        }
    }
    
    // Add metadata section to output JSON with information about the verification
    output["metadata"]["arraySize"] = arraySize;  // Size of arrays from input
    output["metadata"]["file"] = filename;  // Name of input file that was checked
    output["metadata"]["numSamples"] = numSamples;  // Total samples from input
    output["metadata"]["samplesWithInversions"] = samples_with_inversions;  // Count of problematic samples
    
    // Print the final output JSON to standard output with pretty formatting
    // dump(4) formats JSON with 4-space indentation for readability
    cout << output.dump(4) << endl;
    
    return 0;  // Return 0 indicating successful program execution
}
