// FILE: dir_help.cpp
// This is the main driver for Programming Assignment 8: Cheaters!
// written by Eralp Orkun
// 12/2/2020
// Contains driver program with functions to help driver program
/* Student information for project:
 *
 * Replace <NAME> with your name.
 *
 * On my honor, Eralp Orkun, this programming project is my own work
 * and I have not provided this code to any other student.
 *
 * Name: Eralp Orkun
 * email address: eaorkun@utexas.edu
 * UTEID: eao789
 * Section 5 digit ID: 16000
 * Date: 12/2/2020
 */

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "hash.h"
#include <algorithm>

using namespace std;

ifstream myfile; //read file variable
const int TOTAL_ARGUMENTS = 4; //4 arguments needed for this program
const int NON_DIRECTORY_FILES = 2; //for . and ..
const int INDEX_THRESHOLD = 4; //Array offsetting not needed before this value.
const int MINIMUM_LARGER_INDEX = 3; //2 is first index, 3 is next smallest. Need 2 different indexes for pairs


//New type used to track similarities between different files
struct pairSimilarities {
    int commonSequences{};
    string file1;
    string file2;
};

//only made so sort function in descending order is possible, not to be used outside of sort.
bool operator<(const pairSimilarities &lhs, const pairSimilarities &rhs) {
    if(lhs.commonSequences > rhs.commonSequences) {
        return true;
    }
    else {
        return false;
    }
}

