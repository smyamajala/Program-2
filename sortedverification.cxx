#include <iostream>       // for input/output streams (cout, cerr)
#include <fstream>        // for file input/output (ifstream)
#include <vector>         // for using vector data structure
#include "json.hpp"       // include the JSON library

using json = nlohmann::json;  // create shortcut for nlohmann::json
using namespace std;          // use standard namespace

int main(int argc, char** argv) {
    // check if correct number of command line arguments provided
    // argc should be 2
    // program name + input filename
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input.json>" << endl; // print error message to standard error
        return 1;  // return error code 1 indicating failure
    }
    
    // store the filename from command line arguments
    // define it as a variable
    string filename = argv[1];  // argv[1] is the input JSON filename
    
    // create input file stream object to read JSON file
    ifstream input_file(filename);
    
    // check if file was successfully opened
    if (!input_file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl; // print error message if file cannot be opened
        return 1;  // return error code 1 indicating failure
    }

    // create JSON object to store parsed data
    json data;
    try {
        // parse the entire JSON file into the data object
        input_file >> data;  // read and parse data from filestream
    } catch (exception& e) {
        
        // handle JSON parsing errors (invalid JSON format)
        cerr << "Error: Invalid JSON in file " << filename << endl;// print error message
        return 1;  // return error code 1 indicating failure
    }
    
    input_file.close(); // close file
    
    // create output JSON object that will contain our verification results
    json output;
    // tracker how many samples have consecutive inversions
    int samples_with_inversions = 0;
    
    // extract metadata information from the input JSON
    int arraySize = data["metadata"]["arraySize"];  // size of each array
    int numSamples = data["metadata"]["numSamples"];  // number of samples
    
    // iterate through each key-value pair in the input JSON data using iterators
    // auto it = data.begin() gets an iterator to the first element
    for (auto it = data.begin(); it != data.end(); ++it) {
        
        // get the key (sample name) from current JSON element using iterator
        string sample_name = it.key();
        
        // skip the metadata section
        if (sample_name == "metadata") {
            continue;  // skip to next iteration
        }
        
        // extract the array data for current sample
        vector<int> sample_array = it.value();
        
        // create JSON object 
        // stores inversions found in current sample
        json sample_inversions;
        // tracker if current sample has any inversions
        bool has_inversions = false;
        
        // loop through the sample array to check for consecutive inversions
        // check from first element to second-to-last element
        for (int i = 0; i < sample_array.size() - 1; i++) {
            // check if current element is greater than next element (inversion found)
            if (sample_array[i] > sample_array[i + 1]) {
                // tracker becomes true
                has_inversions = true;
                
                // add the inversion to sample_inversions JSON object
                // key: index as string, Value: pair [current_element, next_element]
                sample_inversions[to_string(i)] = {sample_array[i], sample_array[i + 1]};
            }
        }
        
        // add inversions to JSON output
        if (has_inversions) {
            samples_with_inversions++;  // increment counter of samples with inversions
            
            output[sample_name]["ConsecutiveInversions"] = sample_inversions; // add ConsecutiveInversions object for this sample to output
           
            output[sample_name]["sample"] = sample_array;  // include the entire sample array in output for reference
        }
    }
    
    // add metadata section to output JSON with information about the verification
    output["metadata"]["arraySize"] = arraySize;  // size of arrays from input
    output["metadata"]["file"] = filename;  // name of input file that was checked
    output["metadata"]["numSamples"] = numSamples;  // total samples from input
    output["metadata"]["samplesWithInversions"] = samples_with_inversions;  // count of problematic samples
    
    // Print the final output JSON to standard output
    // dump(4) formats to 4-space indentation
    cout << output.dump(4) << endl;
    
    return 0;  // Return 0 :)
}
