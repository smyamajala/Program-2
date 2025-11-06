#include <iostream>       // for input/output streams (cout, cerr)
#include <fstream>        // for file input/output (ifstream)  
#include <vector>         // for using vector data structure
#include <set>           // for using set data structure
#include "json.hpp"       // include the JSON library

using json = nlohmann::json;  // create shortcut for nlohmann::json
using namespace std;          // use standard namespace

// function to read and parse a JSON file
// this function works almost identical to the main function from HW1
// parameters are the filename, which should be a string
json readJSONFile(const string& filename) { 
    ifstream input_file(filename); // create input file stream object
    
    // check if file was successfully opened
    if (!input_file.is_open()) {
        throw runtime_error("Cannot open file: " + filename); // throw error if file cannot be opened
    }
    
    // create JSON object to store parsed data
    json data;
    try {
        // parse the entire JSON file into the data object
        input_file >> data;  // read and parse from filestream
    } catch (exception& e) {
        throw runtime_error("Invalid JSON in file: " + filename); // throw error for invalid JSON format
    }
    
    input_file.close(); // clsoe file
    return data;  // return parsed data
}

int main(int argc, char** argv) {
    // check if correct number of command line arguments provided
    // argc should be 3: program name + first filename + second filename
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <file1.json> <file2.json>" << endl; // print error message to standard error
        return 1;  // return error code 1 indicating failure
    }
    
    // store filenames from command line arguments

    // define the variables of the filenames
    string filename1 = argv[1];  // argv[1] is the first input JSON filename
    string filename2 = argv[2];  // argv[2] is the second input JSON filename
    
    // read both JSON files using previously defined function
    // creating two objects to store data 
    json data1, data2;
    try {
        data1 = readJSONFile(filename1);  // read first JSON file
        data2 = readJSONFile(filename2);  // read second JSON file
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl; // print error message if file reading fails
        return 1;  // return error code 1 indicating failure
    }
    
    // create output JSON object 
    // this contains comparison results
    json output;
    int samples_with_conflicts = 0;  // counter for samples with conflictions

    // get metadata from both files for output
    int arraySize1 = data1["metadata"]["arraySize"];    // array size from first file
    int numSamples1 = data1["metadata"]["numSamples"];  // number of samples from first file
    int arraySize2 = data2["metadata"]["arraySize"];    // array size from second file  
    int numSamples2 = data2["metadata"]["numSamples"];  // number of samples from second file

    
    set<string> all_sample_names; // string vector object to hold all unique sample names
    // this makes sure we check all samples that exist in either file
    
    // iterate through first file and collect sample names
    // parameters are start and end of data
    for (auto it = data1.begin(); it != data1.end(); ++it) {
        string sample_name = it.key();  // get the sample name from current element
        if (sample_name != "metadata") {  // skip metadata section
            all_sample_names.insert(sample_name);  // add sample name to the set
        }
    }
    
    // iterate through second file and collect sample names
    // parameters are start and end of data
    for (auto it = data2.begin(); it != data2.end(); ++it) {
        string sample_name = it.key();  // get the sample name from current element
        if (sample_name != "metadata") {  // skip metadata section
            all_sample_names.insert(sample_name);  // add sample name to the set
        }
    }
    
    // compare each sample that exists in either file
    // parameters is the vector all_sample_names
    for (const string& sample_name : all_sample_names) {
        
        // check if sample exists in both files using find() method
        bool in_file1 = (data1.find(sample_name) != data1.end()) && sample_name != "metadata"; // file 1 check
        bool in_file2 = (data2.find(sample_name) != data2.end()) && sample_name != "metadata"; // file 2 check
        
        // if sample missing from one file, that's a conflict
        if (!in_file1 || !in_file2) {
            samples_with_conflicts++;  // Increment conflict counter
            output[sample_name]["Mismatches"]["missing"] = "Sample missing from one file"; // output message
            continue;  // skip to next sample
        }
        
        // get the arrays from both files for this sample
        vector<int> array1 = data1[sample_name];  // array from first file
        vector<int> array2 = data2[sample_name];  // array from second file
        
        // compare array sizes
        if (array1.size() != array2.size()) {
            samples_with_conflicts++;  // increment conflict counter
            output[sample_name]["Mismatches"]["size"] = "Arrays have different sizes"; // output message
            continue;  // skip to next sample
        }
        
        // compare each element in the arrays
        json mismatches;  // JSON object to store position mismatches
        bool has_mismatches = false;  // tracker if we found any mismatches
        
        // parameters is array1 which stores data1
        for (int i = 0; i < array1.size(); i++) {
            // check if elements at position i are different
            if (array1[i] != array2[i]) {
                has_mismatches = true;  // set tracker to true
                
                // store the mismatch 
                // key is position, value is [file1_value, file2_value]
                mismatches[to_string(i)] = {array1[i], array2[i]};
            }
        }
        
        // add to output if there are mismatches
        if (has_mismatches) {
            samples_with_conflicts++;  // increment conflict counter
            output[sample_name][filename1] = array1;  // add array from first file
            output[sample_name][filename2] = array2;  // add array from second file
            output[sample_name]["Mismatches"] = mismatches;  // add the mismatches
        }
    }
    
    // add metadata section to output JSON
    output["metadata"]["File1"]["name"] = filename1;  // first filename
    output["metadata"]["File1"]["arraySize"] = arraySize1;  // array size from first file
    output["metadata"]["File1"]["numSamples"] = numSamples1;  // sample count from first file
    output["metadata"]["File2"]["name"] = filename2;  // second filename
    output["metadata"]["File2"]["arraySize"] = arraySize2;  // array size from second file
    output["metadata"]["File2"]["numSamples"] = numSamples2;  // sample count from second file
    output["metadata"]["samplesWithConflictingResults"] = samples_with_conflicts;  // conflict count
    
    // print the final output JSON to standard output
    cout << output.dump(4) << endl;  // dump(4) for 4-space indent 
    
    return 0;  // Return 0 program works! :)
    
}
