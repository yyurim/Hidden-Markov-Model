#ifndef __UNIVERSALUTT_H__
#define __UNIVERSALUTT_H__

#include<map>

#include"wordHMM.h"

using namespace std;

// universal utterance HMM of numbers
typedef struct _fullHMM{
    vector<wordInfo> words;
    vector<stateType> state;
    vector<vector<float> > tp;
    vector<pair<int,int> > tp_pair;
}fullHMM;

// Unigram
void setUnigram(map<string, float> &unigram);

// Bigram
// map[from][to] : transition prop from to
void setBigram(map<string, map<string, float> > &bigram);

// universal utterance HMM
// bigram : word-to-word bigram
fullHMM buildFullModel(vector<wordType> &word_hmms, map<string, map<string, float> > &bigram);


#endif

