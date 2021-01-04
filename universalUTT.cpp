#include"universalUTT.h"

using namespace std;

// Unigram
void setUnigram(map<string, float> &unigram){
    ifstream f("data/unigram.txt");
    while(!f.eof()){
        string line;
        string from, to;
        int count = 0;
        getline(f,line);
        stringstream ss(line);
        string temp;
        while(getline(ss, temp, '\t')){
            if(count%2 == 0){
                from = temp;
                count++;
            }
            else if(count%2 == 1){
                to = temp;
                unigram[from] = stof(temp);
                count++;
            }

        }
    }
}

// Bigram
// map[from][to] : transition prop from to
void setBigram(map<string, map<string, float> > &bigram){
    ifstream f("data/bigram.txt");
    while(!f.eof()){
        string line;
        string from, to;
        int count = 0;
        getline(f,line);
        stringstream ss(line);
        string temp;
        while(getline(ss, temp, '\t')){
            if(count%3 == 0){
                from = temp;
                count++;
            }
            else if(count%3 == 1){
                to = temp;
                count++;
            }
            else{
                bigram[from][to] = stof(temp);
                

            }
        }
    }
}

// universal utterance HMM
// bigram : word-to-word bigram
fullHMM buildFullModel(vector<wordType> &word_hmms, map<string, map<string, float> > &bigram){
    fullHMM result;

    // silence model
    wordType &silence = word_hmms[0];
    result.words.push_back(silence.word_info);
    silence.word_info.enter_state = result.state.size();
    for(int i = 0 ; i < silence.states.size() ; i++){
        result.state.push_back(silence.states[i]);
    }
    silence.word_info.exit_state = result.state.size() - 1;

    // words
    for(int w = 1 ; w < word_hmms.size() ; w++){
        
        word_hmms[w].word_info.enter_state = result.state.size();
        wordType word = word_hmms[w];
        result.words.push_back(word.word_info);
        for(int s = 0 ; s < word.states.size() ; s++){
            result.state.push_back(word.states[s]);
        }
        word_hmms[w].word_info.exit_state = result.state.size()-1;
    }
    cout << result.state.size() << endl;

    // initialize transition probability
    for(int i = 0 ; i < result.state.size() + 2 ; i++){
        vector<float> temp;
        for(int j = 0 ; j < result.state.size() + 2 ; j++){
            temp.push_back(0.);
        }
        result.tp.push_back(temp);
    }

    cout << word_hmms.size() << endl;

    // set transition prop
    // concate words with bigrams
    for(int i = 0 ; i < word_hmms.size(); i++){
        wordType &word = word_hmms[i];
        int enter = word.word_info.enter_state;
        int exit = word.word_info.exit_state;
        cout << word.word_info.word << " " << enter << " " << exit << endl;
        
        result.tp[silence.word_info.exit_state][enter] = bigram["<s>"][word.word_info.word];
        result.tp_pair.push_back(make_pair(silence.word_info.exit_state, enter));

        // zero
        if(word.word_info.word == "zero"){
            cout << "zero fixing " <<endl;
            result.tp[silence.word_info.exit_state][enter] = (0.5)*bigram["<s>"][word.word_info.word];
        }

        for(int r = enter ; r <= exit ; r++){
            for(int c = enter ; c <= exit ; c++){
                result.tp[r][c] = word.tp[r - enter][ c - enter];
            }
        }

        for(int j = 0 ; j < word_hmms.size(); j++){
            wordType &to = word_hmms[j];
            
            result.tp[exit][to.word_info.enter_state] = bigram[word.word_info.word][to.word_info.word];
            result.tp_pair.push_back(make_pair(exit, to.word_info.enter_state));

            // zero
            if(to.word_info.word == "zero"){
                cout << "zero fixing " <<endl;
                result.tp[exit][to.word_info.enter_state] = (0.5) * bigram[word.word_info.word][to.word_info.word];
            }
        }

        result.tp[exit][silence.word_info.enter_state] = bigram[word.word_info.word]["<s>"];
        result.tp_pair.push_back(make_pair(exit, silence.word_info.enter_state));
        
    }

    // transition probability from silence to word
    float silence_exit_prob = silence.tp[N_STATE][N_STATE+1];
    for(int i = silence.word_info.exit_state + 1 ; i < result.state.size() + 2 ; i++){
        result.tp[silence.word_info.exit_state][i] *= silence_exit_prob;
    }

    return result;

}


