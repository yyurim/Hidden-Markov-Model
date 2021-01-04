#include "viterbi.h"

using namespace std;

// log transition probability
void prob_to_logprob(vector<vector <float> > &tp, int size){
    for(int r = 0 ; r < size ; r++){
        for(int c = 0 ; c <  size ; c++){
            tp[r][c] = log(tp[r][c]);
        }
    }
}

// calculate part of continuous observation probability
// log(gaussian_weight) - (1/2)log( ((2pi)^dim) * product(i->dim)(var(i)))
float continuous_observation_pt(stateType &state, int pdf_num){
    float sum = 0.0f;
    for(int s = 0 ; s < N_DIMENSION ; s++){
        sum += log(2.0 * M_PI * state.pdf[pdf_num].var[s]);
    }
    float sum_logp = log(state.pdf[pdf_num].weight) - sum*0.5;

    return sum_logp;
}

// calculate continuous observation propbability of the state
// gaussian : continuous_observation_pt - (1/2)sum(i->dim)( ( (e(i) - mean(i))^2 )/ var(i)^2 )
// gaussians with implementation-issue-fixing
float get_state_logprob(stateType state, vector<float> obs_state_pdf){
    float gaussian[N_PDF];

    // for implementation issue
    float max_prob = -numeric_limits<float>::infinity();

    for(int i = 0 ; i < N_PDF ;i++){
        float logp = 0.0f;
        for(int s = 0 ; s < N_DIMENSION ; s++){
            logp += pow(obs_state_pdf[s] - state.pdf[i].mean[s], 2) / state.pdf[i].var[s];
        }
        logp = continuous_observation_pt(state,i) - logp*0.5;
        gaussian[i] = logp;
        if(logp > max_prob) max_prob = logp;
    }

    // for implementation issue
    float state_logprob = 0.0;
    for(int i = 0 ; i < N_PDF ; i++){
        state_logprob += exp(gaussian[i] - max_prob);
    }

    return max_prob + log(state_logprob);
}

vector<vector<float> > get_observation(string path){
    vector< vector<float> > obs;
    ifstream f(path);
    int row, col;

    f >> row >> col;
    for(int r = 0  ; r < row ; r++){
        vector<float> temp;
        for(int c = 0 ; c < col ; c++){
            float t_obs;
            f >> t_obs;
            temp.push_back(t_obs);
        }

        obs.push_back(temp);
    }

    return obs;
}

// viterbi
vector<int> viterbi(const fullHMM &full_hmm, const vector<vector<float> > &obs){
    int Ns = full_hmm.state.size();
    int T = obs.size();
    
    vector<float> prev_m(Ns);   // t-1 state prop
    vector<float> m(Ns);        // t    state prop

    // psi[T][Ns]
    vector<vector<int> > psi;

    for(int r = 0 ; r < T ; r++){
        vector<int> temp;
        for(int c = 0 ; c < Ns ; c++){
            temp.push_back(0);
        }
        psi.push_back(temp);
    }



    // t = 0 : init ; log(P(s_j)) + log(P(e_0|s_j))
    // m0 : full_hmm[0][j] == log(P(state[j]))
    for (int j = 1; j < Ns; j++) {
        prev_m[j] = full_hmm.tp[0][j] + get_state_logprob(full_hmm.state[j], obs[0]);
    }

    // t = 1 to t = last observation
    for (int t = 1; t < T; t++) {
        
        for (int j = 0; j < Ns; j++) {
            // max_probability :  log(max(i) m_(t-1)(s_i)Tp(s_i->s_j))
            float max_prob = -numeric_limits<float>::infinity();
            int max_i = 0;
            for (int i = 0; i < Ns; i++) {
                float p = prev_m[i] + full_hmm.tp[i][j];

                if (p > max_prob) {
                    max_prob = p;
                    max_i = i;
                }
            }
            // m_j = max_probability + log(P(e_t|s_j))
            m[j] = max_prob + get_state_logprob(full_hmm.state[j], obs[t]);

            // the number of the state with max_probability
            psi[t][j] = max_i;
        }

        // t -> t+1 
        prev_m = m;
    }

    // find the state with max_probability
    float max_prob_T = -numeric_limits<float>::infinity();
    int max_i_T = 0;
    for (int i = 0; i < Ns; i++) {
        float p = prev_m[i];
        if (p > max_prob_T) {
            max_prob_T = p;
            max_i_T = i;
        }
    }

    // backtracking : from T-1 to 1, find state squence
    vector<int> best_state_seq;
    int j = max_i_T;
    for (int t = T - 2; t >= 0; t--) {
        int i = psi[t + 1][j];
        best_state_seq.push_back(i);
        j = i;
    }

    // most likely state squence
    reverse(best_state_seq.begin(), best_state_seq.end());

    return best_state_seq;
}

