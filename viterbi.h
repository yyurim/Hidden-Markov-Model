#ifndef __VITERBI_H__
#define __VITERBI_H__

#include <cmath>
#include <algorithm>

#include "universalUTT.h"

using namespace std;

// log transition probability
void prob_to_logprob(vector<vector <float> > &tp, int size);

// calculate part of continuous observation probability
// log(gaussian_weight) - (1/2)log( ((2pi)^dim) * product(i->dim)(var(i)))
float continuous_observation_pt(stateType &state, int pdf_num);

// calculate continuous observation propbability of the state
// gaussian : continuous_observation_pt - (1/2)sum(i->dim)( ( (e(i) - mean(i))^2 )/ var(i)^2 )
// gaussians with implementation-issue-fixing
float get_state_logprob(stateType state, vector<float> obs_state_pdf);

vector<vector<float> > get_observation(string path);

// viterbi
vector<int> viterbi(const fullHMM &full_hmm, const vector<vector<float> > &obs);

// find the best word sequence
// lang_w : word transition penalty
vector<string> word_sequence(const wordHMM &wordModel, map<string, map<string, float> > &bigram, vector<int> best_state_seq, float lang_w);

#endif