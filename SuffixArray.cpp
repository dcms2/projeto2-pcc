#include <bits/stdc++.h>

using namespace std;

class SuffixArray {
private:
    int* sa;
    char* T; int n;
    int* Llcp;
    int* Rlcp;

    void build_sa() {
        //TODO
    }

    void build_lcp(int b, int e) {
        //TODO
    }

public:
    SuffixArray(char* T, int n) {
        this->T = (char*) malloc(n);
        strcpy(this->T, T);
        this->n = n;
        build_sa();
        this->Llcp = (int*) malloc(n);
        this->Rlcp = (int*) malloc(n);
        build_lcp(0, n-1);
    }
};

int main(){
    char T[100];
    scanf("%s", T);
    SuffixArray sa = SuffixArray(T, strlen(T));
    return 0;
}