// find the best word sequence
// lang_w : word transition penalty
vector<string> word_sequence(const wordHMM &wordModel, map<string, map<string, float> > &bigram, vector<int> best_state_seq, float lang_w){
    vector<string> possible_enter_state(best_state_seq.size());
    vector<string> possible_exit_state(best_state_seq.size());

    // get the enter state/exit state of the words
    for(int s = 0 ; s < best_state_seq.size() ; s++){
        for(int i = 0 ; i < wordModel.numbers.size() ; i++){
            if(best_state_seq[s]==wordModel.numbers[i].word_info.enter_state){
                possible_enter_state[s] = wordModel.numbers[i].word_info.word;
            }
        }
        for(int i = 0 ; i < wordModel.numbers.size() ; i++){
            if(best_state_seq[s]==wordModel.numbers[i].word_info.exit_state){
                possible_exit_state[s] = wordModel.numbers[i].word_info.word;
            }
        }

    }

    vector<string> ans;
    string now ="";
    // get the word sequence
    for(int i = 0 ; i < best_state_seq.size() ; i++){
        if(now != ""){
            if(possible_exit_state[i]==now){
                ans.push_back(now);
                now = "";
            }
        }
        else{
            if(possible_enter_state[i] != ""){
                if(possible_enter_state[i]=="<s>")  continue;
                else{
                    now = possible_enter_state[i];
                }
            }
        }
    }
    cout << "Word Sequence >> ";
    for(int i = 0 ; i < ans.size() ; i++){
        cout << ans[i] << " ";
    }
    cout << endl;

    // get possible state sequences
    int to_bin = 1 << (ans.size() - 1);
    float max_prob = -numeric_limits<float>::infinity();
    string best_case;
    for(int i = 0 ; i < pow(2, ans.size()) ; i++){
        int temp = i;
        string cases;
        vector<string> possible_word;

        // for word transition penalty
        int count;
        for(int j = 0 ; j < ans.size() ; j++){
            cases.push_back(temp & to_bin ? '1' : '0');
            temp = temp << 1;
            if(cases.back()=='1'){
                count++;
            }
        }
        for(int j = 0 ; j  < ans.size() ; j++){
            if(cases[j]=='1'){
                possible_word.push_back(ans[j]);
            }
        }

        // log(P(w_1:t)) - weight*|w_1:t|
        float word_transit_prob = 0.0;
        for(int j = 1 ; j < possible_word.size() ; j++){
            word_transit_prob += log(bigram[possible_word[j-1]][possible_word[j]]);
        }
        word_transit_prob -= lang_w*count;

        if(max_prob < word_transit_prob && word_transit_prob != 0){
            max_prob = word_transit_prob;
            best_case = cases;
        }
    }

    vector<string> best_word_seq;
    cout << "Best Word Sequence >> ";
    for(int i = 0 ; i < ans.size() ; i++){
        if(best_case[i]=='1'){
            cout << ans[i] << " ";
            best_word_seq.push_back(ans[i]);
        }
    }
    cout << endl << endl;

    return best_word_seq;

}

