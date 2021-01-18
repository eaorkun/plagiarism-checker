// FILE: hash.cpp
// This is the hash.cpp for Programming Assignment 8: Cheaters!
// written by Eralp Orkun
// 12/2/2020
// Contains a hash table implementation for the programming assignment
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
//
// Created by Eralp on 11/30/2020.
//

#include "hash.h"

using namespace std;

//default constructor, should not be used
HashTable::HashTable() {
    size = 2;
    items = 0;
    maxChain = 25;
    hashArray = new bucket[size]; //new node created to be added
    for(int i = 0; i < size; ++i) {
        hashArray[i].bucketList.clear();
    }
    needsResize = false;
}

//constructor, initialize with initial tableSize and total num of files to be used on the HashTable
HashTable::HashTable(int tableSize, int numOfFiles) {
    size = tableSize;
    items = 0;
    maxChain = numOfFiles;
    hashArray = new bucket[size]; //array created
    for(int i = 0; i < size; ++i) {
        hashArray[i].bucketList.clear();
    }
    needsResize = false;
}

//destructor
HashTable::~HashTable() {
    delete[] hashArray; //delete array
    hashArray = nullptr; //point to nothing
}

//Inserts a node into the correct position of the hashTable
//Pre: valid sequenceNode with a valid key
//Post: Hashtable is updated with the new item
void HashTable::insert(const sequenceNode &item) {
    if(!search(item)) {
        int hashedIndex = hashOf(item.key);
        hashArray[hashedIndex].bucketList.push_back(item);
        ++items;
        loadFactor = (double) items/(double) size;
        if(loadFactor > maxLoadFactor || ((int)(hashArray[hashedIndex].bucketList.size()) > maxChain)) {
            needsResize = true;
        }
    }
}

// equality operator specifically made for sequenceNode in order to check for exact match
// in Hash table.
// Pre: valid sequence nodes
// post: bool condition reflecting whether sequenceNodes are identical in values
bool operator==(const HashTable::sequenceNode &lhs, const HashTable::sequenceNode &rhs) {
    if((lhs.key == rhs.key) && (lhs.fileName == rhs.fileName)) {
        return true;
    }
    else {
        return false;
    }
}

//helper function, searches a given bucket list for item
list<HashTable::sequenceNode>::iterator HashTable::bucketSearch(bucket &curBucket, const sequenceNode &item) {
    list<sequenceNode>::iterator iter;
    for(iter = curBucket.bucketList.begin(); iter != curBucket.bucketList.end(); ++iter) {
        if(*iter == item) {
            break;
        }
    }
    return iter;
}

//Searches for a specific sequenceNode item in the hashTable, if exact match, returns true
//Pre: valid sequenceNode with valid key
//Post: bool condition is returned based whether item is in tree.
bool HashTable::search(const sequenceNode &item) {
    bool found;
    list<sequenceNode>::iterator iter;
    int hashedIndex = hashOf(item.key);
    iter = bucketSearch(hashArray[hashedIndex], item);
    if(iter == hashArray[hashedIndex].bucketList.end()) {
        found = false;
    }
    else {
        found = true;
    }
    return found;
}

//Removes a node from the correct position of the hashTable
//Pre: valid sequenceNode with a valid key
//Post: removes the exact match of inserted item from tree, does nothing if item not found
void HashTable::remove(const sequenceNode &item) {
    list<sequenceNode>::iterator iter;
    int hashedIndex = hashOf(item.key);
    iter = bucketSearch(hashArray[hashedIndex], item);
    if(iter != hashArray[hashedIndex].bucketList.end()) {
        hashArray[hashedIndex].bucketList.erase(iter);
    }
    --items;
}

//helper function, calculates hash value based on key
int HashTable::hashOf(const string &key) const {
    hash<string> hashFunction;
    int hashedNum = hashFunction(key);
    hashedNum = hashedNum%size;
    if(hashedNum < 0) {
        hashedNum = -hashedNum;
    }
    return hashedNum;
}

//Determines if a number is prime and returns a bool condition
//Pre: num >= 1
//Post: bool condition is returned which reflects whether num is prime
bool isPrime(int num) {
    bool isPrime = true;
    if(num == 1) { // edge case
        isPrime = false;
        return isPrime;
    }
    for(int i = 2; i <= num/2; ++i) {
        if(num%i == 0) {
            isPrime = false;
            break;
        }
    }
    return isPrime;
}

//Determines the next prime number for num, returns num if num is prime
//pre: num >= 1
//post: returns num if prime or the next prime number greater then num
int nextPrime(int num) {
    while(!isPrime(num)) {
        ++num;
    }
    return num;
}

//helper function, resizes table.
void HashTable::resize() {
    int oldSize = size;
    bucket *oldArray = hashArray; //save previous array
    size = nextPrime((size*2)+1); //+1 since multiples of 2 are always not prime
    hashArray = new bucket[size]; //array created
    items = 0;
    loadFactor = 0;
    for(int i = 0; i < size; ++i) {
        hashArray[i].bucketList.clear();
    }
    for(int i = 0; i < oldSize; ++i) {
        if(!(oldArray[i].bucketList.empty())) {
            list<sequenceNode>::iterator iter;
            for(iter = oldArray[i].bucketList.begin(); iter != oldArray[i].bucketList.end(); ++iter) {
                insert(*iter);
            }
        }
    }
    delete[] oldArray; //delete
    oldArray = nullptr; //point to nothing
    needsResize = false;
}