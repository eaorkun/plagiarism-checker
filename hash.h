// FILE: hash.h
// This is the .h for hash.cpp for the Programming Assignment 8: Cheaters!
// written by Eralp Orkun
// 12/2/2020
// Contains .h declarations of hash table object
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

#ifndef CHEATERS_HASH_H
#define CHEATERS_HASH_H

#include <string>
#include <list>
#include <array>

using namespace std;

const double MAX_ITEM_TO_ARRAY_RATIO = 0.769; //array should be greater than 1.3 times the size of num items (piazza)

//Hash Table data structure using chaining for collisions
class HashTable {
    public:
        // Each node in each bucket list
        struct sequenceNode {
            string key;
            string fileName;
        };
        // Each bucket contains a list
        struct bucket {
            list<sequenceNode> bucketList;
        };

        //default constructor, should not be used
        HashTable();
        //constructor, initialize with initial tableSize and total num of files to be used on the HashTable
        HashTable(int tableSize, int numOfFiles);
        //destructor
        ~HashTable();
        //Inserts a node into the correct position of the hashTable
        //Pre: valid sequenceNode with a valid key
        //Post: Hashtable is updated with the new item
        void insert(const sequenceNode &item);
        //Searches for a specific sequenceNode item in the hashTable, if exact match, returns true
        //Pre: valid sequenceNode with valid key
        //Post: bool condition is returned based whether item is in tree.
        bool search(const sequenceNode &item);
        //Removes a node from the correct position of the hashTable
        //Pre: valid sequenceNode with a valid key
        //Post: removes the exact match of inserted item from tree, does nothing if item not found
        void remove(const sequenceNode &item);
        //Checks on whether hash Table needs to be resized based on a load factor and the chain threshold
        //Pre: valid HashTable
        //Post: resizes hashTable if needed, otherwise does nothing
        void checkSize() {
            if(needsResize) {
                resize();
            }
        }
    private:
        //helper function, calculates hash value based on key
        int hashOf(const string &key) const;
        //helper function, resizes table.
        void resize();
        //helper function, searches a given bucket list for item
        static list<sequenceNode>::iterator bucketSearch(bucket &curBucket, const sequenceNode &item);
        int size; //holds information of total number of buckets in hash table
        int items; //holds information on total number of nodes currently in table
        double loadFactor; //holds information on current load factor of table
        const double maxLoadFactor = MAX_ITEM_TO_ARRAY_RATIO; //sets the load factor threshold
        int maxChain; // sets the max length of chain allowed for the hash table
        bool needsResize; // updates to true if either load threshold or max chain is reached
        bucket *hashArray; // this is the hash table
};

//Determines if a number is prime and returns a bool condition
//Pre: num >= 1
//Post: bool condition is returned which reflects whether num is prime
bool isPrime(int num);
//Determines the next prime number for num, returns num if num is prime
//pre: num >= 1
//post: returns num if prime or the next prime number greater then num
int nextPrime(int num);


#endif //CHEATERS_HASH_H
