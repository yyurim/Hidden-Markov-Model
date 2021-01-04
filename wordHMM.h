#ifndef __WORDHMM_H__
#define __WORDHMM_H__

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "hmm.h"

using namespace std;

typedef struct _wordinfo{
    string word;            // word : numbers or silence
    vector<string> phones;
    int enter_state;        // enter state of the word in HMM
    int exit_state;         // exit state of the word in HMM
}wordInfo;

// word
typedef struct _wordType{
    wordInfo word_info;             
    vector<stateType> states;       
    vector<vector<float> > tp;      // transition probability
}wordType;

// word instances
typedef struct _wordHMM{
    vector<wordType> numbers;
}wordHMM;

// read phones from hmm.h
// name : phone
int find_hmm(string name);

wordType constructWord(string word, vector<string> chars);

void buildWordModel(wordHMM &word_hmm);

#endif

