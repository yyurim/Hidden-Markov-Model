#include "wordHMM.h"

using namespace std;

// read phones
// name : phone
int find_hmm(string name){
    if(name=="f"){
        return 0;
    }
    else if(name == "k"){
        return 1;
    }
    else if(name == "n"){
        return 2;
    }
    else if(name == "r"){
        return 3;
    }
    else if(name == "s"){
        return 4;
    }
    else if(name == "t"){
        return 5;
    }
    else if(name == "v"){
        return 6;
    }
    else if(name == "w"){
        return 7;
    }
    else if(name == "z"){
        return 8;
    }
    else if(name == "ah"){
        return 9;
    }
    else if(name == "ao"){
        return 10;
    }
    else if(name == "ay"){
        return 11;
    }
    else if(name == "eh"){
        return 12;
    }
    else if(name == "ey"){
        return 13;
    }
    else if(name == "ih"){
        return 14;
    }
    else if(name == "iy"){
        return 15;
    }
    else if(name == "ow"){
        return 16;
    }
    else if(name == "sp"){
        return 17;
    }
    else if(name == "th"){
        return 18;
    }
    else if(name == "uw"){
        return 19;
    }
    else if(name == "sil"){
        return 20;
    }
    else{
        cout << "no such phones\n";
        return -1;
    }

}

wordType constructWord(string word, vector<string> chars){
    wordType result;
    result.word_info.word = word;
    result.word_info.phones = chars;

    // silence model
    if(word == "<s>"){
        int idx = find_hmm(chars[0]);
        hmmType phone = phones[idx];
        for(int s = 0 ; s < N_STATE ; s++){
            result.states.push_back(phone.state[s]);
        }

        for(int r = 0 ; r < N_STATE + 2 ; r++){
            vector<float> tp_row;
            for(int c = 0 ; c < N_STATE + 2 ; c++){
                tp_row.push_back(phone.tp[r][c]);
            }
            result.tp.push_back(tp_row);
        }

        cout << word << " : ";
        for(int i = 0 ; i < chars.size() ; i++){
            cout << chars[i] << " ";
        }
        cout << endl;

        for(int i = 0 ; i <  N_STATE + 2  ; i++){
            for(int j = 0 ; j <  N_STATE + 2  ; j++){
                printf("%.6f ",result.tp[i][j]);
            }
            cout <<endl;
        }
        cout << "---------------------------"<<endl;

        return result;
    }

    // word model
    for(int i = 0 ; i < N_STATE*(chars.size()) ; i++){
        vector<float> tp_row;
        for(int j = 0 ; j < N_STATE*(chars.size()) ; j++){
            tp_row.push_back(0.0);
        }
        result.tp.push_back(tp_row);
    }

    result.tp[0][1] = 1;
    int offset;
    for(int i = 0 ; i < chars.size() - 1 ; i++){
        offset = 1 + N_STATE*i;
        int idx = find_hmm(chars[i]);
        hmmType phone = phones[idx];
        for(int s = 0 ; s < N_STATE ; s++){
            result.states.push_back(phone.state[s]);
        }
        
        for(int r = offset ; r < offset+ N_STATE ; r++){
            for(int c = offset ; c < offset+N_STATE+1 ; c++){
                result.tp[r][c] = phone.tp[r-N_STATE*i][c-N_STATE*i];
            }
        }
    }

    // concate optional silence to the word model
    int idx_sp = find_hmm(chars.back());
    hmmType phone_sp = phones[idx_sp];
    result.states.push_back(phone_sp.state[0]);
    
    offset = N_STATE*(chars.size()-1);
    float exit_prob = result.tp[offset][offset+1];
    for(int r = offset ; r < offset + N_STATE-1 ; r++){
        for(int c = offset+1 ; c < offset +  N_STATE  ; c++){
            result.tp[r][c] = phone_sp.tp[r-offset][c-offset];
        }
    }

    result.tp[offset][offset+1] *= exit_prob;

    cout << word << " : ";
    for(int i = 0 ; i < chars.size() ; i++){
        cout << chars[i] << " ";
    }
    cout << endl;

    for(int i = 0 ; i < N_STATE*(chars.size()) ; i++){
        for(int j = 0 ; j < N_STATE*(chars.size()) ; j++){
            printf("%.6f ",result.tp[i][j]);
        }
        cout <<endl;
    }
    cout << "---------------------------"<<endl;

    return result;
}

void buildWordModel(wordHMM &word_hmm){

    ifstream f("data/dictionary.txt");
    while(!f.eof()){
        string line;
        string word;
        vector<string> chars;
        getline(f, line);
        stringstream ss(line);
        string txt;
        int flag = 0;
        while(getline(ss,txt,' ')){
            if(flag==0){
                word = txt;
                flag = 1;
            }
            else{
                chars.push_back(txt);
            }
        }

        word_hmm.numbers.push_back(constructWord(word, chars));

    }

}

