#include "viterbi.h"

map<string, float> unigram;
map<string, map<string, float> > bigram;
wordHMM word_hmm;

int main(){

    buildWordModel(word_hmm);
    setUnigram(unigram);
    setBigram(bigram);

    fullHMM full_hmm = buildFullModel(word_hmm.numbers, bigram);

    prob_to_logprob(full_hmm.tp, full_hmm.state.size());

    vector<string> filenames;
    ifstream f("data/reference.txt");
    string line;
    getline(f, line); // #!MLF!#

    while (!f.eof()) {
        getline(f, line); // "filename.lab"
        auto pos = line.rfind(".");
        if (pos == string::npos)
            continue;
        string filename(line.substr(1, pos - 1));
        filenames.push_back(filename);

        while (line[0] != '.')
            getline(f, line);
    }
    

    // for word transition penalty
    float lang_w = -0.5;

    ofstream out("result.txt");
    out << "#!MLF!#" << endl;
    
    for (int i = 0 ; i < filenames.size() ; i++) {
        string filename = "data/"+filenames[i];
        cout << (1+i) << '/' << filenames.size() << '\t' << filename << endl;

        vector<vector<float> > obs = get_observation(filename + ".txt");
        vector<int> state_seq = viterbi(full_hmm, obs);
        vector<string> result = word_sequence(word_hmm, bigram, state_seq, lang_w);
        out << '"' << filename << ".rec\"" << endl;
        for (int j = 0 ; j < result.size() ; j++){
            string word = result[j];
            out << word << endl;
        }
        out << '.' << endl;
    }

    return 0;

}
