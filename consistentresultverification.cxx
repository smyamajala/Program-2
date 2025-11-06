#include <iostream>       // For input/output streams (cout, cerr)
#include <fstream>        // For file input/output (ifstream)  
#include <vector>         // For using vector data structure
#include <set>           // For using set data structure
#include "json.hpp"       // Include the JSON library for parsing/creating JSON

using json = nlohmann::json;  // Create shortcut for nlohmann::json
using namespace std;          // Use standard namespace to avoid std::prefix

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
    // argc should be 3: program name + first filename + second filename
    if (argc != 3) {
        // Print error message to standard error if usage is incorrect
        cerr << "Usage: " << argv[0] << " <file1.json> <file2.json>" << endl;
        return 1;  // Return error code 1 indicating failure
    }
    
    // Store the filenames from command line arguments
    string filename1 = argv[1];  // argv[1] contains the first input JSON filename
    string filename2 = argv[2];  // argv[2] contains the second input JSON filename
    
    // Read both JSON files
    json data1, data2;
    try {
        data1 = readJSONFile(filename1);  // Read first JSON file
        data2 = readJSONFile(filename2);  // Read second JSON file
    } catch (const exception& e) {
        // Print error message if file reading fails
        cerr << "Error: " << e.what() << endl;
        return 1;  // Return error code 1 indicating failure
    }
    
    // Create output JSON object that will contain our comparison results
    json output;
    int samples_with_conflicts = 0;  // Counter for samples with differences

    // Get metadata from both files for output
    int arraySize1 = data1["metadata"]["arraySize"];    // Array size from first file
    int numSamples1 = data1["metadata"]["numSamples"];  // Number of samples from first file
    int arraySize2 = data2["metadata"]["arraySize"];    // Array size from second file  
    int numSamples2 = data2["metadata"]["numSamples"];  // Number of samples from second file

    // We need to check all samples that exist in either file
    // Create a set to collect all unique sample names from both files
    set<string> all_sample_names;
    
    // Iterate through first file and collect sample names
    for (auto it = data1.begin(); it != data1.end(); ++it) {
        string sample_name = it.key();  // Get the sample name from current element
        if (sample_name != "metadata") {  // Skip metadata section
            all_sample_names.insert(sample_name);  // Add sample name to the set
        }
    }
    
    // Iterate through second file and collect sample names
    for (auto it = data2.begin(); it != data2.end(); ++it) {
        string sample_name = it.key();  // Get the sample name from current element
        if (sample_name != "metadata") {  // Skip metadata section
            all_sample_names.insert(sample_name);  // Add sample name to the set
        }
    }
    
    // Now compare each sample that exists in either file
    for (const string& sample_name : all_sample_names) {
        // Check if sample exists in both files using find() method
        bool in_file1 = (data1.find(sample_name) != data1.end()) && sample_name != "metadata";
        bool in_file2 = (data2.find(sample_name) != data2.end()) && sample_name != "metadata";
        
        // If sample missing from one file, that's a conflict
        if (!in_file1 || !in_file2) {
            samples_with_conflicts++;  // Increment conflict counter
            output[sample_name]["Mismatches"]["missing"] = "Sample missing from one file";
            continue;  // Skip to next sample
        }
        
        // Get the arrays from both files for this sample
        vector<int> array1 = data1[sample_name];  // Array from first file
        vector<int> array2 = data2[sample_name];  // Array from second file
        
        // Check if arrays have different sizes
        if (array1.size() != array2.size()) {
            samples_with_conflicts++;  // Increment conflict counter
            output[sample_name]["Mismatches"]["size"] = "Arrays have different sizes";
            continue;  // Skip to next sample
        }
        
        // Compare each element in the arrays
        json mismatches;  // JSON object to store position mismatches
        bool has_mismatches = false;  // Flag to track if we found any mismatches
        
        for (int i = 0; i < array1.size(); i++) {
            // Check if elements at position i are different
            if (array1[i] != array2[i]) {
                has_mismatches = true;  // Set flag to true
                // Store the mismatch: key is position, value is [file1_value, file2_value]
                mismatches[to_string(i)] = {array1[i], array2[i]};
            }
        }
        
        // If we found any mismatches, add to output
        if (has_mismatches) {
            samples_with_conflicts++;  // Increment conflict counter
            output[sample_name][filename1] = array1;  // Add array from first file
            output[sample_name][filename2] = array2;  // Add array from second file
            output[sample_name]["Mismatches"] = mismatches;  // Add the mismatches
        }
    }
    
    // Add metadata section to output JSON
    output["metadata"]["File1"]["name"] = filename1;  // First filename
    output["metadata"]["File1"]["arraySize"] = arraySize1;  // Array size from first file
    output["metadata"]["File1"]["numSamples"] = numSamples1;  // Sample count from first file
    output["metadata"]["File2"]["name"] = filename2;  // Second filename
    output["metadata"]["File2"]["arraySize"] = arraySize2;  // Array size from second file
    output["metadata"]["File2"]["numSamples"] = numSamples2;  // Sample count from second file
    output["metadata"]["samplesWithConflictingResults"] = samples_with_conflicts;  // Conflict count
    
    // Print the final output JSON to standard output with pretty formatting
    cout << output.dump(4) << endl;  // dump(4) for pretty printing with 4-space indent
    
    return 0;  // Return 0 indicating successful program execution
}