//Starter code function for obtaining a vector of filenames from a directory
//Not written by me
int getdir(string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

//Counts all possible words from current file
//Pre: requires an open file at beginning of file
//Post: returns maximum number of words from file, closes file
int countWordsFromEssay() {
    string word;
    int count = 0;
    if(myfile.is_open()) {
        while(myfile >> word) {
            ++count;
        }
        myfile.close();
    }
    else {
        cout << "Error opening file" << endl;
    }
    return count;
}


//Creates a maximum estimate for hash Table Size based on length of current essay, sequence length, and total files.
//Pre: requires an open file at beginning of file, valid num for sequence length, valid num for totalFiles
//Post: returns Hash Table size estimate (always prime), closes file
int estimateHashTableSize(int sequenceNum, int totalFiles) {
    int numWords = countWordsFromEssay(); //gets num words from 1 essay
    int maxNumKeys = numWords-(sequenceNum-1); //calculates maximum total keys obtained from sequencing for this essay
    int sizeEstimate = (maxNumKeys*totalFiles); //scale up to total num of files
    int tableSize = (sizeEstimate*13)/10; // Hash table should be about 1.3 * max num keys
    if (tableSize<2){
        tableSize = 2; //in case of overflow, will resize later
    }
    tableSize = nextPrime(tableSize); // prime number
    return tableSize;
}

//Preprocess an essay (a text file) to convert upper case to lower case and remove all punctuation and
//special characters EXCEPT for the apostrophe (’). So the essay after pre-processing should be a
//string of words separated by spaces, all in all lower case.
//Pre: requires an open file at beginning of file, empty string vector
//Post: preProcesses all words in file and adds them to vector, closes file
void preProcessEssay(vector<string> &essay) {
    string word;
    if(myfile.is_open()) {
        string::iterator iter;
        while(myfile >> word) {
            for(iter = word.begin(); iter != word.end(); ++iter) {
                if(isalpha(*iter)) { //alpha characters
                    if(isupper(*iter)) {
                        *iter = tolower(*iter); //convert to lower case
                    }
                }
                else if((*iter == '\'') || (isdigit(*iter))) {
                    // do nothing if apostrophe or numbers, keep these chars
                }
                else {
                    iter = word.erase(iter); //remove
                    --iter;
                }
            }
            if(!word.empty()) {
                essay.push_back(word);
            }
        }
        myfile.close();
    }
    else {
        cout << "Error opening file" << endl;
    }
}

//Calculates the similarity array index based two file indexes for the files vector
//Pre: requires two valid files vector indexes, cannot be the same, smaller index first
//Post: returns the array index for reaching the element containing info of both files.
int calArrIndex(int smallerIndex, int largerIndex) {
    int arrayOffset = 0;
    int index;
    for(int i = largerIndex; i > INDEX_THRESHOLD; --i) {
        arrayOffset = arrayOffset+(i-INDEX_THRESHOLD); //arrayOffset scales as follows: 1, 1+2, 1+2+3, 1+2+3+4...
    }
    index = largerIndex-MINIMUM_LARGER_INDEX+(smallerIndex-NON_DIRECTORY_FILES)+arrayOffset;
    return index;
}

// Takes the command line input variable 𝑝 and parses the pre-processed essay to create a list
// of all unique 𝑝-word sequences in the essay. Then Compare the list of 𝑝-word sequences between
// every essay before the current one and counts the number of common sequences
// Pre: valid preprocessed essay, valid parse variable, valid HashTable, valid files vector, valid curFile index,
// and valid similarity array accurate for the number of files
// Post: sequences the essay and updates the similarity array for number of similarities between current essay and all
// previous essays. Also updates the HashTable by adding all words from current essay
void sequenceEssay(vector<string> &essay, int &seqNum, HashTable &hTable, vector<string> &files, int &curFile,
                   pairSimilarities *array) {
    HashTable::sequenceNode tableInput;
    for(int i = 0; i <= (int) (essay.size()-seqNum); ++i) { //for all sequences in essay
        string sequence;
        for(int j = i; j < (seqNum+i); ++j) { //for all words in sequence
            sequence += essay.at(j);
        }
        tableInput.key = sequence;
        tableInput.fileName = files[curFile];
        hTable.insert(tableInput);
        hTable.checkSize(); //check if resizing needed
        for(int k = 2; k < curFile; ++k) { //check if previous files have same sequence
            tableInput.fileName = files[k];
            int index = calArrIndex(k, curFile);
            if(array[index].file1.empty()) {
                array[index].file1 = files[k];
                array[index].file2 = files[curFile];
            }
            if(hTable.search(tableInput)) {
                ++array[index].commonSequences;
            }
        }
    }
}

// Finds the total number of unique pairs for n Number of inputs (or number of files in this case)
// In other words, it calculates the binomial calculation but only good for n choose 2.
// pre: n is >= 0
// post: returns total number of unique pairs
int findNumPairs(int n) {
    int result;
    result = (n*(n-1))/2;
    return result;
}

//Main driver for Programming Assignment 8: Cheaters!
int main(int argc, char *argv[]) {
    if(argc < TOTAL_ARGUMENTS) {
        cout << "USER ERROR: Not enough arguments." << endl;
        cout << "Please enter directory, word sequence length, and plagiarism suspicion index" << endl;
        return (0);
    }
    else {
        string dir = argv[1]; //get command line arguments
        string pVarString = argv[2];
        string mVarString = argv[3];
        int pVar = stoi(pVarString);
        int mVar = stoi(mVarString);
        if (!(pVar>0)){
            cout << "USER ERROR: Input for p-word sequences must be greater than 0" << endl;
            return (0);
        }
        vector<string> files; // holds all filenames in directory
        getdir(dir, files); // obtains filenames from directory
        int numFiles = (int) files.size()-NON_DIRECTORY_FILES; // -2 for . and ..
        if(numFiles == 0) {
            cout << "USER ERROR: directory empty" << endl;
            return (0);
        }

        myfile.open(dir+"/"+files[2]); //access first essay for estimate
        int sizeEstimate = estimateHashTableSize(pVar, numFiles);
        HashTable myHashTable(sizeEstimate, numFiles); //create hash table
        int numPairs = findNumPairs(numFiles);
        pairSimilarities *similarityArray;
        similarityArray = new pairSimilarities[numPairs]; //create similarity array
        for(int i = 0; i < numPairs; ++i) { //initialize to empty
            similarityArray[i].commonSequences = 0;
            similarityArray[i].file1 = "";
            similarityArray[i].file2 = "";
        }

        for(int i = NON_DIRECTORY_FILES; i < (int) (files.size()); i++) { //iterate through all files in directory
            vector<string> preProcessedEssay; //holds current essay
            myfile.open(dir+"/"+files[i]);
            preProcessEssay(preProcessedEssay);
            sequenceEssay(preProcessedEssay, pVar, myHashTable, files, i,
                          similarityArray);
        }

        vector<pairSimilarities> pairs;
        for(int i = 0; i < numPairs; ++i) { //move similarity into vector
            pairs.push_back(similarityArray[i]);
        }
        delete[] similarityArray;
        sort(pairs.begin(), pairs.end()); //sort

        for(auto &curPair : pairs) { //output
            if(curPair.commonSequences > mVar) {
                cout << curPair.commonSequences;
                cout << ": " << curPair.file1 << ", " << curPair.file2;
                cout << endl;
            }
        }
        return 0;
    }
}
